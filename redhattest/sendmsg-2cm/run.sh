#!/bin/sh

S2E=/home/xqx/git/s2e1.1/build/qemu-debug/i386-s2e-softmmu/qemu


$S2E   -monitor stdio  \
/home/xqx/linux/linux-2.6.0/s2e_redhat_gcc.qcow2 \
-loadvm s2egettest -s2e-config-file test.lua    
