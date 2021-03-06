#!/usr/bin/env bash

g++ -E -CC -std=c++11 -I../include ../include/fplus/fplus.hpp > temp_preprocessor_output.txt
echo "// THIS FILE WAS GENERATED AUTOMATICALLY. DO NOT EDIT." > ../include/fplus/fwd_instances.autogenerated_defines
cat temp_preprocessor_output.txt | grep -B 1 -e "// fwd bind count" | perl -pe "s/^...//g" | sed ':a;N;$!ba;s/\n/ /g' | perl -pe "s/ -- /\n/g" | perl -pe "s/API search type: ([^ ]*).*fwd bind count: ([0-9])/fplus_fwd_define_fn_\2(\1)/g" >> ../include/fplus/fwd_instances.autogenerated_defines
rm -f temp_preprocessor_output.txt