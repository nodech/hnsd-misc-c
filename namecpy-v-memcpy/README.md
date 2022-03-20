#

## Run

```
  $ make                   # just normal gcc files..
  $ CC=gcc CFLAGS=-O2 make # use optimizers
  $ ./bench # Run tests
  $ make clean # remove generated bench
  
  $ make clean && make && ./bench # Simple do everything at once.
  $ make clean && CC=gcc CFLAGS=-O2 make && ./bench # Simple do everything at once.
```
