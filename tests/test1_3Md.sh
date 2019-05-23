#!/bin/bash

TEST_NAME="test1_3Md"

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
__W_B=1
__SP_B=3
__SPP_B=2
__RP_B=3
__RPP_B=2

echo $TEST_NAME

for cache_size in 16384 32768 65536 131072 262144 524288
do
	./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 $cache_size;
done

#for cache_size in 16384 32768 65536 131072 262144 524288
#do
#	valgrind --main-stacksize=83886080 --tool=cachegrind --D1=${cache_size},1,32 \
#			--LL=1073741824,1,32 --cachegrind-out-file=$TEST_NAME.$cache_size.out \
#			./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 $cache_size &>/dev/null;
#	cg_annotate $TEST_NAME.$cache_size.out | tail -8
#done