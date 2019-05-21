#!/bin/bash

TEST_NAME="test2_3Md"

__C=64
__K=64
__W=54
__H=54
__R=3
__S=3
__SIGMAW=1
__SIGMAH=1

__K_B=24
__C_B=24
__H_B=54
__W_B=3
__SP_B=3
__SPP_B=1
__RP_B=3
__RPP_B=1

echo $TEST_NAME

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 5 32768;

for cache_size in 16384 32768 65536 131072 262144 524288
do
	valgrind --main-stacksize=83886080 --tool=cachegrind --D1=${cache_size},1,32 \
			--LL=1073741824,1,32 --cachegrind-out-file=$TEST_NAME.$cache_size.out \
			./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 $cache_size &>/dev/null;
	cg_annotate $TEST_NAME.$cache_size.out | tail -8
done