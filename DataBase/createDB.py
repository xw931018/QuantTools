#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/1/7 01:14
# @Author  : wxiong
# @Site    : 
# @File    : createDB.py.py
# @Desc    : Create postgresql database to store financial data created by wxiong

import psycopg2
import pandas as pd
import datetime
from DataBase.config import ConfigDB
import logging
import fix_yahoo_finance as yf
import pandas_datareader as pdr

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO) # format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')

MASTER_LIST_FAILED_SYMBOLS = []

DB_KWARGS = {'db_host': ConfigDB.HOST.value,
             'db_user': ConfigDB.USER.value,
             'db_password': ConfigDB.PASSWORD.value,
             'db_name': ConfigDB.DB.value}

def check_db_exists(db_host = ConfigDB.HOST, db_user = ConfigDB.USER,
                    db_password = ConfigDB.PASSWORD, db_name = ConfigDB.DB):
    try:
        conn = psycopg2.connect(host = db_host,
                                database = db_name,
                                user = db_user,
                                password = db_password)
        cur = conn.cursor()
        cur.close()
        logger.info('[check_db_exists] Database {db} exists.'.format(db = db_name))
        conn.close()
        return True
    except:
        logger.info('[check_db_exists] Database {db} doesn\'t exist'.format(db = db_name))
        return False

def create_db(db_host, db_user, db_password, db_name):
    if check_db_exists(db_host, db_user, db_password, db_name):
        logger.info('[create_db] DB {} already exists'.format(db_name))
        pass
    else:
        logger.info('[create_db] Creating database {}'.format(db_name))
        conn = psycopg2.connect(host = db_host,
                                database = 'postgres',
                                user = db_user,
                                password = db_password)
        conn.set_isolation_level(psycopg2.extensions.ISOLATION_LEVEL_AUTOCOMMIT)
        cur = conn.cursor()
        cur.execute('CREATE DATABASE {};'.format(db_name))
        cur.close()
        conn.close()

def create_mkt_tables(db_host, db_user, db_password, db_name):
    errs = []
    if check_db_exists(db_host, db_user, db_password, db_name):
        tables = ['exchange', 'data_vendor', 'symbol', 'daily_data']
        commands = ("""
                    CREATE TABLE exchange (
                            id SERIAL PRIMARY KEY,
                            abbrev TEXT NOT NULL,
                            name TEXT NOT NULL,
                            currency VARCHAR(64) NULL,
                            created_date TIMESTAMP NOT NULL,
                            last_updated_date TIMESTAMP NOT NULL
                        )
                    """,
                    """
                    CREATE TABLE data_vendor (
                            id SERIAL PRIMARY KEY,
                            name TEXT UNIQUE NOT NULL,
                            website_url VARCHAR(255) NULL,
                            created_date TIMESTAMP NOT NULL,
                            last_updated_date TIMESTAMP NOT NULL
                        )
                    """,
                    """
                    CREATE TABLE symbol (
                           id SERIAL PRIMARY KEY,
                           exchange_id integer NULL,
                           ticker TEXT NOT NULL,
                           instrument TEXT NOT NULL,
                           name TEXT NOT NULL,
                           sector TEXT NOT NULL,
                           currency VARCHAR(64) NULL,
                           created_date TIMESTAMP NOT NULL,
                           last_updated_date TIMESTAMP NOT NULL,
                           FOREIGN KEY (exchange_id) REFERENCES exchange(id)
                        )
                    """,
                    """
                    CREATE TABLE daily_data (
                           id SERIAL PRIMARY KEY,
                           data_vendor_id INTEGER NOT NULL,
                           stock_id INTEGER NOT NULL,
                           created_date TIMESTAMP NOT NULL,
                           last_updated_date TIMESTAMP NOT NULL,
                           date_price DATE,
                           open_price  NUMERIC,
                           high_price  NUMERIC,
                           low_price   NUMERIC,
                           close_price NUMERIC,
                           adj_close_price NUMERIC,
                           volume BIGINT,
                           FOREIGN KEY (data_vendor_id) REFERENCES data_vendor(id),
                           FOREIGN KEY (stock_id) REFERENCES symbol(id)
                        )
                    """)
        for i, command in enumerate(commands):
            try:
                logger.info('[create_mkt_tables] Building table: ' + tables[i])
                conn = psycopg2.connect(host=db_host,
                                        database=db_name,
                                        user=db_user,
                                        password=db_password)
                cur = conn.cursor()
                cur.execute(command)
                conn.commit()
                cur.close()
            except (Exception, psycopg2.DatabaseError) as error:
                logger.error("[create_mkt_table] {}:  {}".format(tables[i], error))
                errs.append((tables[i], error))
                cur.close()
            finally:
                if conn:
                    conn.close()
    else:
        pass
    return errs

