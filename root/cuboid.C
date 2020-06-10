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

Bool_t gCleanup = false;

void cuboid(int nBins=20)
{
  //TString inputFile = TString("../data/cuboidData.dat");
  //TString inputFile = TString("../data/cuboidDataS2015.dat");
  //TString inputFile = TString("../data/cuboidDataF2018B1.dat");
  //TString inputFile = TString("../data/cuboidDataF2018B2.dat");
  //TString inputFile = TString("../data/cuboidDataF2018B3.dat");
  //TString inputFile = TString("../data/cuboidDataF2018B4.dat");
  //TString inputFile = TString("../data/cuboidDataF2019B2.dat");
  TString inputFile = TString("../data/cuboidDataF2019B02.dat");

  // Now open our input
  printf(" Input file: %s\n",inputFile.Data());

  ifstream input;
  input.open(inputFile.Data());

  TString  name,s;
  Int_t    idx=0,nLines=0;
  Double_t x,dx,y,dy,z,dz,v,dv;

  TH1D *hV = new TH1D("V","Cuboid V [mm^{3}]",nBins,2900,3900);
  TH1D *hX = new TH1D("x","Cuboid x [mm]",nBins,8.5,10.5);
  TH1D *hY = new TH1D("y","Cuboid y [mm]",nBins,8.7,11);
  TH1D *hZ = new TH1D("z","Cuboid z [mm]",nBins,34,40);

  // create histograms
  TH1D *h[4];
  h[0] = hX; 
  h[1] = hY; 
  h[2] = hZ; 
  h[3] = hV; 

  // Define working histogram
  TH1D* hist = h[3];
  TString titles = TString("; ") + TString(hist->GetTitle()) + TString("; Number of Entries");
  
  // Make sure we have the right styles
  MitRootStyle::Init(0);
  MitRootStyle::InitHist(hist,"","",kBlue);

  printf(" Title: %s\n",titles.Data());
  hist->SetTitle(titles.Data());
  hist->SetMarkerSize(1.4);

  while (1) {

    // read in
    input >> name >> idx >> x >> dx >> y >> dy >> z >> dz >> v >> dv;

    // show what we are reading
    if (nLines < 20)
      printf(" name=%s, v=%8f dv=%8f\n",name.Data(),v,dv);

    // check it worked
    if (! input.good())
      break;

    if (gCleanup && z<34) {
      printf(" Rejected measurement (z): %f\n",z);
      continue;
    }

    printf("Volume: %f\n",v);
    hV->Fill(v);
    hX->Fill(x);
    hY->Fill(y);
    hZ->Fill(z);

    nLines++;
  }
  input.close();

  printf(" Number of lines: %d\n",nLines);
  
  TCanvas *cv = new TCanvas();
  cv->Draw();
  gStyle->SetOptStat(1110010);

  hist->Draw("e");
  hist->Fit("gaus","l");

  hist->SetNdivisions(505);
  hist->Draw("e");
  TF1 *gaussian = hist->GetFunction("gaus");
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

  cv->SaveAs("../pics/cuboid.png");
}
