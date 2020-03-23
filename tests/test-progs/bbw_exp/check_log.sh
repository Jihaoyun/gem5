#!/bin/bash

home_dir=`echo $HOME`

dir="${home_dir}/Work/gem5/gem5"

current_dir=`pwd`

stat_file="${current_dir}/gem5_stat.txt"
debug_file="${current_dir}/gem5_debug.txt"
script_file="lapo/reg_fault_injector_o3_monitor.py"

cmd="${dir}/build/ARM/gem5.opt --debug-file=${debug_file} --debug-start=0 --debug-end=0 --debug-flags=DataCommMonitor,Registers,O3Registers --stats-file ${stat_file} ${dir}/configs/${script_file} -m 40000000000 -b"
src_file="${current_dir}/bbw_test_mod.assemble"

result_file=$1
time_out_thresh=30000000000

ref_chk_file="${current_dir}/goldenCheckData.dat"
ref_sim_file="${current_dir}/goldenSimData.dat"

#----------------------------
# Run the gem5 command
#----------------------------
if [ -e $result_file ]; then
    rm $result_file
fi

for ((i=1; i <= $2; i++))
do
    debug_file="${current_dir}/gem5_debug_${i}.txt"
    lasti=$(($i-1))
    debug_start="${lasti}00000000"
    debug_end="${i}00000000"
    cmd="${dir}/build/ARM/gem5.opt --debug-file=${debug_file} --debug-start=${debug_start} --debug-end=${debug_end} --debug-flags=DataCommMonitor,Registers,O3Registers --stats-file ${stat_file} ${dir}/configs/${script_file} -m 40000000000 -b"
    if [ $i -eq 1 ]; then
        $cmd $src_file >> $result_file
        gem5_result=$?
    else
        $cmd $src_file
    fi
done

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
diff_value=`diff $ref_chk_file ${current_dir}/checkData.dat | wc -l`

if [ $diff_value -eq 0 ]; then
    # Check simulation validity
    diff_value=`diff $ref_sim_file ${current_dir}/simData.dat | wc -l`

    if [ $diff_value -ne 0 ]; then
        # Check if the latency is too long
        grep_value=`grep "@ tick" < $result_file | grep -e "[0-9]*" -o`

        if [ $grep_value -ge $time_out_thresh ]; then
            echo "< UNDETECTED ATTACK > Attack causes program unable to generate result in limited time!" 
            sed '1i < UNDETECTED ATTACK > Attack causes program unable to generate result in limited time!\n' -i $result_file
            exit 3  # Time out error with no result
        else
            echo "< UNDETECTED ATTACK > Attack produces incorrect sim result without being detected by checksum!"
            sed '1i < UNDETECTED ATTACK > Attack produces incorrect sim result without being detected by checksum!\n' -i $result_file
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
    echo "< DETECTED ATTACK > Attack causes checksum error!"
    sed '1i < DETECTED ATTACK > Attack causes checksum error!\n' -i $result_file
    exit 4  # Detected dataflow error
fi
