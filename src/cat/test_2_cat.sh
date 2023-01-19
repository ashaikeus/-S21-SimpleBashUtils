#!/bin/bash
make all

for flag in "-b" "-e" "-n" "-s" "-t" "-E" "-T"
do
    echo " "
    test_number=1
    for test_file in "tests_grep/test_text_0.txt" "./tests_grep/test_text_1.txt" "./tests_grep/test_text_2.txt" "./tests_grep/test_text_3.txt" "./tests_grep/test_text_4.txt" "./tests_grep/test_text_5.txt" "./tests_grep/test_text_6.txt" "./tests_grep/test_text_7.txt" "./tests_grep/test_text_ptrn.txt" "tests_cat/test_text_1.txt" "tests_cat/test_text_2.txt" "./tests_cat/test_text_3.txt" "./tests_cat/test_text_4.txt" "./tests_cat/test_text_5.1.txt ./tests_cat/test_text_5.2.txt"
    do
        echo "Flag $flag, test $test_number:"
        valgrind --log-file="1.txt" ./s21_cat $flag $test_file > 1.txt
        tail -n4 ./1.txt
        echo " "
        rm 1.txt
        test_number=$(( $test_number + 1 ))
    done
done
echo " "