import fcntl
import os

F_SETPIPE_SZ=1031
fds = os.pipe()
for i in range(5):
    print(fcntl.fcntl(fds[0],F_SETPIPE_SZ,64))
