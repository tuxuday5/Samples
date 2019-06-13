# Samples
Sample scripts/programs in various languages

##### epoll sys call in linux not working as expected

```
epoll/
  epoll_test.c
  writeNo.py
```

epoll_test.c forks and launches writeNo.py. It connects the stdout of child to a pipe, so that it can read from it.
writeNo.py writes a no for every second, using sleep(1).


**Expected behaviour**

As the child writes a no every second, the parent should read it from the pipe and write a no every second to stdout.

**Actual behaviour**

The parent waits till the child writes all the nos, and then reads all data from pipe and writes to stdout. Verified by doing strace on parent. It blocks in epoll_wait.

**How is epoll_test.c implemented**

epoll_test uses epoll linux syscall to do i/o multiplex, though there is no multiplexing in this simple program.
epoll creates pipe() for attaching to child stdout.
- forks()
  - parent
    - sets up epoll()
    - does epoll_wait(events)
    - if events contains child stdout, then child wrote data
    - read data from the descriptor and write to stdout
    - exit if HUP is received.
  - child
    - exec writeNo.py # passed as arg.

**How to run**

gcc  epoll_test.c
./a.out /usr/bin/python3.7 writeNo.py

If you don't have python3.7 modify writeNo.py print statement accordingly. 

```print "{0:03d}".format(x)```

**My system**

```
$ uname -a
Linux linus 4.15.0-51-generic #55-Ubuntu SMP ...
```
