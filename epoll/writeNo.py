#!/usr/bin/python3.7 -u
import time

pageSize=3

for x in range(100):
    time.sleep(0.5)
    print(f'{x:-{pageSize}d}',flush=True)
