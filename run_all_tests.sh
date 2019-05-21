#!/bin/bash

rm log.txt
touch log.txt

for test in tests/*
do
	$test &>> log.txt
done
