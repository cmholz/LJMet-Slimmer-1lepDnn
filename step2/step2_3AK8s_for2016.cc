#define step2_cxx
#include "step2.h"
#include <fstream>
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TRandom.h>
#include <TRandom3.h>
#include <sstream>
#include <string>
#include <vector>
#include <TLorentzVector.h>
#include <math.h>
using namespace std;

//outputTree->Branch("maxProb_JetSubCalc_PtOrdered",&maxProb_JetSubCalc_PtOrdered);
//outputTree->Branch("AK4HT",&AK4HT,"AK4HT/F");



void step2::Loop(TString inTreeName, TString outTreeName) 
{
  inputTree = (TTree*)inputFile->Get(inTreeName);
  if (inputTree == 0) return;
  if(inputTree->GetEntries() == 0){
    std::cout << "WARNING! Found 0 events in the tree!!!" << std::endl;
    return;
  }

  Init(inputTree);
   
  inputTree->SetBranchStatus("*",1);

  // Makes a copy of the branches with status 1
  outputFile->cd();
  if(isMC && inTreeName == "ljmet"){
    TH1D* numhist = (TH1D*)inputFile->Get("NumTrueHist");
    TH1D* wgthist = (TH1D*)inputFile->Get("weightHist");
    numhist->Write();
    wgthist->Write();
  }
  TTree *outputTree = inputTree->CloneTree(0);

  float dnnJweight, dnnJweight3;
  outputTree->Branch("dnnJweight",&dnnJweight,"dnnJweight/F");
  outputTree->Branch("dnnJweight3",&dnnJweight3,"dnnJweight3/F");
  
  int npassed_all = 0;

  std::vector<float> probjratio = {0.888723637779,1.11117801156,1.09421740743,1.08223856257,1.13353150611,1.09418371362,1.05938130382,1.06588859373,1.04587452994,1.02238163222,1.05197580678,1.06081163644,1.09263537523,1.03832567993,1.02445959078,1.03520162968,1.05014069006,1.05226814363,1.0636255559,0.990454461959,1.02301126096,1.04894966152,0.985485118164,1.0630025804,0.992207629216,0.998163555394,1.00456013924,1.00516357285,1.00662932089,1.0403510253,1.02382292135,0.993370512007,1.07461638378,1.08109039718,1.02000585971,1.05098651646,1.05597873497,1.06921240199,1.064272571,1.088499045,1.07565089285,1.09465726634,1.0988973526,1.08873660155,1.05787653148,1.01511236619,0.953563428259,0.903901679297,0.844111467478,1.84407493987};

  Long64_t nentries = inputTree->GetEntriesFast();
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = inputTree->GetEntry(jentry);   nbytes += nb;
    if (Cut(ientry) != 1) continue; 
    
    //      if (ientry > 5000) continue;
    
    if(jentry % 10000 ==0) std::cout<<"Completed "<<jentry<<" out of "<<nentries<<" events"<<std::endl;
    
    if( MCPastTrigger < 1 || DataPastTrigger < 1 || NJetsAK8_JetSubCalc < 3 || corr_met_MultiLepCalc < 50. || leptonPt_MultiLepCalc < 55) continue;

    dnnJweight = 1;
    dnnJweight3 = 1;
    if (NJetsAK8_JetSubCalc > 0) dnnJweight *= probjratio.at(floor(dnn_J_DeepAK8Calc_PtOrdered->at(0)/0.02));
    if (NJetsAK8_JetSubCalc > 1) dnnJweight *= probjratio.at(floor(dnn_J_DeepAK8Calc_PtOrdered->at(1)/0.02));
    if (NJetsAK8_JetSubCalc > 2) dnnJweight3 = dnnJweight*probjratio.at(floor(dnn_J_DeepAK8Calc_PtOrdered->at(2)/0.02));

    npassed_all++;
    outputTree->Fill(); 
  }
  std::cout<<"NPassed ALL = " << npassed_all << "/"<<nentries<<std::endl;
  std::cout << "DONE" << std::endl;
  outputTree->Write();
 
}
