#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2024/3/20 上午7:45
# @Author  : wxiong
# @Site    : 
# @File    : backtesting.py.py
# @Desc    : Undocumented .py script created by wxiong

from strategy import Strategy

class BackTestingMultipleDays:
    def __init__(self, StrategyClass : Strategy, data=None, **kwargs):
        self.StrategyClass = StrategyClass
        self.data = data
        self.stratKwargs = kwargs
        self.strategiesDuringBacktestingPeriod = {}
        self.portfoliosDuringBacktestingPeriod = {}
        self.inventoriesDuringBacktestingPeriod = {}

    def backtestingStrategy(self, startDate, endDate):
        dataForBacktest = self.data.loc[startDate:endDate]
        dates = dataForBacktest.index.get_level_values(0).unique().sort_values()
        for day in dates:
            dataOneDay = dataForBacktest.loc[day]
            strategy = self.StrategyClass(intraDayData=dataOneDay, **self.stratKwargs)
            strat = strategy.generateTradingStrategy()
            strat, portfolios, inventories = strategy.backtestingTradingStrategy()
            self.strategiesDuringBacktestingPeriod[day] = strat
            self.portfoliosDuringBacktestingPeriod[day] = portfolios
            self.inventoriesDuringBacktestingPeriod[day] = inventories
            print('Date {} backtested'.format(day.date()))
        return pd.concat(self.portfoliosDuringBacktestingPeriod), pd.concat(self.inventoriesDuringBacktestingPeriod)