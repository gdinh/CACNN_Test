#!/bin/bash

echo "Test1"

__C=3
__K=96
__W=109
__H=109
__R=7
__S=7
__SIGMAW=2
__SIGMAH=2

__K_B=37
__C_B=3
__H_B=109
__W_B=1
__SP_B=3
__SPP_B=2
__RP_B=3
__RPP_B=2

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 16384;

__K_B=64
__C_B=3
__H_B=109
__W_B=1
__SP_B=3
__SPP_B=2
__RP_B=3
__RPP_B=2

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 32768;

__K_B=64
__C_B=3
__H_B=109
__W_B=3
__SP_B=3
__SPP_B=2
__RP_B=3
__RPP_B=2

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 65536;

__K_B=64
__C_B=3
__H_B=109
__W_B=6
__SP_B=3
__SPP_B=2
__RP_B=3
__RPP_B=2

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 131072;

__K_B=64
__C_B=3
__H_B=109
__W_B=12
__SP_B=3
__SPP_B=2
__RP_B=3
__RPP_B=2

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 262144;

__K_B=64
__C_B=3
__H_B=109
__W_B=24
__SP_B=3
__SPP_B=2
__RP_B=3
__RPP_B=2

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 524288;

echo "Test2"

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

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 16384;

__K_B=34
__C_B=34
__H_B=54
__W_B=5
__SP_B=3
__SPP_B=1
__RP_B=3
__RPP_B=1


./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 32768;

__K_B=49
__C_B=49
__H_B=54
__W_B=7
__SP_B=3
__SPP_B=1
__RP_B=3
__RPP_B=1


./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 65536;

__K_B=64
__C_B=64
__H_B=54
__W_B=12
__SP_B=3
__SPP_B=1
__RP_B=3
__RPP_B=1


./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 131072;

__K_B=64
__C_B=64
__H_B=54
__W_B=24
__SP_B=3
__SPP_B=1
__RP_B=3
__RPP_B=1


./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 262144;

__K_B=64
__C_B=64
__H_B=54
__W_B=48
__SP_B=3
__SPP_B=1
__RP_B=3
__RPP_B=1

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 524288;
