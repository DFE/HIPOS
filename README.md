The HIPOS Platform Project
==========================

HIPOS is an Open Source,
[Yocto](https://www.yoctoproject.org)/[OpenEmbedded](http://openembedded.org)
based Linux distribution for embedded devices mainly from the
[DResearch Fahrzeugelektronik GmbH](http://www.dresearch-fe.de).

For documentation on HIPOS please have a look at our
[GitHub Wiki](https://github.com/DFE/HIPOS/wiki). 

Getting Started
---------------

You need to do this only *once* for every new build environment you're setting up.

### Prerequisites

**A powerful build machine with a lot of RAM, with fast and large HDDs (SSDs preferred)**

Although OpenEmbedded builds sandboxes to be reproducible and build host independent you will need some tools to kick-start OpenEmbedded's first stage. After that OE will build an extensive set of tools for compilation and cross-compilation for itself. No matter what machine you've got, from-scratch builds will tend to take several hours (so try to build incrementally).

For required packages at your build host see [OE wiki](http://www.openembedded.org/wiki/Getting_started#Required_software).

### Set up build environment from scratch

You only need to do this once for every new from-scratch build. We've said that already, but we want to be on the safe side.

**Attention** : OpenEmbedded makes heavy use of *absolute pathnames* in all automatically generated scripts. This means that **you cannot move your build environment elsewhere after it has been initialised*. You would have to start from scratch then.

**1. get HIPOS**

```
git clone git://github.com/DFE/HIPOS.git`
```

This will clone the HIPOS project into the local sub directory `HIPOS/`. Change into this directory by issuing 

```
cd HIPOS
```

**2. run the init script**

Run (exactly like this!):
```
source ./init-open.sh .
```
in the repository root. All the git submodules (i.e. the other OE layers we're using plus an up-to-date bitbake) will be initialized and cloned. The script will also generate bblayers.conf and local.conf from their respective template files.

Building and Debugging
----------------------

tbd

Supported Machines
------------------

* **hikirk** - Marvell Kirkwood based NAS/IP Recorder hardware known as [MR4020](http://www.dresearch-fe.de/en/products/recorder/)
* **himx0294** - FreeScale i.MX6 based hardware (details coming soon)

External Resources
------------------

* [yocto Project Home](https://www.yoctoproject.org)
* [yocto Project Documentation](https://www.yoctoproject.org/documentation) (Bitbake User Manual, Developer Manual and more)
* [OpenEmbedded Home](http://openembedded.org)
* [GNU Autoconf](http://www.gnu.org/software/autoconf/manual/autoconf.html), [Automake](http://www.gnu.org/software/automake/manual/automake.html) manuals

Community
---------

If you're interested in HIPOS development just come over and say hello on our
[mailing list](https://groups.google.com/a/dresearch-fe.de/group/hipos-devel-list/topics).
