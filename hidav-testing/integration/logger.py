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
filename = None

def init( stdout_too = False ):
    """ Initalise logging
        @return: logger instance """

    global logger, filename

    if logger:
        return logger

    logger = logging.getLogger( __name__  )
    logger.setLevel( logging.DEBUG )

    filename = "run-%s.log" % datetime.datetime.now()

    handler = logging.FileHandler( filename, mode='w+')
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

