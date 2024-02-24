#!/bin/bash
make all

template="H"

for flag in e i v c l n h s o
do
    echo " "
    test_number=0
    echo "Flag -$flag:"
    for test_file in "./tests_grep/test_text_0.txt" "./tests_grep/test_text_1.txt" "./tests_grep/test_text_2.txt" "./tests_grep/test_text_3.txt" "./tests_grep/test_text_4.txt" "./tests_grep/test_text_5.txt" "./tests_grep/test_text_6.txt" "./tests_grep/test_text_7.txt ./tests_grep/test_text_ptrn.txt"
    do
        echo "Flag -$flag, test $test_number:"
        # valgrind --leak-check=full --track-origins=yes --log-file="1.txt" ./s21_grep -$flag $template $test_file > 1.txt
        leaks -atExit -- ./s21_grep -$flag $template $test_file > 1.txt
        tail -n4 ./1.txt
        rm 1.txt
        test_number=$(( $test_number + 1 ))
    done
done
echo " "
