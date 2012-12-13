guc
===

#
# GUC - GPU Microcontroller Compiler
# 
# README
# 
# Copyright (C) 2012 Yusuke Fujii <yukke@ubi.cs.ritsumei.ac.jp>
# Copyright (C) 2011-2012 Shinpei Kato
# 
#  
#

GUC is a compiler for GPU microcontroller.

Installation Docs:

$(TOPDIR) will represent your top working directory, henceforth.

1. Download

cd $(TOPDIR)
git clone git://0x04.net/envytools.git
git clone git://github.com/cs005/guc.git

2. envytools

envytools is a rich set of open-source tools to compile or decompile
NVIDIA GPU program code, firmware code, macro code, and so on. It is
also used to generate C header files with GPU command definitions.
In addition, envytools document the NVIDIA GPU architecture details,
while are not disclosed to the public so far. If you are interested 
in GPU system software development, this is what you should read!
Please follow the instruction below to install envytools.

cd $(TOPDIR)/envytools
mkdir build
cd build
cmake .. # may require some packages on your distro
make
sudo make install # will install tools to /usr/local/{bin,lib}

3. GUC install

cd $(TOPDIR)/guc/compiler
mkdir build
cd build
../llvm/configure # if can't finished configure, add option.
make
sudo make install # will install tools. clang & llc

4. Compile test
