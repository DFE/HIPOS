#!/usr/bin/python -tt
# -*- coding: utf-8 -*-
"""
Created on Thu Aug  9 09:43:34 2012

@author: binschek
"""

import sys
import time
import unittest
import device
import threading

import logger


class DeviceTestCase(unittest.TestCase):

    __dev = None
    __devsem = threading.Lock()
    
    @classmethod
    def get_device(cls):
        if not cls.__dev:
            cls.__devsem.acquire()
            if not cls.__dev:
                cls.__create_device()
            cls.__devsem.release()
        return cls.__dev

    @classmethod        
    def __create_device(cls):
        """ boot HidaV-divice to NAND """
        cls.__dev = device.Device( devtype = "hidav" )
        print "Boot to NAND ..."
        cls.__dev.conn._serial.boot_to_nand(sync = False,
                                      kernel_partition = None,
                                      rootfs_partition = None )

    def __init__(self, *args, **kwargs):
        """ open unit-test """
        super(DeviceTestCase, self).__init__(*args, **kwargs)
        self.dev = DeviceTestCase.get_device()
        self.logger = logger.init()


