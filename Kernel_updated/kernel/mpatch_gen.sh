#!/bin/bash
rm -rf mpatch_Kconfig
rm -rf ./include/mstar
../mstar/mpatch/mpatch.bin mpatch.config mpatch_macro.h mpatch_Kconfig ../mstar/mpatch/doc/
if [ "$?" == "1" ]; then
    exit 1
fi
mkdir ./include/mstar
cp -af mpatch_macro.h ./include/mstar
rm -rf mpatch_macro.h
cd ..
exit $mpatch_return_value



