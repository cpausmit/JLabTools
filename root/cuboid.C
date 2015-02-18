#include <fstream>
#include <iostream>
#include <TString.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TF1.h>
#include <TArrow.h>
#include <TLatex.h>

#include <MitRootStyle.C>

using namespace std;

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

  TH1D *hVolume    = new TH1D("Volume","Cuboid Volume",nBins,2900,3900);
//  TH1D *hVolumeGen = new TH1D("VolumeGen","Generated Cuboid Volume",
//			      nBins,3000,4000);

  MitRootStyle::InitHist(hVolume,"","",kBlue);

  TString titles = TString("; Volume [mm^{3}]") + TString("; Number of Entries");
  hVolume->SetTitle(titles.Data());
  hVolume->SetMarkerSize(1.4);

  while (1) {

    // read in
    input >> name >> idx >> x >> dx >> y >> dy >> z >> dz >> v >> dv;


    // show what we are reading
    if (nLines < 4)
      printf(" name=%s, v=%8f dv=%8f\n",name.Data(),v,dv);

    // check it worked
    if (! input.good())
      break;

    if (z<34) {
      printf(" Rejected measurement (z): %f\n",z);
      continue;
    }
      
    hVolume->Fill(v);

    nLines++;
  }
  input.close();

  // Make sure we have the right styles
  MitRootStyle::Init(-1);

  TCanvas *cv = new TCanvas();
  cv->Draw();
  hVolume->Draw("e");
  hVolume->Fit("gaus","l");

  TF1 *gaussian = hVolume->GetFunction("gaus");
  gaussian->SetLineColor(kRed);

  Double_t mean = gaussian->GetParameter(1);
  Double_t width = gaussian->GetParameter(2);

  char text[40];
  TLatex latex;
  latex.SetTextSize(0.034);
  latex.SetTextAlign(32);
  latex.SetTextColor(kBlack);
  latex.SetTextSize(0.034);

  sprintf(text," Mean: %.1f mm^{3}",mean);
  latex.DrawLatex(3880,14.6,text);
  sprintf(text," Width: %.1f mm^{3}",width);
  latex.DrawLatex(3880,14.0,text);

  cv->SaveAs("cuboid.png");
}
