#!/bin/bash

home_dir=`echo $HOME`
dir="${home_dir}/Work/gem5/gem5"

stat_file="${dir}/tests/test-progs/multiplication/gem5_stat.txt"
debug_file="${dir}/tests/test-progs/multiplication/gem5_debug.txt"
script_file="lapo/reg_fault_injector_o3_monitor.py"

cmd="${dir}/build/ARM/gem5.opt --debug-file=${debug_file} --debug-flags=DataCommMonitor --stats-file ${stat_file} ${dir}/configs/${script_file} -m 500000000 -b"
src_file="${dir}/tests/test-progs/multiplication/fixpoint_matrix_mul_mod.assemble"

result_file=$1
time_out_thresh=240000000
ref_product="43 33 79 61"

#----------------------------
# Run the gem5 command
#----------------------------
if [ -e $result_file ]; then
    rm $result_file
fi
$cmd $src_file >> $result_file

gem5_result=$?

#----------------------------
# Attack causes blue screen
#----------------------------
if [ $gem5_result -ne 0 ]; then
    echo "< DETECTED ATTACK > Blue screen error has been found!"
    sed '1i < DETECTED ATTACK > Blue screen error has been found!\n' -i $result_file
    exit 1  # Blue screen error 
fi

#------------------------------
# Attack causes dataflow error
#------------------------------
grep_value=`grep "\[ERROR\]" < $result_file | wc -l`

if [ $grep_value -eq 0 ]; then
    # Check product validity
    grep_value=`grep "The calculated checksum matrix product" -A 3 $result_file | grep "[0-9]*" -o`
    grep_value=`echo $grep_value`

    if [ "$grep_value" != "$ref_product" ]; then
        # Check if the latency is too long
        grep_value=`grep "@ tick" < $result_file | grep -e "[0-9]*" -o`

        if [ $grep_value -ge $time_out_thresh ]; then
            echo "< UNDETECTED ATTACK > Attack causes program unable to generate result in limited time!" 
            sed '1i < UNDETECTED ATTACK > Attack causes program unable to generate result in limited time!\n' -i $result_file
            exit 3  # Time out error with no result
        else
            echo "< UNDETECTED ATTACK > Attack produces incorrect product without being detected by checksum (not be reported by software as well)!"
            sed '1i < UNDETECTED ATTACK > Attack produces incorrect product without being detected by checksum (not be reported by software as well)!\n' -i $result_file
            exit 2  # Undetected dataflow error
        fi
    else
        # Check if the latency is too long
        grep_value=`grep "@ tick" < $result_file | grep -e "[0-9]*" -o`

        if [ $grep_value -ge $time_out_thresh ]; then
            echo "< UNDETECTED ATTACK > Attack causes program takes too long to generate the correct result!" 
            sed '1i < UNDETECTED ATTACK > Attack causes program takes too long to generate the correct result!\n' -i $result_file
            exit 3  # Time out error with correct result
        else
            echo "< SUCCEED > Attack has no effect on the program functionality"
            sed '1i < SUCCEED > Attack has no effect on the program functionality\n' -i $result_file
            exit 0  # Program finished gracefully
        fi
    fi
else
    # Check if error being detected
    grep_value=`grep "\[ERROR\] Checksum" < $result_file | wc -l`

    if [ $grep_value -eq 0 ]; then
        echo "< UNDETECTED ATTACK > Attack produces incorrect product without being detected by checksum!"
        sed '1i < UNDETECTED ATTACK > Attack produces incorrect product without being detected by checksum!\n' -i $result_file
        exit 4  # Undetected dataflow error
    else
        echo "< DETECTED ATTACK > Attack causes checksum error!"
        sed '1i < DETECTED ATTACK > Attack causes checksum error!\n' -i $result_file
        exit 5  # Detected dataflow error
    fi
fi
