#!/bin/bash

funciton() {
    echo "testing"
}

while true; do
    read -r var
    case "$var" in
        test)
            funciton
            ;;
    esac
done
