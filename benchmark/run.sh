#!/bin/bash

echo "generating file data.log"
sh data_gen.sh

echo "converting data.log to data.log.dat"
python data_parser.py

echo "now generating bar-chart"
gnuplot plot_gen.gnuplot

echo "done! check benchmark.pdf"

