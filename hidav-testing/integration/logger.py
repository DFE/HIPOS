#!/usr/bin/python -tt
# -*- coding: utf-8 -*-
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

import logging
import datetime
import threading

class Logger(logging.Logger):
    """ The Logger class singleton. 
        This class will create exactly one logger for the whole test suite.
    """

    __instance = None
    __lock     = threading.Lock()


    @classmethod
    def __new__(cls, *args, **kwargs):
        """ This method returns the logger singleton or creates one if none is a
            vailable as of yet.

            This class method is called by the python runtime to create an
            instance. It will be run transparently to any user instantiating
            a logger simply by
            
            .. code-block:: python
                
                loggy = Logger()
                

            :return: logger instance 
        """

        if not Logger.__instance:
            Logger.__lock.acquire()
            try:
                if not Logger.__instance:
                    Logger.__instance = cls.__create_instance()
            finally:
                Logger.__lock.release()

        return Logger.__instance


    @classmethod
    def __create_instance(cls):
        """ This method actually instantiates the logger handler. 

            :return: logger instance
        """
        logger = logging.getLogger("Hidav-Integration")
        logger.setLevel(logging.DEBUG)

        filename = "run-%s.log" % datetime.datetime.now()

        handler = logging.FileHandler(filename, mode='w+')
        handler.setFormatter(logging.Formatter(
            "%(asctime)s %(levelname)s %(filename)s::%(funcName)s(): "
            + "%(message)s"))
        logger.addHandler(handler)

        return logger


def init():
    """ Legacy interface to get a logger instance. """
    return Logger()
    