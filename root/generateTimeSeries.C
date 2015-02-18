#include <TRandom.h>
#include <TCanvas.h>
#include <TH1D.h>

//#include <MitRootStyle.C>

// Random generator available as global
TRandom *gRandom = 0;

void  setupRandom(Int_t seed);
Int_t measureNDecays(Double_t rate, Double_t interval);
void  fillDecayTimes(const Int_t nDecays, Double_t decayTimes[]);

//--------------------------------------------------------------------------------------------------
// This function simulates a classical counting experiment, where you have a radioactive source that
// has a very large number of potentially decaying atoms and a very long half-life.  Typical would
// be Strontium-90 (http://en.wikipedia.org/wiki/Strontium-90) with a half-life of 28.8 years. This
// is a beta decay (e-) into Yttrium-90. The implementation is done such that it assumes a really
// constant rate.
//
// The function first determines how many radiocative decays it really recorded in the given time
// interval at the specificed average rate and then generates a times series recording the time for
// for each of the decays observed. From the time series we can first see whether the rate is
// actually statistically speaking constant during the interval. We can also record the time
// differences between two events to derive from the plot the meaning of average time difference
// between events and the most likely difference (median).
// --------------------------------------------------------------------------------------------------

void generateTimeSeries(Int_t    seed     = 46456,
		        Double_t rate     = 10,
		        Double_t interval = 1000,
			Double_t nBins    = 100)
{
  // frame work where we setup the experiment and run in the measuring loop

  // make sure to initialize the random number generator
  setupRandom(seed);
  
  // use rate and interval to determine the number of observed decays in the interval
  const Int_t nObserved = measureNDecays(rate,interval);

  // book and generate the specific decay times 
  Double_t decayTimes[nObserved];
  fillDecayTimes(nObserved,decayTimes);
  
  // analyze the time series (it is ordered already)
  TH1D *hTimes  = new TH1D("DecayT",";Number of Counts;Time [sec]",nBins,0.,interval);
  TH1D *hDeltaT = new TH1D("DeltaT",";Number of Entries;Delta Time [sec]",nBins,0.,5.0*(1.0/rate));

  // generate
  Double_t lastTime = 0.;

  Double_t tTotal = 0;
  for (Int_t i=0; i<nObserved; i++) {
    Double_t deltaT = decayTimes[i] - lastTime;

    // book relevant histograms
    hTimes->Fill(decayTimes[i] * interval);
    hDeltaT->Fill(deltaT * interval);

    tTotal += deltaT * interval;
    
    // make sure to update the lastTime
    lastTime = decayTimes[i];
  }
  
  
  printf(" Total time is: %f\n",tTotal);
  printf(" Rate is      : %f\n",nObserved/interval);
  
  // draw the plot
  TCanvas *cv = new TCanvas();
  cv->Draw();
  //hTimes->Draw("e");
  hDeltaT->Draw("e");
  hDeltaT->Fit("expo");

  // cleanup
  if (gRandom)
    delete gRandom;
}

void setupRandom(Int_t seed)
{
  // initialize the random generator
  if (gRandom)
    delete gRandom;
  gRandom = new TRandom(seed);
}

Int_t measureNDecays(Double_t rate, Double_t interval)
{
  // Make one measurement of the number of counts in a given time interval with a given expected
  // rate. The return value is the number of counts (an integer of course).

  Double_t mean = rate * interval;
  Int_t    nDecays = gRandom->Poisson(mean);

  return nDecays;
}

void fillDecayTimes(const Int_t nDecays, Double_t decayTimes[])
{
  printf(" Generate %d times\n",nDecays);
  
  Double_t x[nDecays];
  for (Int_t i=0; i<nDecays; i++) {
    x[i] = gRandom->Uniform();
    //printf(" Next Time: %f\n",x[i]);
  }
  
  // allright, sorting it by hand
  // Int_t    idxLast =   -1;
  Double_t lowest  =  1.1;
  Double_t last    = -0.1;
  
  for (Int_t i=0; i<nDecays; i++) { 

    // search next lowest time (exactly equal values are not handled correctly)
    lowest = 1.1;             // reset our lowest to high value
    for (Int_t j=0; j<nDecays; j++) {
      if (x[j] < lowest && x[j] > last) {
	//idxLast = j;
	lowest  = x[j];
      }
    }

    // store this time
    decayTimes[i] = lowest;
    //printf(" Ordered Times: %f  (%d)\n",lowest,idxLast);

    // remember how far we got
    last = lowest;
  }
}
