#!/usr/bin/python
#
# HidaV automated test framework logging helper
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Thilo Fromm <fromm@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

""" Logging helper Package """

import logging, datetime

logger = None

def init( stdout_too = False ):
    """ Initalise logging
        @return: logger instance """

    global logger

    if logger:
        return logger

    logger = logging.getLogger( __name__  )
    logger.setLevel( logging.DEBUG )

    handler = logging.FileHandler( "run-%s.log" % datetime.datetime.now(), mode='w+')
    handler.setFormatter( logging.Formatter(
        "%(asctime)s %(levelname)s %(filename)s::%(funcName)s(): "
        + "%(message)s" ) )
    logger.addHandler( handler )

    if stdout_too:
        handler = logging.StreamHandler()
        handler.setFormatter( logging.Formatter(
            "%(asctime)s %(levelname)s %(filename)s::%(funcName)s(): "
            + "%(message)s" ) )
        logger.addHandler( handler )
    return logger
