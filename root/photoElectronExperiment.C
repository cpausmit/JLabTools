//--------------------------------------------------------------------------------------------------
// This function simulates a classical counting experiment, where you have a light source that is
// attenuated until you have a very low number of photons passing through and hitting the connected
// photomultiplier.
//
// The function first determines how many photons it really recorded in the given time interval at
// the specified average rate and then generates a times series recording the time for for each of
// the photons observed.
//
// From the time series we can first see whether the rate is statistically speaking constant during
// the interval. We can also record the time differences between two events to derive from the plot
// the meaning of average time difference between events and the most likely difference (median).
// --------------------------------------------------------------------------------------------------
#include <vector>

using namespace std;
typedef std::vector<double> dbls;

#include <TMath.h>
#include <TRandom.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH1D.h>

#include <MitRootStyle.C>

// Random generator available as global
TRandom *gRandom = 0;

void  setupRandom(Int_t seed);
Int_t measureNPhotons(Double_t rate, Double_t interval);
vector<double> fillObservationTimes(const Int_t nDecays);

void photoElectronExperiment(Int_t    seed         = 46456, // random seed to get different setups
			     Double_t meanNPhotons = 1.8,   // mean number of photons in interval
			     Double_t interval     = 4.5,   // interval measured in nano seconds
			     Int_t    nIntervals   = 1000)  // number of intervals to measure photons
{
  // first we need to setup a very long time interval (compared to the one we measure) and generate
  // the time series of uniformly distributed photons

  printf("\n Number of measurements (intervals): %d\n",nIntervals);
  printf(" Mean number of photons in interval: %.1f\n",meanNPhotons);
  printf(" Duration of interval              : %.1f ns\n",interval);

  // - rate of photons [1/ns]
  Double_t rate = meanNPhotons/interval;
  // - setup the long time interval for the time series (not perfect but good enough)
  Double_t longInterval = 100 * nIntervals * interval;
  // - how many decays during the long interval (not exact but for long interval small effect)
  const Int_t nPhotons = longInterval*rate;
  // - fill in time uniformly distributed ([0-1])
  vector<double> observationTimes = fillObservationTimes(nPhotons);
  // - observation times have to be projected into our long interval
  for (Int_t i=0;i<nPhotons;i++)
    observationTimes[i] = observationTimes[i]*longInterval;

  // now we use the long interval to cut out independent intervals for
  // our measurement and count the number of photons

  // - helpers o keep track of the relevant interval borders and the last used photon index
  Double_t lower = 0.0, upper = interval;
  Int_t iLast = 0;
  // - book the measured photons per interval
  Int_t nObservedPhotons[nIntervals];

  for (Int_t i=0;i<nIntervals;i++) {
    // find number of photons within that interval
    Int_t nPhotonsInInterval = 0;
    for (Int_t j=iLast;j<nPhotons;j++) {
      if (observationTimes[j]>lower && observationTimes[j]<=upper) {
	nPhotonsInInterval++; // one more photon found
	iLast = j;
      }
      if (observationTimes[j]>upper) {
	nObservedPhotons[i] = nPhotonsInInterval;
	break;
      }
    }
    // move to the next interval
    lower += interval;
    upper += interval;
  }
  
  // now make our histograms

  // - make sure we have the right styles
  MitRootStyle::Init(-1);

  Int_t nBins = meanNPhotons * 6;

  // - make the relevant histograms
  TH1D *hFrame = new TH1D("Frame",";Number of Intervals;Number of Photons",1,0.,nBins);
  MitRootStyle::InitHist(hFrame,"Number of Photons","Number of Intervals",kBlack);  
  TH1D *hNPhotons = new TH1D("NPhotons",";Number of Intervals;Number of Photons",nBins,-0.5,nBins-0.5);
  MitRootStyle::InitHist(hNPhotons,"Number of Photons","Number of Intervals",kBlack);  
  TH1D *hNPhotonsP = new TH1D("NPhotonsP",";Number of Intervals;Number of Photons",nBins,-0.5,nBins-0.5);
  MitRootStyle::InitHist(hNPhotonsP,"Number of Photons","Number of Intervals",kBlack);  

  for (Int_t i=0; i<nIntervals; i++) {
    // fill relevant histograms
    hNPhotons->Fill(nObservedPhotons[i]);
    hNPhotonsP->Fill(nObservedPhotons[i]);
  }

  TF1 *pois = new TF1("pois","[0]*TMath::Poisson(x,[1])",0,50);
  pois->SetParameters(nIntervals,1.0);
  
  // Make the plot
  TCanvas *cv = new TCanvas("cv","multipads",1600,1600);
  cv->Divide(1,1,0,0);
  cv->cd(1);
  MitRootStyle::InitSubPad(gPad);

  Double_t delta = max(1.2*TMath::Sqrt(hNPhotons->GetMaximum()),0.2*hNPhotons->GetMaximum());
  hFrame->SetMaximum(hNPhotons->GetMaximum()+delta);
  hFrame->Draw("");

  hNPhotons->SetMarkerColor(kBlue);
  hNPhotons->SetMarkerSize(1.6);
  hNPhotons->SetLineWidth(4.0);
  hNPhotons->Draw("epSame");
  hNPhotons->Fit("gaus");
  hNPhotons->GetFunction("gaus")->SetLineColor(kRed);
  hNPhotons->GetFunction("gaus")->SetLineWidth(4.0);
  hNPhotonsP->SetMarkerColor(kBlue);
  hNPhotonsP->SetMarkerSize(1.6);
  hNPhotonsP->SetLineWidth(4.0);
  hNPhotonsP->Draw("epSame");
  hNPhotonsP->Fit(pois);
  hNPhotonsP->GetFunction("pois")->SetLineColor(kGreen);
  hNPhotonsP->GetFunction("pois")->SetLineWidth(4.0);

  
}

void setupRandom(Int_t seed)
{
  // Initialize the random generator.

  if (gRandom)
    delete gRandom;
  gRandom = new TRandom(seed);
}

Int_t measureNPhotons(Double_t rate, Double_t interval)
{
  // Make one measurement of the number of counts in a given time interval with a given expected
  // rate. The return value is the number of counts (an integer of course).

  Double_t mean = rate * interval;
  Int_t    nDecays = gRandom->Poisson(mean);

  return nDecays;
}

vector<double>
fillObservationTimes(const Int_t nPhotons)
{
  // For a given number of photons generate the corresponding times assuming a flat distribution. The
  // time interval is normalized to one, so all values are between 0 and 1.

  printf("\n N Photons: %d (generate and sort)\n",nPhotons);

  vector<double> x;
  for (Int_t i=0; i<nPhotons; i++) {
    Double_t r = gRandom->Uniform();
    // this is not the fastest way to do it
    dbls::iterator it = std::lower_bound(x.begin(),x.end(),r);
    x.insert(it,r);
  }

  printf(" N Photons: %d -- DONE\n\n",nPhotons);

  return x;
}
