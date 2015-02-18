// Random generator available as global
TRandom *gRandom = 0;

Int_t measure(Double_t rate, Double_t interval);

//--------------------------------------------------------------------------------------------------
// This function simulates a classical counting experiment, where you have a radioactive source that
// has a very large number of potentially decaying atoms and a very long half-life.  Typical would
// be Strontium-90 (http://en.wikipedia.org/wiki/Strontium-90) with a half-life of 28.8 years. This
// is a beta decay (e-) into Yttrium-90. The implementation is done such that it assumes a really
// constant rate.
//
// The function makes a histogram of the observed counts and will fit the distribution to a Gaussian
// which it of course is not but usually does a pretty decent job at it. One of the interesting
// things to study here is to see how many experiments you need to perfrom to show a significant
// deviation from Gaussian behavior. Quite a lot actually!
// --------------------------------------------------------------------------------------------------

void countingExperiment(Int_t    seed     = 456456,
			Int_t    nDraws   = 10000,
		        Double_t rate     = 1,
		        Double_t interval = 1,
			Double_t mean     = 3400,
			Double_t width    = 150,
			Double_t nBins    = 100)
{
  // frame work where we setup the experiment and run in the measuring loop

  Double_t delta = 10 * sqrt(rate * interval);

  TH1D *hCounts = new TH1D("Counts",";Number of Measurements;Counts of Radioactive Decays",
			   nBins,rate*interval-delta,rate*interval+delta);

  // initialize the random generator
  if (gRandom)
    delete gRandom;
  gRandom = new TRandom(seed);

  // generate
  for (Int_t i=0; i<nDraws; i++) {
    nDecays = measure(rate,interval);
    hCounts->Fill(nDecays);
  }

  // make the plot
  TCanvas *cv = new TCanvas();
  cv->Draw();
  hCounts->Draw("e");
  hCounts->Fit("gaus");

  // cleanup
  if (gRandom)
    delete gRandom;
}

Int_t measure(Double_t rate, Double_t interval)
{
  // Make one measurement of the number of counts in a given time interval with a given expected
  // rate. The return value is the number of counts.

  Double_t mean = rate * interval;
  Int_t    nDecays = gRandom->Poisson(mean);

  return nDecays;
}
