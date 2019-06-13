import sys
import time
import os
#f=open("aFifo",'r')
for x in range(10):
    #try:
    #    val = f.read(2)
    #except Exception as e:
    #    raise 
    time.sleep(1)
    print(f'{x:03d}')
