To include seed gain information in Run-2 EGM TnP Tree

# Reference

* https://github.com/cms-egamma/EgammaAnalysis-TnPTreeProducer/tree/RunIIfinal

# Recipe

* initial setup for the first time

```bash
lxplus

cd /afs/cern.ch/user/k/kplee/work/private/Detector/EGMTnPTree

# -- change to CentOS7
# -- https://cms-sw.github.io/singularity.html
cmssw-el7

export SCRAM_ARCH=slc7_amd64_gcc700
cmsrel CMSSW_10_6_13
cd CMSSW_10_6_13/src
cmsenv
git clone -b RunIIfinal git@github.com:cms-egamma/EgammaAnalysis-TnPTreeProducer.git EgammaAnalysis/TnPTreeProducer
scram b -j8

cd EgammaAnalysis/TnPTreeProducer
git remote add mycode git@github.com:KyeongPil-Lee/EgammaAnalysis-TnPTreeProducer.git
git checkout -b "RunII_seedGain"

```

* after the initial setup

```bash
lxplus

cmssw-el7

cd /afs/cern.ch/user/k/kplee/work/private/Detector/EGMTnPTree/CMSSW_10_6_13/src/EgammaAnalysis/TnPTreeProducer
cmsenv
```

## Run TnP code locally

```bash
cd python
cmsRun TnPTreeProducer_cfg.py doTrigger=True era=UL2018 maxEvents=10000 >&TnPTreeProducer_cfg.log& \
tail -f TnPTreeProducer_cfg.log

```



## Submit CRAB jobs

```bash
cd crab
source /cvmfs/cms.cern.ch/common/crab-setup.sh
python tnpCrabSubmit_T2B.py # -- turn off doL1matching: it doesn't work if doL1matching=True...

```



## Check CRAB status

```bash
cd /afs/cern.ch/user/k/kplee/work/private/Detector/EGMTnPTree/CMSSW_10_6_13/src/EgammaAnalysis/TnPTreeProducer/crab/crab_2024-09-19

voms-proxy-init --voms cms
#source /cvmfs/cms.cern.ch/common/crab-setup.sh

cmssw-el7

cmsenv

# crab status crab_UL2018_DY_LO # -- done except for 2 jobs
# crab status crab_UL2018_DY_NLO # -- completed (23 Sep. 2024)
# crab status crab_UL2018_Run2018A  # -- completed (23 Sep. 2024)
# crab status crab_UL2018_Run2018B # -- completed (25 Sep. 2024)
# crab status crab_UL2018_Run2018C # -- completed (23 Sep. 2024)
crab status crab_UL2018_Run2018D # -- 63% processed (25 Sep. 2024)

# crab resubmit crab_UL2018_DY_LO
# crab resubmit crab_UL2018_DY_NLO
# crab resubmit crab_UL2018_Run2018A
# crab resubmit crab_UL2018_Run2018B
# crab resubmit crab_UL2018_Run2018C
crab resubmit crab_UL2018_Run2018D
```

