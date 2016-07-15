#!/bin/bash

for FILE in *.rpm
do
rpm2cpio $FILE | cpio -idmv
done
