#!/bin/bash

sudo mount -t efs -o tls fs-03679d242d88a3601:/ efs

make

mkdir build

mv server build