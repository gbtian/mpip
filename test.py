#!/usr/bin/env python
import os

os.system('sudo insmod mpip.ko')
os.system('sudo rmmod mpip')
os.system('dmesg && dmesg --clear')
