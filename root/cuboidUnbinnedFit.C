#include <fstream>
#include <iostream>
#include <TString.h>
#include <TFile.h>
#include <TMath.h>
#include <TMinuit.h>
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

void        Init(TString inputFile);
extern void Fcn(Int_t &nPar, Double_t *gIn, Double_t &f, Double_t *par, Int_t iFlag);

void        FillOneMeasurement(TH1D *h, Double_t mean, Double_t sigma);
Double_t    CalculateWeightedMean(Bool_t lWidth = kFALSE);


void cuboidUnbinnedFit(int nBins=20)
{
  TString inputFile = TString("../data/cuboidData.dat");
  //TString inputFile = TString("../data/cuboidDataS2015.dat");

  // Read data into our global vectors
  Init(inputFile);
  
  // setup the minimization engine
  TMinuit *minuit = new TMinuit(1);
  minuit->SetFCN(Fcn);

  Int_t    iFlag;
  Double_t argList[10];
  argList[0] = 1;
  minuit->mnexcm("SET ERR",argList,1,iFlag);
  minuit->mnparm(0,"mean",3500,0.1,0.,1000000.,iFlag);

  minuit->mnexcm("CALL FCN",argList,1,iFlag);

  // ask for no more than 100 iterations
  argList[0] = 100;
  // ask for full minimazation and analysis of uncertainties
  minuit->mnexcm("MIGRAD", argList,1,iFlag);
  minuit->mnexcm("HESSE",  argList,1,iFlag);
  minuit->mnexcm("MINOS",  argList,1,iFlag);
  
  // Make sure we have the right styles for the plot
  MitRootStyle::Init(-1);

  // Display our result as good as we can
  TH1D *hVolume    = new TH1D("Volume","Cuboid Volume",nBins,2900,3900);
  MitRootStyle::InitHist(hVolume,"","",kBlue);
  TString titles = TString("; Volume [mm^{3}]") + TString("; Number of Entries");
  hVolume->SetTitle(titles.Data());
  hVolume->SetMarkerSize(1.4);

  // Fill the histogram
  for (UInt_t i=0; i<vVolume.size(); i++) {
    Double_t sigma = uVolume.at(i);
    Double_t mean = vVolume.at(i);
    FillOneMeasurement(hVolume,mean,sigma);
  }
  
  // Calculate the average values
  Double_t mean  = CalculateWeightedMean(kFALSE); //minuit->GetParameter(0,mean,meanE);
  Double_t sigma = CalculateWeightedMean(kTRUE);
  Double_t norm  = double(vVolume.size()) * hVolume->GetBinWidth(0);

  printf(" Normalization:  %f\n",norm);
  printf(" Weighted Mean:  %f\n",mean);
  printf(" Weighted Width: %f\n",sigma);
  
  TCanvas *cv = new TCanvas();
  cv->Draw();
  hVolume->Draw("p");

//  TF1 *f = new TF1("f",
//		   "[2]/TMath::Sqrt(TMath::TwoPi())/[1]*TMath::Exp((x-[0])*(x-[0])/(-2*[1]*[1]))",
//		   2000,5000);
//  // Initialize function
//  f->SetParameter(0,mean);
//  f->SetParameter(1,sigma);
//  f->SetParameter(2,norm);
//  f->SetLineColor(kRed);
//  f->Draw("same");

  cv->SaveAs("../pics/cuboidUnbinned.png");

}

void Init(TString inputFile)
{
  // Initialize the input data from file for the minimization

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
      printf(" Init -- name=%s, v=%8f dv=%8f\n",name.Data(),v,dv);

    // error
    if (dv <= 0)
      printf(" Init -- ERROR -- dv=%8f\n",dv);
     
    // check it worked
    if (! input.good())
      break;

    if (z<34) {
      printf(" Init -- Rejected measurement z=%8f\n",z);
      continue;
    }

    if (dv <= 80) {
      printf(" Init -- Rejected measurement v=%8f  dv=%8f\n",v,dv);
      continue;
    }

    vVolume.push_back(v);
    uVolume.push_back(dv);

    nLines++;
  }
  input.close();
}  

void Fcn(Int_t &nPar, Double_t *gIn, Double_t &f, Double_t *par, Int_t iFlag)
{
  // Calculate the quantity to minimize: chi^2 - or the equivalent 2*log(Prod P)
  
  Double_t sumLogP = 0.;
  const Double_t iSqrtTwoPi = 1.0/TMath::Sqrt(TMath::TwoPi());
  Double_t mean = par[0];
  
  for (UInt_t i=0; i<vVolume.size(); i++) {
    Double_t sigma = uVolume.at(i);
    Double_t x = vVolume.at(i);
    Double_t arg = log(iSqrtTwoPi/sigma) - 1.0*(x-mean)*(x-mean)/(2*sigma*sigma);
    //printf(" fcn -- Adding: %f\n",arg);
    sumLogP += arg;
  }
  
  //printf(" fcn -- value: %f\n",-2.0 * sumLogP);
  f = -2.0 * sumLogP;

  // Use variables not used to make the compiler happy
  if (nPar < 0)
    printf(" fcn -- ERROR -- %f %d",gIn[0],iFlag);
}

void FillOneMeasurement(TH1D *h, Double_t mean, Double_t sigma)
{
  // Fill the Gaussian PDF into the given histogram
  
  TF1 *f = new TF1("f",
		   "1.0/TMath::Sqrt(TMath::TwoPi())/[1]*TMath::Exp((x-[0])*(x-[0])/(-2*[1]*[1]))",
		   2000,5000);
  // Initialize function
  f->SetParameter(0,mean);
  f->SetParameter(1,sigma);
  
  
  for (Int_t i=0; i<=h->GetNbinsX(); i++) {
    Double_t x   = h->GetBinCenter(i);
    Double_t in  = f->Integral(h->GetBinLowEdge(i), h->GetBinLowEdge(i)+h->GetBinWidth(i));
    //printf(" x,in: %f %f\n",x,in);
    h->Fill(x,in);
  }
}

Double_t CalculateWeightedMean(Bool_t lWidth)
{
  // Calculate a weighted mean value
  
  Double_t mean = 0.;
  Double_t weightSum = 0;
  
  // Fill the histogram
  for (UInt_t i=0; i<vVolume.size(); i++) {
    Double_t weight = 1.0/(uVolume.at(i)*uVolume.at(i));
    Double_t value = vVolume.at(i);
    if (lWidth)
      value = uVolume.at(i)*uVolume.at(i);
    mean += weight * value;
    weightSum += weight;
  }

  if (lWidth)
    return sqrt(mean/weightSum);
  else
    return mean/weightSum;
}
