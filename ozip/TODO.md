# userdata gc的时候的memory leak

1. GC和正常close的冲突
2. 当archive或者是file关闭两次，double free导致的coredump
