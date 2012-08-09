.. _integration-test-framework:            

Integration Test Framework
**************************
The framework allows for automati remote control operations on a device. It is
used by the integration tests to e.g. connect to a device, ito switch it on or 
off, to install packages, or to run generic commands.
                                          
.. toctree::
   :maxdepth: 2

.. warning::
   TODO: class diagram

Device
======
.. automodule:: device

Class Members
^^^^^^^^^^^^^^^^^^^^
.. autoclass:: Device
   :members:
   :undoc-members:

Board Controller
================
.. automodule:: bcc

Class Members
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. autoclass:: Bcc
   :members:
   :undoc-members:

Connection
==========
.. automodule:: connection

Class Members
^^^^^^^^^^^^^^^^^^^^^^^^
.. autoclass:: Connection
   :members:
   :undoc-members:

SSH Connection
==============
.. automodule:: ssh_conn

Class Members
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. autoclass:: SshConn
   :members:
   :undoc-members:

Serial Connection
=================
.. automodule:: serial_conn

Class Members
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. autoclass:: SerialConn
   :members:
   :undoc-members:

