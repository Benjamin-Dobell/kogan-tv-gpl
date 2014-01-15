#!/bin/bash

find ./ | tr '[A-Z]' '[a-z]' | sort | uniq -c | grep -v " 1 "
