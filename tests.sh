#!/bin/bash

__echo_color() {
    printf "\033[1;%s%s\033[0m\n" "$1" "$2"
}

__echo_green() {
    __echo_color "32m" "$1"
}

__echo_red() {
    __echo_color "31m" "$1"
}

__print_test_success() {
    __echo_green "✔ $1"
}

__print_test_error() {
    __echo_red "✘ $1."
}

echo "Test: zero exit code"
for f in $(find tests/expect/success -not -path "tests/expect/success/on-output/*" -name "*.nva" -type f | sort); do
    if ./novalang "$f" 1>/dev/null 2>&1; then
        __print_test_success "$f"
    else
        __print_test_error "$f"
    fi
done

echo

echo "Test: zero exit code + correct output"
for f in $(find tests/expect/success/on-output -not -path "tests/expect/success/on-output/with-input/*" -name "*.nva" -type f | sort); do
    if ./novalang "$f" | diff "${f%.nva}".output -; then
        __print_test_success "$f"
    else
        __print_test_error "$f"
    fi
done

echo

echo "Test: zero exit code + correct output for input"
for f in $(find tests/expect/success/on-output/with-input -name "*.nva" -type f | sort); do
    if ./novalang "$f" <"${f%.nva}".input | diff "${f%.nva}".output -; then
        __print_test_success "$f"
    else
        __print_test_error "$f"
    fi
done

echo

echo "Test: non-zero exit code"
for f in $(find tests/expect/error -not -path "tests/expect/error/on-output/*" -name "*.nva" -type f | sort); do
    if ./novalang "$f" 1>/dev/null; then
        __print_test_success "$f"
    else
        __print_test_error "$f"
    fi
done

echo

echo "Test: non-zero exit code + correct output"
for f in $(find tests/expect/error/on-output -name "*.nva" -type f | sort); do
    if ./novalang "$f" 2>&1 | diff "${f%.nva}".output -; then
        __print_test_success "$f"
    else
        __print_test_error "$f"
    fi
done
