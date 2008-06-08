#!/bin/bash -x
#

locales="ar bg cs de es fa fi fr he hu it ja nb nl pl pt ru sq sv tr zh_cn zh_tw"
poDir="po"
outDir="ts"

for ts in $locales
do
    input=$poDir/$ts/
    locale="`echo $ts|tr _ -`";
    mkdir -p $outDir/$locale/
    po2ts -i $poDir/$ts/vidalia.po -o $outDir/$locale/vidalia_$locale.ts 
done
