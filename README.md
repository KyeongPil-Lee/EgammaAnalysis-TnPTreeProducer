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
crab status crab_UL2018_Run2018D # -- 96.9% processed (26 Sep. 2024)

# crab resubmit crab_UL2018_DY_LO
# crab resubmit crab_UL2018_DY_NLO
# crab resubmit crab_UL2018_Run2018A
# crab resubmit crab_UL2018_Run2018B
# crab resubmit crab_UL2018_Run2018C
crab resubmit crab_UL2018_Run2018D
```



## TnP tree for RECO

### Updates

* `python/egmGoodParticlesDef_cff.py`

```python
# -- process.eleVarHelper
ebRecHits        = cms.InputTag("reducedEcalRecHitsEB","","RECO"),
eeRecHits        = cms.InputTag("reducedEcalRecHitsEE","","RECO"),
```

* `plugin/SCVariableHelper.h`, `python/egmTreesContent_cff.py`
  * To have `sc_seedGain` information
* `python/TnPTreeProducer_cfg.py`
  * To avoid a strange error which happens only for MC AOD:
    add `inputCommands = cms.untracked.vstring("keep *", "drop recoTrackExtrasedmAssociation_muonReducedTrackExtras__RECO")` in PoolSource
* `crab/tnpCrabSubmit_T2B.py`
  * Switch to AOD samples
  * Update the option (produce RECO trees only)

### Local test

```bash
cd python
cmsRun TnPTreeProducer_cfg.py isAOD=True doRECO=True doEleID=False doTrigger=False era=UL2018 maxEvents=10000 >&TnPTreeProducer_cfg.log& \
tail -f TnPTreeProducer_cfg.log

# -- test on MC
cmsRun TnPTreeProducer_cfg.py isMC=True isAOD=True doRECO=True doEleID=False doTrigger=False era=UL2018 maxEvents=1000 >&TnPTreeProducer_cfg.log& \
tail -f TnPTreeProducer_cfg.log
```

### CRAB submit

```
cd crab
python tnpCrabSubmit_T2B.py
```

### CRAB status

```bash
cd /afs/cern.ch/work/k/kplee/private/Detector/EGMTnPTree/CMSSW_10_6_13/src/EgammaAnalysis/TnPTreeProducer/crab/crab_2024-10-08

cmssw-el7

cmsenv
voms-proxy-init --voms cms

crab status crab_UL2018_Run2018A # -- 99.9% done (3PM, 10 Oct.)
# crab status crab_UL2018_Run2018B # -- done (4PM, 9 Oct.)
# crab status crab_UL2018_Run2018C # -- done (4PM, 9 Oct.)
crab status crab_UL2018_Run2018D

crab resubmit crab_UL2018_Run2018A
# crab resubmit crab_UL2018_Run2018B
# crab resubmit crab_UL2018_Run2018C
crab resubmit crab_UL2018_Run2018D

cd ../crab_2024-10-08-v2

crab status crab_UL2018_DY_LO # -- done (5PM, 10 Oct.)
# crab status crab_UL2018_DY_NLO # -- done (4PM, 9 Oct.)

crab resubmit crab_UL2018_DY_LO
# crab resubmit crab_UL2018_DY_NLO
```

