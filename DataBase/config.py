#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/1/6 21:35
# @Author  : wxiong
# @Site    : 
# @File    : config.py.py
# @Desc    : Undocumented .py script created by wxiong

import enum

class ConfigDB(enum.Enum):
    HOST   = 'localhost'
    PORT   = 5432
    USER   = 'weixiong'
    DB     = 'mathfin_db'

