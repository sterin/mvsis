# MVSIS 1.3

This repository is a mirror of the [original](https://embedded.eecs.berkeley.edu/mvsis/) MVSIS code. It contains a few modifications to make it easier to compile on a modern system.

The original README of MVSIS 1.3 is included in the file name README (without the .md extension)

## Caveats

MVSIS 1.3 has not been maintained since 2005. It is therefore somewhat tricky to compile on a modern system.

Another problems is that the some parts of the code may not have been ported fully to 64 bit. It may be a good idea to compile for 32 bit first, validate the results and only then compile for 64 bit and compare the results.

## Building on a modern x86_64 Linux machine

The following steps have been tested on Ubuntu 14.04 to 17.04. If you have a different system, it would be a good starting step. The main differences would be the method of installing 32 bit support on a modern 64 bit system and the method of installing 32 bit version of libreadline library and headers.

1. Make sure 32 bit packages can be installed 

```shell
sudo  dpkg --add-architecture i386
```

2. Install the required packages:


```shell
sudo apt-get install -y git build-essential autoconf gcc-multilib libreadline-dev:i386
```

3. Clone the MVSIS repository:

```shell
git clone https://github.com/sterin/mvsis.git
```

4. Configure

```shell
autoconf
./configure
```

5. Build

```shell
make CFLAGS='-m32 -O3' LDFLAGS='-m32 -O3'
```
