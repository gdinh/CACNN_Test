#!/bin/bash

TEST_NAME="test1_3Mu"

__C=3
__K=96
__W=109
__H=109
__R=7
__S=7
__SIGMAW=2
__SIGMAH=2

__K_B=38
__C_B=3
__H_B=109
__W_B=2
__SP_B=4
__SPP_B=2
__RP_B=4
__RPP_B=2

echo $TEST_NAME

for cache_size in 16384 32768 65536 131072 262144 524288
do
	./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 $cache_size;
done