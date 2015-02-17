void cuboid(int nBins=20)
{
  TString inputFile = TString("../data/cuboidData.dat");
  //TString inputFile = TString("../data/cuboidDataS2015.dat");

  // Now open our input
  printf(" Input file: %s\n",inputFile.Data());

  ifstream input;
  input.open(inputFile.Data());

  TString  name,s;
  Int_t    idx=0,nLines=0;
  Double_t x,dx,y,dy,z,dz,v,dv;

  TH1D *hVolume    = new TH1D("Volume","Cuboid Volume",nBins,2500,4500);
  TH1D *hVolumeGen = new TH1D("VolumeGen","Generated Cuboid Volume",
			      nBins,3000,4000);

  while (1) {

    // read in
    input >> name >> idx >> x >> dx >> y >> dy >> z >> dz >> v >> dv;


    // show what we are reading
    if (nLines < 5)
      printf(" name=%s, v=%8f dv=%8f\n",name.Data(),v,dv);

    // check it worked
    if (! input.good())
      break;


    //if (v<3200 || v>3900)
    //  continue;

    hVolume->Fill(v);

    nLines++;
  }
  input.close();

  TCanvas *cv = new TCanvas();
  cv->Draw();
  hVolume->Draw("e");

  hVolume->Fit("gaus");
}
