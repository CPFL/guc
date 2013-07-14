# GUC - GPU Microcontroller Compiler

GUC is a compiler for GPU microcontroller.
GUC has supporting fμc microcontrollers architecture.

#Installation Docs:

$(TOPDIR) will represent your top working directory, henceforth.
##Download

    cd $(TOPDIR)
    git clone git://0x04.net/envytools.git
    git clone git://github.com/cs005/guc.git
    
##envytools

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

##GUC install

    cd $(TOPDIR)/guc/compiler
    mkdir build
    cd build
    ../llvm/configure # if can't finished configure, add option.
    make
    sudo make install # will install tools. clang & llc

##Compile test
###Compile  
    cd $(TOPDIR)/guc/test/test_br
    make
###Execution test
    sudo make run

##Sample program
GUC have included a sample of some.
Summarize belows:

    test_br: This is a compilation test program of the branch.
    test_func: This is a compilation test program of the function.
    test_sp: This is a compilation test program of the iterative process.
    
    test_intr: The fuc has intrrupt handler. This program is using intrrupt.
    test_timer: The fuc has timer. This code is sample of using the timer intterupt.
    test_xfer: Xfer is the data transfer feature of fuc. This code is sample of using the xfer.
    
# Related research papers 
Yusuke Fujii, Takuya Azumi, Nobuhiko Nishio, Shinpei Kato. "Exploring Microcontrollers in GPUs",
to appear at the 4th Asia-Pacific Workshop on Systems(APSYS 2013), Singapore, 2013

#Licence
-----------  
 Copyright (C) Yusuke Fujii <yukke@ubi.cs.ritsumei.ac.jp>, Shinpei Kato

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

