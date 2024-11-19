gcc master.c -o master
gcc slave.c -o slave
./master 3 my_shared_mem
