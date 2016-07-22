# Note you need gnuplot 4.4 for the pdfcairo terminal.
clear
reset

set terminal pdfcairo font "Gill Sans,5" linewidth 1 rounded fontscale .8 noenhanced
set output "benchmark.pdf"

# starts multiplot
set multiplot layout 2,1

# Line style for axes
set style line 80 lt rgb "#808080"

# Line style for grid
set style line 81 lt 0  # dashed
set style line 81 lt rgb "#808080"  # grey

set grid back linestyle 81
set border 3 back linestyle 80 

#set xtics rotate out
set style data histogram
set style histogram clustered

set style fill solid border
set xlabel 'Heap vs SimplePQ Timing'   
set ylabel 'Time (nanosec)'        
set key top right

# plot 1
set title 'Request Addition Time'       
plot for [COL=2:3] 'data.log.dat' using COL:xticlabels(1) title columnheader

# plot 2
set title 'Request Completion Time'       
plot for [COL=4:5] 'data.log.dat' using COL:xticlabels(1) title columnheader
