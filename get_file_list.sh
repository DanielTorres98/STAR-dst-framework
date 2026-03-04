#!/bin/bash

get_file_list.pl -distinct -keys path,filename \
    -cond 'trgsetupname=production_pAu200_2015,filetype=daq_reco_MuDst,library=SL16d,filename~st_physics,production=P16id,storage!=hpss,runnumber>=16128030,runnumber<=16159024' \
    -limit 5 \
    -delim '/' > list/file_list.list

# Check if list/ZrZr.list is not empty
if [ -s list/file_list.list ]; then
    echo "list/file_list.list generated and is not empty."
else
    echo "list/file_list.list is empty. No files where found"
fi

# catalog:star.bnl.gov?trgsetupname=production_pAu200_2015,filetype=daq_reco_MuDst,filename~st_physics" nFiles="50"

# Generate runnumber list
get_file_list.pl -distinct -keys runnumber \
    -cond 'trgsetupname=production_pAu200_2015,filetype=daq_reco_MuDst,library=SL16d,filename~st_physics,production=P16id,storage!=hpss' \
    -limit 0 | uniq | sort > list/runnumber_list.list

# Check if runnumber_list.list is not empty
if [ -s list/runnumber_list.list ]; then
    echo "runnumber_list.list generated and is not empty."
else
    echo "runnumber_list.list is empty."
fi

echo "Appending root://xrdstar.rcf.bnl.gov:1095// to file paths."

sed -i -e 's/^/root:\/\/xrdstar.rcf.bnl.gov\:1095\//' list/file_list.list

echo "Done."