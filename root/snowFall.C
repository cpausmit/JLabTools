#include <fstream>
#include <iostream>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TF1.h>
#include <TArrow.h>
#include <TLatex.h>

#include <MitRootStyle.C>

using namespace std;

void snowFall(Double_t snow2015 = 108.6, Int_t nBins = 24)
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
  Int_t    nYears = 0;
  Double_t fTotSum = 0.;
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
      //cout << name << " ERROR in reading!" << endl;
      break;
    }
      
    // keep track of the total sum
    fTotSum += fTot;
    nYears++;
    
    hSnowFall->Fill(fTot);

    nLines++;
  }
  input.close();

  // Start the plotting
  
  // Make sure we have the right styles
  MitRootStyle::Init(-1);
  
  TCanvas *cv = new TCanvas();
  cv->Draw();
  
  hSnowFall->Draw("e");
  hSnowFall->SetLineColor(kBlack);
  hSnowFall->Fit("gaus","L");

  TF1 *gaussian = hSnowFall->GetFunction("gaus");
  gaussian->SetLineColor(kRed);
  gaussian->Draw("same");
  
  // Calculate probabilities
  Double_t p = gaussian->Integral(snow2015,1000.)/gaussian->Integral(-1000.,1000.);
  cout << " Probability: " << p << endl;

  char text[120];

  
  TArrow* arrow = new TArrow(snow2015,0.1,snow2015,8.,0.03,"<|");
  arrow->SetAngle(40);
  arrow->SetLineWidth(2);
  arrow->Draw();

  TLatex latex;
  latex.SetTextSize(0.034);
  latex.SetTextAlign(32);
  latex.DrawLatex(118,9.5,"2014/15 (Mar. 16)");

  latex.SetTextColor(kRed);
  latex.SetTextSize(0.020);
  sprintf(text," Likely? p=%.2f%%; #sigma=%.2f",100.*p,
	  fabs(snow2015-gaussian->GetParameter(1))/gaussian->GetParameter(2));
  latex.DrawLatex(118,8.8,text);

  latex.SetTextAlign(32);
  latex.SetTextColor(kBlack);
  latex.SetTextSize(0.034);
  sprintf(text," Average: %.1f''",fTotSum/max(nYears,1));
  latex.DrawLatex(118,20.,text);
  sprintf(text," Mean: %.1f''",gaussian->GetParameter(1));
  latex.DrawLatex(118,18.,text);
  sprintf(text," Width: %.1f''",gaussian->GetParameter(2));
  latex.DrawLatex(118,17.,text);

  latex.SetTextColor(kRed);
  latex.DrawLatex(118,15.8,"Fit Prob. (G): 72%");

  latex.SetTextColor(kRed);
  latex.SetTextSize(0.06);
  latex.DrawLatex(110,13,"RECORD !");

  cv->SaveAs("snowFallBoston.png");
  
  return;
}
