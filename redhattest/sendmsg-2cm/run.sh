#!/bin/sh

S2E=/home/xqx/xqx/s2e1.2/build/qemu-release/i386-s2e-softmmu/qemu-system-i386


$S2E   \
/home/xqx/xqx/img/s2e_redhat_gcc.qcow2 \
-loadvm s2eget_test2.6 -s2e-config-file test.lua -m 2048  -vnc :21 