def parse_wiki_sp500_df():
    '''
    Parse S&P500 data frame and extract fields to insert into Symbol database
    Output: list of securities, each security is a tuple containing the extracted fields.
    '''
    now = datetime.datetime.utcnow()
    sp500 = pd.read_html('http://en.wikipedia.org/wiki/List_of_S%26P_500_companies', header = 0)[0]
    return list(sp500.apply(lambda x:
                            (x['Symbol'], 'equity', x['Security'], x['GICS Sector'], 'USD', now, now), axis = 1))

def insert_sp500_symbols_postgres(db_host = 'localhost',
                                  db_name = 'mathfin_db',
                                  db_user = 'weixiong', db_password = ''):
    '''
    Insert S&P500 symbols data frame from wiki into our Symbol database
    '''
    conn = psycopg2.connect(host = db_host,
                            database = db_name,
                            user = db_user,
                            password = db_password)
    symbols = parse_wiki_sp500_df()
    columns = '''
              ticker, instrument, name, sector, currency, created_date, last_updated_date
              '''
    insert = ", ".join(['%s']*7)
    logger.info('[insert_sp500_symbols_postgres] Saving SP500 symbols into DB')
    command_str = "INSERT INTO symbol (%s) VALUES (%s)" % (columns, insert)
    with conn:
        cur = conn.cursor()
        cur.executemany(command_str, symbols)
    if conn:
        conn.close()


def fetch_symbol_tickers(connection):
    with connection:
        cur = connection.cursor()
        cur.execute("SELECT id, ticker FROM symbol")
        data = cur.fetchall()
        return data


def insert_new_vendor(vendor_name, connection):
    now = datetime.datetime.utcnow()
    cur = connection.cursor()
    logger.info('[insert_new_vendor] Inserting vendor {} into DB'.format(vendor_name))
    cur.execute("INSERT INTO data_vendor (name, created_date, last_updated_Date) VALUES (%s, %s, %s)",
                (vendor_name, now, now))
    connection.commit()


def fetch_vendor_id(vendor_name, connection):
    cur = connection.cursor()
    cur.execute("SELECT id FROM data_vendor WHERE name = (%s)", (vendor_name,))
    vendor_id = cur.fetchall()
    return vendor_id[0][0]

def insert_yahoo_data(symbol, symbol_id, vendor_id, conn):
    cur = conn.cursor()
    start_dt = datetime.date(2004, 12, 30)
    end_dt = datetime.date.today()
    yf.pdr_override()
    try:
        data = pdr.get_data_yahoo(symbol, start_dt, end_dt)
    except:
        MASTER_LIST_FAILED_SYMBOLS.append(symbol)
        raise Exception("Failed to load {}".format(symbol))

    data.reset_index(inplace=True)
    data.rename(columns={
        'Date': 'date_price',
        'Open': 'open_price',
        'High': 'high_price',
        'Low': 'low_price',
        'Close': 'close_price',
        'Adj Close': 'adj_close_price',
        'Volume': 'volume'
    }, inplace=True)
    cols = ['data_vendor_id', 'stock_id', 'created_date',
            'last_updated_date', 'date_price', 'open_price',
            'high_price', 'low_price', 'close_price',
            'adj_close_price', 'volume']
    data['stock_id'] = symbol_id
    data['data_vendor_id'] = vendor_id
    data['created_date'] = datetime.datetime.utcnow()
    data['last_updated_date'] = datetime.datetime.utcnow()
    data = data[cols]
    data_list = data.values.tolist()
    data_tuples = [tuple(x) for x in data_list]

    logger.info('[insert_yahoo_data] Inserting Yahoo stock data into DB: {}'.format(symbol))
    insert_query = '''
                   INSERT INTO daily_data ({}) VALUES ({})
                   '''.format(', '.join(cols), ', '.join(['%s'] * len(cols)))
    cur = conn.cursor()
    cur.executemany(insert_query, data_tuples)
    conn.commit()
    logger.info('{} completed'.format(symbol))

def insert_all_symbols_yahoo(db_host, db_user, db_password, db_name):
    errs = []
    conn = psycopg2.connect(host = db_host,
                            database = db_name,
                            user = db_user,
                            password = db_password)
    cur = conn.cursor()
    vendor = 'Yahoo Finance'
    try:
        insert_new_vendor(vendor, conn)
    except:
        logger.info('Vendor {} already exists'.format(vendor))

    vendor_id = fetch_vendor_id(vendor, conn)
    symbols = fetch_symbol_tickers(conn)
    for stock in symbols:
        symbol_id = stock[0]
        symbol = stock[1]
        try:
            insert_yahoo_data(symbol, symbol_id, vendor_id, conn)
        except Exception as e:
            logger.info('[insert_all_symbols_yahoo] error occurred at {} : {}'.format(symbol, e))
            errs.append((symbol, e))
            continue
    conn.close()

def main():
    create_db(**DB_KWARGS)
    create_mkt_tables(**DB_KWARGS)
    insert_sp500_symbols_postgres(**DB_KWARGS)
    insert_all_symbols_yahoo(**DB_KWARGS)

    print(MASTER_LIST_FAILED_SYMBOLS)


# if __name__ == '__main__':
#     main()