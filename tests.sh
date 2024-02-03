#!/bin/bash

success() {
    echo " " "\e[32m$1\e[0m"
}

error() {
    echo " " "\e[31m$1\e[0m"
    echo "  "$2
}

echo "Test: zero exit code"
for f in $(find tests/expect/success -maxdepth 1 -name "*.nva" -type f | sort); do
    out=$(./novalang $f 2>&1 1>/dev/null)

    if [ $? -eq 0 ]; then
        success $f
    else
        error $f $out
    fi
done

echo

echo "Test: zero exit code + correct output"
for f in $(find tests/expect/success/on-output -maxdepth 1 -name "*.nva" -type f | sort); do
    out=$(./novalang $f | diff ${f%.nva}.output -)

    if [ $? -eq 0 ]; then
        success $f
    else
        error $f $out
    fi
done

echo

echo "Test: zero exit code + correct output for input"
for f in $(find tests/expect/success/on-output/with-input -maxdepth 1 -name "*.nva" -type f | sort); do
    out=$(./novalang $f <${f%.nva}.input | diff ${f%.nva}.output -)

    if [ $? -eq 0 ]; then
        success $f
    else
        error $f $out
    fi
done

echo

echo "Test: non-zero exit code"
for f in $(find tests/expect/error -maxdepth 1 -name "*.nva" -type f | sort); do
    out=$(./novalang $f 2>&1 1>/dev/null)

    if [ $? -ne 0 ]; then
        success $f
    else
        error $f
    fi
done

echo

echo "Test: non-zero exit code + correct output"
for f in $(find tests/expect/error/on-output -maxdepth 1 -name "*.nva" -type f | sort); do
    out=$(./novalang $f 2>&1 | diff ${f%.nva}.output -)

    if [ $? -ne 0 ]; then
        success $f
    else
        error $f $out
    fi
done
