# Samples
Sample scripts/programs in various languages

## epoll sys call in linux not working as expected

epoll_test.c forks and launches writeNo.py. It connects the stdout of child to a pipe, so that it can read from it.
writeNo.py writes a no for every second, using sleep(1).


```
epoll/
  epoll_test.c
  writeNo.py
  writeNo.c
  pipePageSize.py
  epoll_test_socket.c
```

