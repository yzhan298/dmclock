#!/bin/bash
config_dir="configs"
output_file="data.log"
repeat=6


# create two simulators in two directories 
mkdir build_h
cd build_h
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Release ../../.
make dmclock-sims
cd ..

mkdir build_q
cd build_q
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_SIMPLE_PQ=On ../../.
make dmclock-sims
cd ..

# run simulators 
echo '' > $output_file
for config in "$config_dir"/*
do
  i=0
  while [ $i -lt $repeat ]
  do  
    i=$(( $i + 1 ))
    
    # clear cache first
    sync
    sudo sh -c 'echo 1 >/proc/sys/vm/drop_caches'
    sudo sh -c 'echo 2 >/proc/sys/vm/drop_caches'
    sudo sh -c 'echo 3 >/proc/sys/vm/drop_caches'

    # run with heap
    msg="file_name:heap:$config"
    echo $msg >> $output_file
    echo "running $msg ..."
    ./build_h/sim/dmc_sim -c $config | awk '(/average/)' >> $output_file


    # clear cache again
    sync
    sudo sh -c 'echo 1 >/proc/sys/vm/drop_caches'
    sudo sh -c 'echo 2 >/proc/sys/vm/drop_caches'
    sudo sh -c 'echo 3 >/proc/sys/vm/drop_caches'
    
    # run with simple pq
    msg="file_name:pq:$config"
    echo $msg >> $output_file
    echo "running $msg ..."
    ./build_q/sim/dmc_sim -c $config | awk '(/average/)' >> $output_file    
  
  done # end repeat
done # end config

