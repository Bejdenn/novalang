#!/bin/bash

echo "Tests that expect to have a successful exit code"
for f in $(find ./tests/expect/success -name "*.nva" -type f | sort); do
    out=$(cat $f | ./novalang 2>&1 1>/dev/null)

    if [ $? -eq 0 ]; then
        echo "\e[32m$f\e[0m"
    else
        echo "\e[31m$f\e[0m" " " $out
    fi
done

echo

echo "Tests that expect to have a non-zero exit code"
for f in $(find ./tests/expect/error -name "*.nva" -type f | sort); do
    out=$(cat $f | ./novalang 2>&1 1>/dev/null)

    if [ $? -ne 0 ]; then
        echo "\e[32m$f\e[0m" "\t" $out
    else
        echo "\e[31m$f\e[0m"
    fi
done
