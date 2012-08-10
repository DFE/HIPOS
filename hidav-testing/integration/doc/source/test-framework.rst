.. _integration-test-framework:            

Integration Test Framework
**************************
The framework allows for automati remote control operations on a device. It is
used by the integration tests to e.g. connect to a device, ito switch it on or 
off, to install packages, or to run generic commands.
                                          
.. toctree::
   :maxdepth: 2

.. graphviz::

   digraph i {
     "device" -> "bcc";
     "device" -> "connection"
     "connection" -> "serial_conn";
     "connection" -> "ssh_conn";
   }

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

