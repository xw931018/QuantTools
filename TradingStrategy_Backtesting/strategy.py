#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2024/3/20 上午7:44
# @Author  : wxiong
# @Site    : 
# @File    : strategy.py.py
# @Desc    : Undocumented .py script created by wxiong

import numpy as np
import pandas as pd
import datetime as dt
import matplotlib.pyplot as plt
from collections import Counter
import random
from abc import ABCMeta, abstractmethod
import seaborn as sns

## If tradeDirection < 0 (Trader to sell), price should be best buy in LOB;
## If tradeDirection > 0 (Trader to buy),  price should be best sell in LOB.

priceColumn = lambda tradeDirection: 'BP1' if tradeDirection < 0 else 'AP1'


class Strategy(metaclass=ABCMeta):
    '''This is an abstract class for generating intraday trading strategy.
       It receives data of ONE single trading day and generate trading strategy for that day.
    '''
    intraDayData: pd.DataFrame
    tradingStrategy: list
    portfolios: list
    inventories: list

    @abstractmethod
    def _tradingDecisionRuleForOneEntry(self):
        '''generate trading direction and volume for one single entry of data'''
        pass

    @abstractmethod
    def generateTradingStrategy(self):
        '''generate trading strategy specifying direction and volume at every element of data.'''
        pass

    def getIntraDayData(self, intraDayData):
        self.intraDayData = intraDayData

    def backtestingTradingStrategy(self):
        '''A generic backtesting method that can back-test a given trading strategy within ONE trading day
           Output: 1- Trading Strategy; 2- Portfolio variation of that trading day; 3- Inventory evolution of that trading day
        '''
        dataOneDay = self.intraDayData
        inventory, portfolio = 0, 0
        self.inventories, self.portfolios = [], []

        for i, (index, entry) in enumerate(dataOneDay.iloc[:-1].iterrows()):
            trade = self.tradingStrategy[i]
            pColumn = priceColumn(trade)
            currentPrice = entry['WMP']
            nextPrice = dataOneDay.iloc[i + 1]['WMP']
            portfolio += - entry[pColumn] * trade + inventory * (nextPrice - currentPrice)
            inventory += trade
            self.inventories.append(inventory)
            self.portfolios.append(portfolio)

        ## Do not forget to liquidate terminal position

        lastTrade = inventory
        entry = dataOneDay.iloc[-1]
        portfolio += entry[priceColumn(lastTrade)] * lastTrade

        # 只有经历过了backtesting之后，tradingStrategy最后一项平仓，才算得到了一个完整的intraday交易策略。
        self.tradingStrategy[-1] = -lastTrade

        self.inventories.append(0)
        self.portfolios.append(portfolio)

        self.tradingStrategyDf = pd.Series(self.tradingStrategy, index=self.intraDayData.index.get_level_values(0))
        self.portfoliosDf = pd.Series(self.portfolios, index=self.intraDayData.index.get_level_values(0))
        self.inventoriesDf = pd.Series(self.inventories, index=self.intraDayData.index.get_level_values(0))

        return self.tradingStrategyDf, self.portfoliosDf, self.inventoriesDf


class LongOnlyStrategy(Strategy):
    def __init__(self, intraDayData=None, frequency=120, unitVolume=100):
        '''A long-only strategy class.
         intraDayData : Price data of a complete trading day.
         frequency    : order posting freqeucny.
         unitVolume   : unit of a single order, if the order size is constant.'''
        self.frequency = frequency
        self.unitVolume = unitVolume
        self.intraDayData = intraDayData
        self.tradingStrategy = []

    def _tradingDecisionRuleForOneEntry(self, initialTime, entry, lastNonZeroTrade=None, features=None):
        '''Features: the features selected based on which trading decision is made.'''
        currentTime = entry['Time']
        delta = (currentTime - initialTime) / self.frequency
        remainder = (currentTime - initialTime) % self.frequency
        if delta == 0:
            return self.unitVolume
        else:
            direction = ((lastNonZeroTrade > 0) - (lastNonZeroTrade < 0)) * (-1)  # Don not forget the brackets
            if remainder == 0:
                return direction * self.unitVolume
            else:
                return 0

    def generateTradingStrategy(self, features=None):
        if self.intraDayData is None:
            raise Exception("No trading data is provided")
        self.tradingStrategy = []

        initialTime = self.intraDayData['Time'][0]
        lastNonZeroTrade = self.unitVolume
        for i, entry in self.intraDayData.iterrows():
            trade = self._tradingDecisionRuleForOneEntry(initialTime, entry, lastNonZeroTrade=lastNonZeroTrade,
                                                         features=features)
            self.tradingStrategy.append(trade)
            if trade != 0:
                lastNonZeroTrade = trade
        return self.tradingStrategy


class RandomizedStrategy(Strategy):
    def __init__(self, intraDayData=None, frequency=3, unitVolume=100):
        '''A randomized strategy class. It generates random trading direction at each time step.
         intraDayData : Price data of a complete trading day.
         frequency    : order posting freqeucny.
         unitVolume   : unit of a single order, if the order size is constant.'''
        self.frequency = frequency
        self.unitVolume = unitVolume
        self.intraDayData = intraDayData
        self.tradingStrategy = []

    def _tradingDecisionRuleForOneEntry(self, initialTime, entry, features=None):
        '''Features: the features selected based on which trading decision is made.
           Not used in benchmark trading strategies, but will be used in later feature-based RL srategies
        '''
        currentTime = entry['Time']
        remainder = (currentTime - initialTime) % self.frequency
        if remainder == 0:
            direction = random.choice([-1, 1])
            return direction * random.choice(np.arange(1, 101, 1))  # self.unitVolume
        return 0

    def generateTradingStrategy(self, features=None):
        if self.intraDayData is None:
            raise Exception("No trading data is provided")
        initialTime = self.intraDayData['Time'][0]
        self.tradingStrategy = []

        for i, entry in self.intraDayData.iterrows():
            trade = self._tradingDecisionRuleForOneEntry(initialTime, entry, features=features)
            self.tradingStrategy.append(trade)

        return self.tradingStrategy

