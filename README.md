# CS-351-Systems-Programming
## Lab-Content
 - One Introduction session of C Programming language by implementing a chainning hashtable
 - Three Computer labs directlty coming from **CSAPP:** *Computer Systems: A Programmer's Perspective 3rd* 
 
 > *Hashtable Implementation w/o worry about design hash function*
 
 > *Simple Tiny Shell, Cache Simulation, Malloc Lab*
 
## Miscellaneous
### Feel free to download or take my code as your referrences that does not mean I encourage you to cheat on homework
  - 01 Folder and related material are git -commit -push -clone command tutorial from my teacher Michael Saelee
  for more info please read <http://moss.cs.iit.edu/cs351/mp1.html>
  - 02 folder is Hashtable Implementation related stuff.
  <http://moss.cs.iit.edu/cs351/mp2.html>
  ```
  hashtable.c  // core function implementations are covered in this material 
  // after TYPE make to compile, please remeber to type commands below one after the other to double check the correctness
  make test0? make diff0?
  
  Error Message example in below:
  
  make diff02
  14,16c14,16
  < Num buckets = 7
  < Max chain length = 4
  < Avg chain length = 2.33
  ---
  > Num buckets = 5
  > Max chain length = 3
  > Avg chain length = 1.67
  make: *** [diff02] Error 1
  
  // the final command to check memory leak 
  make leakcheck
  
  
  ==23==
  ==23== HEAP SUMMARY:
  ==23==     in use at exit: 0 bytes in 0 blocks
  ==23==   total heap usage: 40,047 allocs, 40,047 frees, 1,180,036 bytes allocated
  ==23==
  ==23== All heap blocks were freed -- no leaks are possible
  ==23==
  ==23== For counts of detected and suppressed errors, rerun with: -v
  ==23== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
  ```
  - 03 TO 05 folder are CSAPP labs, you can find similar material from other repos, I only tell you how to compile from commandline 
  <http://moss.cs.iit.edu/cs351/mp3.html>
  
  ```
  tsh.c  // core function implementations
  
  compile command
  
  
  $ ./sdriver.pl -h
  Usage: sdriver.pl [-hv] -t <trace> -s <shellprog> -a <args>
  Options:
  -h            Print this message
  -v            Be more verbose
  -t <trace>    Trace file
  -s <shell>    Shell program to test
  -a <args>     Shell arguments
  -g            Generate output for autograder
 
  
 Shell driver on your shell using trace file trace01.txt (for instance) by typing:

  $ ./sdriver.pl -t trace01.txt -s ./tsh -a "-p"
  (the -a "-p" argument tells your shell not to emit a prompt), or

  $ make test01
  Compare your result with the reference shell, you can run the trace driver on the reference shell by typing:

  $ ./sdriver.pl -t trace01.txt -s ./tshref -a "-p"
  or

  $ make rtest01
  For your reference, tshref.out gives the output of the reference solution on all races. This might be more convenient for you than    
  manually running the shell driver on all trace files
   
  $ make test15
  ./sdriver.pl -t trace15.txt -s ./tsh -a "-p"
  #
  # trace15.txt - Putting it all together
  #
  tsh> ./bogus
  ./bogus: Command not found.
  tsh> ./myspin 10
  Job (9721) terminated by signal 2
  tsh> ./myspin 3 &
  [1] (9723) ./myspin 3 &
  tsh> ./myspin 4 &
  [2] (9725) ./myspin 4 &
  tsh> jobs
  [1] (9723) Running    ./myspin 3 &
  [2] (9725) Running    ./myspin 4 &
  tsh> fg %1
  Job [1] (9723) stopped by signal 20
  tsh> jobs
  [1] (9723) Stopped    ./myspin 3 &
  [2] (9725) Running    ./myspin 4 &
  tsh> bg %3
  %3: No such job
  tsh> bg %1
  [1] (9723) ./myspin 3 &
  tsh> jobs
  [1] (9723) Running    ./myspin 3 &
  [2] (9725) Running    ./myspin 4 &
  tsh> fg %1
  tsh> quit
  ```
  
  <http://moss.cs.iit.edu/cs351/m4.html>
  ```
  linux> make
  $ ./driver.py
  Part A: Testing cache simulator
  Running ./test-csim
  traces/yi.trace: No such file or directory
  Error running reference simulator: Status 1
                        Your simulator     Reference simulator
  Points (s,E,b)    Hits  Misses  Evicts    Hits  Misses  Evicts
     3 (1,1,1)       9       8       6       9       8       6  traces/yi2.trace
     0 (4,2,4)      -1      -1      -1      -1      -1      -1  traces/yi.trace
     3 (2,1,4)       2       3       1       2       3       1  traces/dave.trace
     3 (2,1,3)     167      71      67     167      71      67  traces/trans.trace
     3 (2,2,3)     201      37      29     201      37      29  traces/trans.trace
     3 (2,4,3)     212      26      10     212      26      10  traces/trans.trace
     3 (5,1,5)     231       7       0     231       7       0  traces/trans.trace
     6 (5,1,5)  265189   21775   21743  265189   21775   21743  traces/long.trace
    24


  Part B: Testing transpose function
  Running ./test-trans -M 32 -N 32
  Running ./test-trans -M 64 -N 64
  Running ./test-trans -M 61 -N 67

  Cache Lab summary:
                        Points   Max pts      Misses
  Csim correctness          24.0        27
  Trans perf 32x32           8.0         8         289
  Trans perf 64x64           8.0         8        1221
  Trans perf 61x67          10.0        10        1991
          Total points    50.0        53
  ```
  <http://moss.cs.iit.edu/cs351/mp5.html>
  ```
  ./mdriver -V
  Using default tracefiles in ./traces/
  Measuring performance with the interval timer.

  Testing mm malloc
  Reading tracefile: amptjp-bal.rep
  Checking mm_malloc for correctness, efficiency, and performance.
  Reading tracefile: cccp-bal.rep
  Checking mm_malloc for correctness, efficiency, and performance.
  Reading tracefile: cp-decl-bal.rep
  Checking mm_malloc for correctness, efficiency, and performance.
  Reading tracefile: expr-bal.rep
  Checking mm_malloc for correctness, efficiency, and performance.
  Reading tracefile: coalescing-bal.rep
  Checking mm_malloc for correctness, efficiency, and performance.
  Reading tracefile: random-bal.rep
  Checking mm_malloc for correctness, efficiency, and performance.
  Reading tracefile: random2-bal.rep
  Checking mm_malloc for correctness, efficiency, and performance.
  Reading tracefile: binary-bal.rep
  Checking mm_malloc for correctness, efficiency, and performance.
  Reading tracefile: binary2-bal.rep
  Checking mm_malloc for correctness, efficiency, and performance.
  Reading tracefile: realloc-bal.rep
  Checking mm_malloc for correctness, efficiency, and performance.
  Reading tracefile: realloc2-bal.rep
  Checking mm_malloc for correctness, efficiency, and performance.

  Results for mm malloc:
  trace  valid  util     ops      secs  Kops
  0       yes   88%    5694  0.000262 21749
  1       yes   90%    5848  0.000235 24928
  2       yes   94%    6648  0.000528 12598
  3       yes   96%    5380  0.000386 13952
  4       yes   99%   14400  0.000484 29777
  5       yes   88%    4800  0.000360 13352
  6       yes   83%    4800  0.000427 11246
  7       yes   90%   12000  0.000295 40678
  8       yes   69%   24000  0.000653 36742
  9       yes   59%   14401  0.000412 34971
  10       yes   52%   14401  0.000311 46246
  Total          83%  112372  0.004351 25827

  Perf index = 50 (util) + 40 (thru) = 90/100
  ```
  
