// Random generator available as global
TRandom *gRandom = 0;

Int_t measure(Double_t rate, Double_t interval);

void countingExperiment(Int_t    seed     = 456456,
			Int_t    nDraws   = 10000,
		        Double_t rate     = 1,
		        Double_t interval = 1,
			Double_t mean     = 3400,
			Double_t width    = 150,
			Double_t nBins    = 100)
{
  Double_t delta = 10 * sqrt(rate * interval);

  TH1D *hCounts = new TH1D("Counts",":Number of Measurements:Counts of Radioactive Decays",
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
  // rate.

  Double_t mean = rate * interval;
  Int_t    nDecays = gRandom->Poisson(mean);

  return nDecays;
}
