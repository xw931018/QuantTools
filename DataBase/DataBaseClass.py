#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/3/10 20:15
# @Author  : wxiong
# @Site    : 
# @File    : DataBaseClass.py.py
# @Desc    : Definition of DataBase class .py script created by wxiong

import pandas as pd
import datetime
import logging
import psycopg2
from DataBase.config import ConfigDB

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO) # format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')

class DataBase():
    def __init__(self, **kwargs):
        self.db_name = kwargs.pop('db_name', 'postgres')
        self.db_user = kwargs.pop('db_user', ConfigDB.USER.value)
        self.db_host = kwargs.pop('db_host', 'localhost')
        self.db_password = kwargs.pop('db_password', '')
        self.new_db  = kwargs.pop('new_db', False)
        self.reconfig(**kwargs)

    def connect(self):
        try:
            conn = psycopg2.connect(host = self.db_host,
                                    database = self.db_name,
                                    user = self.db_user,
                                    password = self.db_password)
            conn.set_isolation_level(psycopg2.extensions.ISOLATION_LEVEL_AUTOCOMMIT)
            self.connection = conn
            self.cursor = conn.cursor()
        except Exception as e:
            logger.error('DB {db_name} does not exist: {err_msg}'.format(db_name = self.db_name, err_msg = e))
            if self.new_db:
                logger.info('Creating new database {db_name}'.format(db_name = self.db_name))
                conn = psycopg2.connect(host = self.db_host,
                                    database ='postgres',
                                    user = self.db_user,
                                    password = self.db_password)
                conn.set_isolation_level(psycopg2.extensions.ISOLATION_LEVEL_AUTOCOMMIT)
                cur = conn.cursor()
                cur.execute('CREATE DATABASE {}'.format(self.db_name))
                cur.close()
                conn.close()
                conn = psycopg2.connect(host=self.db_host,
                                        database=self.db_name,
                                        user=self.db_user,
                                        password=self.db_password)
                self.connection = conn
                self.cursor = conn.cursor()
                logger.info('DB {db_name} created successfully'.format(db_name=self.db_name))
            pass

    def reconfig(self, **kwargs):
        for k, v in kwargs.items():
            setattr(self, k, v)

    def __del__(self):
        logger.info('[Delete] Closing connection and cursor...')
        if hasattr(self, 'cursor'):
            self.cursor.close()
        if hasattr(self, 'connection'):
            self.connection.close()

    def getConn(self):
        return self.connection

    def execute(self, query):
        cur = self.cursor
        cur.execute(query)

    def createTable(self, tableDict):
        logger.info('[create table] Creating table {}'.format(tableDict['tableName']))
        query = '''
                CREATE TABLE {name} (
                    {fields}
                )
                '''.format(name = tableDict['tableName'],
                           fields = ','.join([' '.join(field)
                                              for field in tableDict['fields']]))
        try:
            self.execute(query)
        except Exception as e:
            logger.info('Error while creating table {}'.format(tableDict['tableName']), e)

    def createTables(self, tableDictList):
        for tableDict in tableDictList:
            self.createTable(tableDict)

    def insertTable(self, df, table_name):
        fields = list(df)
        values = 'VALUES ({})'.format(','.join(['%s' for _ in fields]))
        insert_query = 'INSERT INTO {table} ({fields}) {values}'.format(table = table_name,
                                                                        fields = ', '.join(fields),
                                                                        values = values)
        df_list = df.values.tolist()
        df_tuples = [tuple(x) for x in df_list]
        self.cursor.executemany(insert_query, df_tuples)
        logger.info('Insert into table {} completed'.format(table_name))

    def getAllFromTable(self, table_name):
        # query = 'SELECT * FROM {}'.format(table_name)
        # self.cursor.query(query)
        # res_list = self.cursor.fetchall()
        return pd.read_sql_query('select * from "{}"'.format(table_name), con = self.connection)