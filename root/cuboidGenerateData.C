void cuboidGenerateData(Int_t    seed   = 456456,
			Int_t    nDraws = 49,
			Double_t mean   = 3400,
			Double_t width  = 150,
			Double_t nBins  = 200)
{
  TH1D *hVolumeGen = new TH1D("VolumeGen","Generated Cuboid Volume",
			      nBins,2500,4500);

  // generate

  TRandom *rg = new TRandom(seed);
  Double_t vGen;

  for (int i=0;i<nDraws;i++) {
    vGen = rg->Gaus(mean,width);
    hVolumeGen->Fill(vGen);
  }    

  TCanvas *cv = new TCanvas();
  cv->Draw();
  hVolumeGen->Draw("e");
  hVolumeGen->Fit("gaus");
}
