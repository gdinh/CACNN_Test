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


__K_B=5
__C_B=2
__H_B=18
__W_B=3
__SP_B=3
__SPP_B=2
__RP_B=3
__RPP_B=1

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 1024;

__K_B=7
__C_B=3
__H_B=26
__W_B=3
__SP_B=3
__SPP_B=2
__RP_B=3
__RPP_B=1

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 2048;

__K_B=10
__C_B=3
__H_B=36
__W_B=3
__SP_B=3
__SPP_B=2
__RP_B=3
__RPP_B=1

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 4096;

__K_B=18
__C_B=3
__H_B=65
__W_B=2
__SP_B=3
__SPP_B=2
__RP_B=3
__RPP_B=2

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 8192;

echo "Test2"

__C=64
__K=64
__W=54
__H=54
__R=3
__S=3
__SIGMAW=1
__SIGMAH=1

__K_B=6
__C_B=6
__H_B=18
__W_B=3
__SP_B=3
__SPP_B=1
__RP_B=3
__RPP_B=1

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 1024;

__K_B=8
__C_B=8
__H_B=26
__W_B=3
__SP_B=3
__SPP_B=1
__RP_B=3
__RPP_B=1

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 2048;


__K_B=12
__C_B=12
__H_B=36
__W_B=3
__SP_B=3
__SPP_B=1
__RP_B=3
__RPP_B=1

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 4096;


__K_B=17
__C_B=17
__H_B=52
__W_B=3
__SP_B=3
__SPP_B=1
__RP_B=3
__RPP_B=1

./measure $__C $__K $__W $__H $__R $__S $__SIGMAW $__SIGMAH $__C_B $__K_B $__W_B $__H_B $__RP_B $__RPP_B $__SP_B $__SPP_B 1 8192;
