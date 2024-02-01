#!/bin/bash

for f in $(find ./tests/ -name "*.nl" -type f | sort); do
    out=$(cat $f | ./novalang 2>&1 1>/dev/null)

    if [ $? -eq 0 ]; then
        echo "\e[32m$f\e[0m"
    else
        echo "\e[31m$f\e[0m" " " $out
    fi
done
