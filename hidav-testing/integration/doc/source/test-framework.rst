.. _integration-test-framework:

Integration Test Framework
**************************
The framework allows for automati remote control operations on a device. It is
used by the integration tests to e.g. connect to a device, ito switch it on or 
off, to install packages, or to run generic commands.
                                          
.. toctree::
   :maxdepth: 2

.. digraph:: Class_diagram

     "device" -> "bcc";
     "device" -> "connection";
     "connection" -> "serial_conn";
     "connection" -> "ssh_conn";
     "logger";

Device Test
===========
.. automodule:: devicetestcase
   :members:
   :undoc-members:

Device
======
.. automodule:: device
   :members:
   :undoc-members:

Board Controller
================
.. automodule:: bcc
   :members:
   :undoc-members:

Connection
==========
.. automodule:: connection
   :members:
   :undoc-members:

SSH Connection
==============
.. automodule:: ssh_conn
   :members:
   :undoc-members:

Serial Connection
=================
.. automodule:: serial_conn
   :members:
   :undoc-members:

Logger
======
.. automodule:: logger
   :members: 
   :special-members: __new__
   :private-members: __create_instance

