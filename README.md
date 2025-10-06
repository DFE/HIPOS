The HIPOS Platform Project
==========================

HIPOS is an Open Source,
[Yocto](https://www.yoctoproject.org)/[Poky](https://github.com/yoctoproject/poky)
based BSP and Linux distribution for embedded devices mainly from the
[iris-GmbH infrared & intelligent sensors](https://www.iris-sensing.com/).

Getting Started
---------------

### Prerequisites

**A powerful build machine with a lot of RAM, with fast and large HDDs (SSDs preferred)**

* git
* python 3.x
* [kas](https://github.com/siemens/kas) 
* A Linux-based build host with Yocto dependencies installed

For required packages at your build host see [OE wiki](http://www.openembedded.org/wiki/Getting_started#Required_software).

### Building an image

**1. get HIPOS**

```
git clone git://github.com/iris-GmbH/HIPOS.git
```

This will clone the HIPOS project into the local sub directory `HIPOS/`. Change into this directory by issuing 

```
cd HIPOS
```

**2. start the Yocto build using kas**

Run (exactly like this!):
```
kas build kas-open.yml
```
in the repository root. All configured yocto layers we're using will be initialized. The script will also generate bblayers.conf and local.conf, after which the hipos-prodimage will be built.


Debugging
---------

### build environment

**1. Enter interactive build shell**
```
kas shell kas-open.yml
```
**2. Execute a Yocto task**
```
kas shell kas-open.yml -c "MACHINE=himx0294 bitbake hipos-devimage"
```
