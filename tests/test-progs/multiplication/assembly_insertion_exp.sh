#!/bin/bash
home_dir=`echo $HOME`
dir="${home_dir}/Work/gem5/gem5"
src_file="${dir}/tests/test-progs/multiplication/fixpoint_matrix_mul_mod.assemble"

log_file="gem5_coverage_log.txt"
stat_file="gem5_stat.txt"
debug_file="gem5_debug.txt"
target_file="${dir}/tests/test-progs/multiplication/fixpoint_matrix_mul.s"
insert_script="${dir}/tests/test-progs/multiplication/insert_branch.pl"
check_script="${dir}/tests/test-progs/multiplication/check_log.sh"

result_dir="${dir}/tests/test-progs/multiplication/Exp_result"
result_file="gem5_coverage_test.txt"
result_clean_cmd="rm ${dir}/tests/test-progs/multiplication/Exp_result/*/*/*"
dir_clean_cmd="rmdir ${dir}/tests/test-progs/multiplication/Exp_result/*/*"

num_exp=10

make build

test -d ${result_dir}
flag=`echo $?`
if [ $flag -eq 1 ]; then
    mkdir ${result_dir}
fi

test -d ${result_dir}/No_error
flag=`echo $?`
if [ $flag -eq 1 ]; then
    mkdir ${result_dir}/No_error
fi

test -d ${result_dir}/Detected_blue_screen_error
flag=`echo $?`
if [ $flag -eq 1 ]; then
    mkdir ${result_dir}/Detected_blue_screen_error
fi

test -d ${result_dir}/Undetected_product_error
flag=`echo $?`
if [ $flag -eq 1 ]; then
    mkdir ${result_dir}/Undetected_product_error
fi

test -d ${result_dir}/Undetected_timeout_error
flag=`echo $?`
if [ $flag -eq 1 ]; then
    mkdir ${result_dir}/Undetected_timeout_error
fi

test -d ${result_dir}/Detected_checksum_error
flag=`echo $?`
if [ $flag -eq 1 ]; then
    mkdir ${result_dir}/Detected_checksum_error
fi

#---------------------
# Coverage Experiment
#---------------------
echo ""
echo "-------------------------------------------------"
echo "- Clean experiment dir and start new experiment  "
echo "-------------------------------------------------"
echo ""
echo $result_clean_cmd
echo $dir_clean_cmd
${result_clean_cmd} && ${dir_clean_cmd}

for exp_id in `seq 1 $num_exp`; do

    exp_res_file="exp_${exp_id}.txt"

    #-------------
    # Branch args
    #-------------
    branch_num=1
    branch_prob=0.0$(($RANDOM % 10 + 1))
    branch_int=1
    
    branch_args="-b -bn $branch_num -bp $branch_prob -bi $branch_int"
    
    #------------
    # NOPs args
    #------------
    nop_num=1
    nop_prob=0.0$(($RANDOM % 10 + 1))
    nop_cycle=5
    
    nop_args="-nop -nopn $nop_num -nopp $nop_prob -nopc $nop_cycle"
    
    #------------------------
    # Run the insert script
    #------------------------
    #args="${branch_args} ${nop_args}"
    args="${branch_args}"
    cmd="${insert_script} ${args} ${target_file}"
    echo ""
    echo "-------------------------------------------------"
    echo "- Run the insert script to modify assembly code  "
    echo "-------------------------------------------------"
    $cmd && make all
    
    echo "---------------------------------------------" >  $log_file
    echo "  Branch args: ${branch_args}"                 >> $log_file
    #echo "  NOPS args  : ${nop_args}"                    >> $log_file
    echo "---------------------------------------------" >> $log_file
    echo ""                                              >> $log_file
    
    #-----------------------
    # Run the check script
    #-----------------------
    echo ""
    echo "-----------------------------------------------"
    echo "- Run the check script and collect the result  "
    echo "-----------------------------------------------"
    echo ""
    ${check_script} ${result_file}
    result=`echo $?`
    echo ""
#    echo $result

    #---------------------
    # Collect the result
    #---------------------
    cat $result_file >> $log_file
    cp ${log_file} ${exp_res_file}
    
    if [ $result -eq 0 ]; then
        mkdir ${result_dir}/No_error/$exp_id
        mv ${exp_res_file} ${result_dir}/No_error/$exp_id
        mv ${stat_file} ${result_dir}/No_error/$exp_id
        mv ${debug_file} ${result_dir}/No_error/$exp_id
        cp ${src_file/.assemble/.s} ${result_dir}/No_error/$exp_id/${exp_res_file/.txt/.s}

    elif [ $result -eq 1 ]; then
        mkdir ${result_dir}/Detected_blue_screen_error/$exp_id
        mv ${exp_res_file} ${result_dir}/Detected_blue_screen_error/$exp_id
        mv ${stat_file} ${result_dir}/Detected_blue_screen_error/$exp_id
        mv ${debug_file} ${result_dir}/Detected_blue_screen_error/$exp_id
        cp ${src_file/.assemble/.s} ${result_dir}/Detected_blue_screen_error/$exp_id/${exp_res_file/.txt/.s}

    elif [ $result -eq 2 ] || [ $result -eq 4 ]; then
        mkdir ${result_dir}/Undetected_product_error/$exp_id
        mv ${exp_res_file} ${result_dir}/Undetected_product_error/$exp_id
        mv ${stat_file} ${result_dir}/Undetected_product_error/$exp_id
        mv ${debug_file} ${result_dir}/Undetected_product_error/$exp_id
        cp ${src_file/.assemble/.s} ${result_dir}/Undetected_product_error/$exp_id/${exp_res_file/.txt/.s}

    elif [ $result -eq 3 ]; then
        mkdir ${result_dir}/Undetected_timeout_error/$exp_id
        mv ${exp_res_file} ${result_dir}/Undetected_timeout_error/$exp_id
        mv ${stat_file} ${result_dir}/Undetected_timeout_error/$exp_id
        mv ${debug_file} ${result_dir}/Undetected_timeout_error/$exp_id
        cp ${src_file/.assemble/.s} ${result_dir}/Undetected_timeout_error/$exp_id/${exp_res_file/.txt/.s}

    elif [ $result -eq 5 ]; then
        mkdir ${result_dir}/Detected_checksum_error/$exp_id
        mv ${exp_res_file} ${result_dir}/Detected_checksum_error/$exp_id
        mv ${stat_file} ${result_dir}/Detected_checksum_error/$exp_id
        mv ${debug_file} ${result_dir}/Detected_checksum_error/$exp_id
        cp ${src_file/.assemble/.s} ${result_dir}/Detected_checksum_error/$exp_id/${exp_res_file/.txt/.s}
    fi

done