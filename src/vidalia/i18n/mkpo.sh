#!/bin/bash -x
#

locales="ar bg cs de es fa fi fr he hu it ja nb nl pl pt ru sq sv tr zh-cn zh-tw"
outDir="po"

for ts in $locales
do
    pootleDir="`echo $ts|tr - _`";
    mkdir -p $outDir/$pootleDir/
    ts2po -i vidalia_$ts.ts -o $outDir/$pootleDir/vidalia.po
    #po2ts -i vidalia_fi.po -o vidalia_fi.ts
done
