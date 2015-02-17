#include <fstream>
#include <iostream>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TArrow.h>
#include <TLatex.h>

#include <MitRootStyle.C>

using namespace std;

void snowFall(int nBins = 24)
{
  TString inputFile = TString("../data/snowFall.dat");

  // Now open our input
  printf(" Input file: %s\n",inputFile.Data());

  ifstream input;
  input.open(inputFile.Data());

  TString  name,s;
  Int_t    nLines=0;
  Double_t fJul,fAug,fSep,fOct,fNov,fDec,fJan,fFeb,fMar,fApr,fMay,fJun,fTot;
  
  TH1D    *hSnowFall = new TH1D("SnowFall","Snow Fall per Year",nBins,0.,120.);
  MitRootStyle::InitHist(hSnowFall,"","",kBlue);

  TString titles = TString("; Yearly Snow Fall [inches]") + TString("; Number of Entries");
  hSnowFall->SetTitle(titles.Data());
  hSnowFall->SetMarkerSize(1.4);

  // Read the header line
  input >> name >> s >> s >> s >> s >> s >> s >> s >> s >> s >> s >> s >> s >> s;

  // Loop through data section of the file
  while (1) {

    // read in
    input >> name
	  >> fJul >> fAug >> fSep >> fOct >> fNov >> fDec
	  >> fJan >> fFeb >> fMar >> fApr >> fMay >> fJun
	  >> fTot;

    // show what we are reading
    if (nLines < 5)
      cout << name
	   << " " << fJul << " " << fAug << " " << fSep << " " << fOct << " " << fNov << " " << fDec
	   << " " << fJan << " " << fFeb << " " << fMar << " " << fApr << " " << fMay << " " << fJun
	   << " " << fTot << endl;

    // check it worked
    if (! input.good()) {
      cout << name << " ERROR in reading!" << endl;
      break;
    }
      
    hSnowFall->Fill(fTot);

    nLines++;
  }
  input.close();

  // Make sure we have the right styles
  MitRootStyle::Init(-1);
  
  TCanvas *cv = new TCanvas();
  cv->Draw();
  
  hSnowFall->Draw("e");
  hSnowFall->SetLineColor(kBlack);
  hSnowFall->Fit("gaus","L");

  TArrow* arrow = new TArrow(92.5,0.1,92.5,8.,0.03,"<|");
  arrow->SetAngle(40);
  arrow->SetLineWidth(2);
  arrow->Draw();

  TLatex latex;
  latex.SetTextSize(0.034);
  latex.SetTextAlign(22);
  latex.DrawLatex(92.5,9.5,"2014/15 (Feb. 17)");
  latex.SetTextColor(kRed);
  latex.SetTextSize(0.020);
  latex.DrawLatex(92.5,8.8,"How Likely?  G: 2.44 std, A: 2 of 120");

  latex.SetTextAlign(32);
  latex.SetTextSize(0.034);
  latex.DrawLatex(118,20.,"Average: 43.7''");
  latex.DrawLatex(118,18.,"Mean (G): 41.8''");
  latex.DrawLatex(118,17.,"Width (G): 20.8''");

  latex.SetTextColor(kRed);
  latex.DrawLatex(118,15.8,"Fit Prob. (G): 72%");

  return;
}
