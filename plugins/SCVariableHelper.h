#ifndef _SCVARIABLEHELPER_H
#define _SCVARIABLEHELPER_H

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/Candidate.h"

#include <DataFormats/PatCandidates/interface/Electron.h>

#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "RecoEgamma/EgammaHLTAlgos/interface/EgammaHLTTrackIsolation.h"
#include "EgammaAnalysis/TnPTreeProducer/plugins/WriteValueMap.h"

template <class T>
class SCVariableHelper : public edm::EDProducer {
 public:
  explicit SCVariableHelper(const edm::ParameterSet & iConfig);
  virtual ~SCVariableHelper() ;
  
  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup) override;
  
private:
  const edm::EDGetTokenT<std::vector<T> > probesToken_;
  const edm::EDGetTokenT<reco::TrackCollection> trackProducer_;

  edm::EDGetTokenT<EcalRecHitCollection> recHitsEBToken_;
  edm::EDGetTokenT<EcalRecHitCollection> recHitsEEToken_;

  const bool countTracks_;
  
  const double trkIsoPtMin_; 
  const double trkIsoConeSize_;
  const double trkIsoZSpan_;   
  const double trkIsoRSpan_;  
  const double trkIsoVetoConeSize_;
  const double trkIsoStripBarrel_;
  const double trkIsoStripEndcap_;
  
  EgammaHLTTrackIsolation* isoCalculator_;
};

template<class T>
SCVariableHelper<T>::SCVariableHelper(const edm::ParameterSet & iConfig) :
probesToken_(consumes<std::vector<T> >(iConfig.getParameter<edm::InputTag>("probes"))),
  trackProducer_(consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("trackProducer"))),
  recHitsEBToken_(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("ebRecHits"))),
  recHitsEEToken_(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("eeRecHits"))),
  countTracks_         (iConfig.getParameter<bool>("countTracks")),
  trkIsoPtMin_       (iConfig.getParameter<double>("trkIsoPtMin")),
  trkIsoConeSize_    (iConfig.getParameter<double>("trkIsoConeSize")),
  trkIsoZSpan_       (iConfig.getParameter<double>("trkIsoZSpan")),
  trkIsoRSpan_       (iConfig.getParameter<double>("trkIsoRSpan")),
  trkIsoVetoConeSize_(iConfig.getParameter<double>("trkIsoVetoConeSize")),
  trkIsoStripBarrel_ (iConfig.getParameter<double>("trkIsoStripBarrel")),
  trkIsoStripEndcap_ (iConfig.getParameter<double>("trkIsoStripEndcap")) {
  
  isoCalculator_ = new EgammaHLTTrackIsolation(trkIsoPtMin_, trkIsoConeSize_,
					       trkIsoZSpan_, trkIsoRSpan_, trkIsoVetoConeSize_,
					       trkIsoStripBarrel_, trkIsoStripEndcap_);
  
  produces<edm::ValueMap<float> >("scTkIso");
  produces<edm::ValueMap<float> >("seedGain");
}

template<class T>
SCVariableHelper<T>::~SCVariableHelper()
{}

template<class T>
void SCVariableHelper<T>::produce(edm::Event & iEvent, const edm::EventSetup & iSetup) {

  // read input
  edm::Handle<std::vector<T> > probes;
  iEvent.getByToken(probesToken_,  probes);

  edm::Handle<reco::TrackCollection> trackHandle;
  iEvent.getByToken(trackProducer_, trackHandle);
  const reco::TrackCollection* trackCollection = trackHandle.product();

  // prepare vector for output
  std::vector<float> scIsoValues;

  std::vector<float> seedGains;
  const auto& recHitsEBProd = iEvent.get(recHitsEBToken_);
  const auto& recHitsEEProd = iEvent.get(recHitsEEToken_);
  
  typename std::vector<T>::const_iterator probe, endprobes = probes->end();
  
  for (probe = probes->begin(); probe != endprobes; ++probe) {
    
    float isol;
    if (countTracks_) {
      isol = isoCalculator_->photonTrackCount(&(*probe), trackCollection, false);
    } else {
      isol = isoCalculator_->photonPtSum(&(*probe), trackCollection, false);
    }
    
    scIsoValues.push_back(isol);

    // seed gain loop
    float tmpSeedVal = 12.0;
    auto detid = probe->superCluster()->seed()->seed();
    // -- try EB first
    auto seed_EB = recHitsEBProd.find(detid);
    if( seed_EB != recHitsEBProd.end() ) {
      if (seed_EB->checkFlag(EcalRecHit::kHasSwitchToGain6)) tmpSeedVal = 6.0;
      if (seed_EB->checkFlag(EcalRecHit::kHasSwitchToGain1)) tmpSeedVal = 1.0;
    }
    else { // -- try EE
      auto seed_EE = recHitsEEProd.find(detid);
      if( seed_EE != recHitsEEProd.end() ) {
        if (seed_EE->checkFlag(EcalRecHit::kHasSwitchToGain6)) tmpSeedVal = 6.0;
        if (seed_EE->checkFlag(EcalRecHit::kHasSwitchToGain1)) tmpSeedVal = 1.0;
      }
    }
    seedGains.push_back(tmpSeedVal);


    // const auto& coll = probe->isEB() ? recHitsEBProd : recHitsEEProd;
    // auto seed = coll.find(detid);
    // float tmpSeedVal = 12.0;
    // if (seed != coll.end()){
    //     if (seed->checkFlag(EcalRecHit::kHasSwitchToGain6)) tmpSeedVal = 6.0;
    //     if (seed->checkFlag(EcalRecHit::kHasSwitchToGain1)) tmpSeedVal = 1.0;
    // }
    // seedGains.push_back(tmpSeedVal);
  }

  
  // convert into ValueMap and store
  writeValueMap(iEvent, probes, scIsoValues, "scTkIso");
  writeValueMap(iEvent, probes, seedGains, "seedGain");
}

#endif
