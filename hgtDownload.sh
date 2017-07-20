#!/bin/bash

file=${*:1}
lat=${file:1:2}
lon=${file:4:3}
catalog='heightmaps/'

mkdir -p $catalog

if [ ${lon:0:2} = '00' ]
then
    lon=${lon:2}
elif [ ${lon:0:1} = '0' ]
then
    lon=${lon:1}
fi

if [ ${lat:0:1} = '0' ]
then
    lat=${lat:1}
fi

for((i=0; i<$2; i++))
do
    curlat=$(($lat-$i))
    if [ ${#curlat} = 1 ]
    then
        curlat='0'$curlat
    fi

    for((j=0; j<$2; j++))
    do
        curlon=$(($lon+$j))
        if [ ${#curlon} = 1 ]
        then
            curlon='00'$curlon
        elif [ ${#curlon} = 2 ]
        then
            curlon='0'$curlon
        fi
        filename=${file:0:1}''$curlat''${file:3:1}''$curlon'.hgt.zip'
        filepath=$catalog$filename
        (wget -nv https://dds.cr.usgs.gov/srtm/version2_1/SRTM3/Eurasia/''$filename -O $filepath;\
        unzip -u $filepath -d $catalog;\
        rm -f $filepath) &
    done
done

wait
