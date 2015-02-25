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

// Global
std::vector<double> vVolume; // value of volumes
std::vector<double> uVolume; // uncertainties of volumes

extern void fcn(Int_t &nPar, Double_t *gin, Double_t &f, Double_t *par, Int_t iFlag);
void Init(TString inputFile);

void cuboid(int nBins=20)
{
  TString inputFile = TString("../data/cuboidData.dat");
  //TString inputFile = TString("../data/cuboidDataS2015.dat");

  // Read data into our global vectors
  Init(inputFile);
  
  TMinuit *minuit = new TMinuit(1);
  minuit->SetFCN(fcn);
  
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

void Init(TString inputFile) {
  // Initialize the inut data from file for the minimization

  // clear our input data
  vVolume.clear();
  uVolume.clear();
  
  // open the file
  printf(" Input file: %s\n",inputFile.Data());
  ifstream input;
  input.open(inputFile.Data());

  TString  name,s;
  Int_t    idx=0,nLines=0;
  Double_t x,dx,y,dy,z,dz,v,dv;

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

    vVolume.append(v);
    uVolume.append(dv);

    nLines++;
  }
  input.close();
}  


void fcn(Int_t &nPar, Double_t *gin, Double_t &f, Double_t *par, Int_t iFlag)
{
  // Calculate the quantity to minimize: chi^2 - or the equivalent 2*log(Prod P)
  Double_t prob = 0.;

  
  //f = 2.0 * log(p);
}
