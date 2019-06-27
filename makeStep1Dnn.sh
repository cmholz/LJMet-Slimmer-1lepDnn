#!/bin/bash

hostname
date

infilename=${1}
outfilename=${2}
inputDir=${3}
outputDir=${4}
idlist=${5}
scratch=${PWD}

source /cvmfs/cms.cern.ch/cmsset_default.sh
scramv1 project CMSSW CMSSW_10_2_10
cd CMSSW_10_2_10

echo "unpacking tar"
tar -xf ${scratch}/slimmerdnn.tar
rm ${scratch}/slimmerdnn.tar

cd src/LJMet-Slimmer-1lepDnn/

echo "cmsenv"
eval `scramv1 runtime -sh`

echo "setting macroDir to PWD"
macroDir=${PWD}
export PATH=$PATH:$macroDir

XRDpath=root://cmseos.fnal.gov/$inputDir

echo "Running step1 over list: ${idlist}"
for iFile in $idlist; do
    inFile=${iFile}
    if [[ iFile == ext* ]] ;
    then
	inFile=${iFile:4}
    elif [[ iFile == [ABCDEF]* ]] ;
    then
	inFile=${iFile:1}
    fi

    echo "creating ${outfilename}_${iFile}.root"
    root -l -b -q runStep1Dnn.C\(\"$macroDir\",\"$XRDpath/${infilename}_${inFile}.root\",\"${outfilename}_${iFile}.root\",\"${outputDir}\"\)
done

rm *.json

echo "ROOT Files:"
ls -l *.root

# copy output to eos

echo "xrdcp output for condor"
for FILE in *.root
do
  echo "xrdcp -f ${FILE} root://cmseos.fnal.gov/${outputDir}/${FILE}"
  xrdcp -f ${FILE} root://cmseos.fnal.gov/${outputDir}/${FILE} 2>&1
  XRDEXIT=$?
  if [[ $XRDEXIT -ne 0 ]]; then
    rm *.root
    echo "exit code $XRDEXIT, failure in xrdcp"
    exit $XRDEXIT
  fi
  rm ${FILE}
done

echo "done"