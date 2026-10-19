# The HIPOS Platform Project

HIPOS is an open-source BSP and Linux distribution for embedded devices, based on [Yocto](https://www.yoctoproject.org/) / [Poky](https://github.com/yoctoproject/poky).

It is primarily developed for embedded devices from [iris-GmbH infrared & intelligent sensors](https://www.iris-sensing.com/).

## Getting Started

### Prerequisites

A powerful build machine with sufficient RAM and fast, large storage (SSDs recommended).

Required software:

- Git
- Python 3
- [kas](https://github.com/siemens/kas)
- A Linux-based build host with the required Yocto dependencies installed

For the required packages, see the [OpenEmbedded Wiki](https://www.openembedded.org/wiki/Getting_started#Required_software).

### Building an Image

#### 1. Clone HIPOS

```bash
git clone https://github.com/iris-GmbH/HIPOS.git
cd HIPOS
```

#### 2. Start the Yocto Build Using kas

Run the following command from the repository root:

```bash
kas build kas-open.yml
```

All configured Yocto layers will be initialized automatically. The build process also generates `bblayers.conf` and `local.conf`, and then builds the `hipos-prodimage` image.

## Debugging

### Enter an Interactive Build Shell

```bash
kas shell kas-open.yml
```

### Execute a Yocto Task

For example, to build `hipos-devimage` for the `himx0294` machine:

```bash
kas shell kas-open.yml -c "MACHINE=himx0294 bitbake hipos-devimage"
```

## kas-container

### Prerequisites

- Git
- Docker or Podman

### Building an Image with kas-container

#### 1. Initialize and Update Git Submodules

```bash
git submodule update --init
```

#### 2. Start the Yocto Build Using kas-container

```bash
./kas-container build kas-open.yml
```

### Useful kas-container Commands

#### Enter an Interactive Build Shell

```bash
./kas-container shell kas-open.yml
```
