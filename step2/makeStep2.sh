#!/bin/bash

hostname

infilename=${1}
outfilename=${2}
inputDir=${3}
outputDir=${4}

scratch=${PWD}
macroDir=${PWD}
source /cvmfs/cms.cern.ch/cmsset_default.sh
scramv1 project CMSSW CMSSW_10_2_16_UL
cd CMSSW_10_2_16_UL
eval `scramv1 runtime -sh`
cd -

runShifts=true
if [[ $outputDir == *"training"* ]]; then
  echo "Not running shifts"
  runShifts=false
fi

export PATH=$PATH:$macroDir

XRDpath=root://cmseos.fnal.gov/$inputDir
XRDoutPath=root://cmseos.fnal.gov/$outputDir
root -l -b -q makeStep2.C\(\"$macroDir\",\"$XRDpath/$infilename\",\"$outfilename\",$runShifts\)

echo "ROOT Files:"
ls -l *.root
#rm puppiCorr.root

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
