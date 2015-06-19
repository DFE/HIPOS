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

### Initialize your build environment

**1. Build environment is set up**
If you did not set up your build environment and you need to start from scratch please work through **Getting started** first.

**2. A command shell is open in the repository root**

You cloned the HIPOS repository in the first step, and you `cd`'d into the directory created.

**3. source `setup-build-open.sh`**

If you re-visit the HIPOS folder later, you need to execute this script in order to re-init the build environment.

The script will respond:
<pre>
### Shell environment set up for builds. ###
...
</pre>

**Note** that this script will put you in the subdirectory `build-open/`. This is your work / scratch directory.

You're now ready to bitbake your first HIPOS target. Do not build one of the targets mentioned in the setup script. Instead, bake a real HIPOS image like that:

```
bitbake hipos-devimage
```

Because the first build takes a really long time, it's now a good idea to inspect the created HIPOS sub-folders and recipes.

### Select a machine
The target machine is determined by the value assigned to `MACHINE` in `build/conf/local.conf`.
This configuration file is automatically generated from `build/conf/local.conf.template` and an optional file
`build/conf/private.conf` containing private settings.

The default machine should be set in the template, temporary changes should be made in private.conf or by setting the `MACHINE` environment variable.

You need to execute `hipos-init.sh` to apply your changes in one of configuration files.

NOTE:
Do not make any changes in `build/conf/local.conf` itself as they will be discarded the next time `hipos-init.sh` is executed.

Supported machines are
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
