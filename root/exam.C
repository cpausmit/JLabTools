#include <fstream>
#include <iostream>
#include <TString.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TF1.h>
#include <TArrow.h>
#include <TLatex.h>
#include <TMath.h>

#include <MitRootStyle.C>

using namespace std;

void exam(int nBins=50)
{
  TString  inputFile = TString("../data/8.01-exam2.csv");
  TString  name,s;
  Int_t    idx=0,nLines=0,tmp=0,nCounts=0;
  Double_t x=0.,average=0.,width=0.;

  TH1D    *hGrades = new TH1D("Grades","Exam",nBins,0,100);

  MitRootStyle::InitHist(hGrades,"","",kBlue);

  TString titles = TString("; Grade [%]") + TString("; Number of Entries");
  hGrades->SetTitle(titles.Data());
  hGrades->SetMarkerSize(1.4);

  // Now open our input
  printf(" Input file: %s\n",inputFile.Data());
  ifstream input;
  input.open(inputFile.Data());
  while (1) {

    // read in
    input >> x;
    
    // show what we are reading
    if (nLines < 4)
      printf(" x=%8f\n",x);

    // check it worked
    if (! input.good())
      break;

    if (x>1) {
      nCounts++;
      average += x;
      hGrades->Fill(x);
    }
    else {
      printf(" Rejected measurement (z): %f\n",x);
    }
    nLines++;
  }
  if (nCounts>0)
    average = average/nCounts;

  // do not really need second loop, can be calculated in one loop....
  input.close();
  input.open(inputFile.Data());
  while (1) {

    // read in
    input >> x;
    
    // show what we are reading
    if (nLines < 4)
      printf(" x=%8f\n",x);

    // check it worked
    if (! input.good())
      break;

    if (x>1) {
      width += (average-x)*(average-x);
    }
    else {
      printf(" Rejected measurement (z): %f\n",x);
    }
    nLines++;
  }
  input.close();
  if (nCounts>0)
    width = TMath::Sqrt(width/nCounts);
      
  printf("\n Average: %.2f +- %.2f\n\n",average,width);

  
  // Make sure we have the right styles
  MitRootStyle::Init(-1);

  TCanvas *cv = new TCanvas();
  cv->Draw();
  hGrades->Draw("hist");
//  hGrades->Fit("gaus","l");
//
//  TF1 *gaussian = hGrades->GetFunction("gaus");
//  gaussian->SetLineColor(kRed);
//
//  Double_t mean = gaussian->GetParameter(1);
//  Double_t width = gaussian->GetParameter(2);
//
//  char text[40];
//  TLatex latex;
//  latex.SetTextSize(0.034);
//  latex.SetTextAlign(32);
//  latex.SetTextColor(kBlack);
//  latex.SetTextSize(0.034);
//
//  sprintf(text," Mean: %.1f mm^{3}",mean);
//  latex.DrawLatex(3880,14.6,text);
//  sprintf(text," Width: %.1f mm^{3}",width);
//  latex.DrawLatex(3880,14.0,text);

  cv->SaveAs("../pics/grades.png");
}
