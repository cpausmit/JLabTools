#include <time.h>

#include <fstream>
#include <iostream>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TF1.h>
#include <TArrow.h>
#include <TLatex.h>

#include <MitRootStyle.C>

using namespace std;

void makeFrame(TGraphErrors *graph, const char *xtit="kaon p_{T} [GeV]", const char *ytit="quality")
{

  Double_t xMin = graph->GetX()[0]; 
  Double_t xMax = graph->GetX()[graph->GetN()-1]; 
  Double_t xDel = (xMax-xMin)/20.;
  printf(" X(min,max,delta): %f, %f, %f\n",xMin,xMax,xDel);
  Double_t yMin = 99999;
  Double_t yMax = -99999;
  for (Int_t i=0; i<graph->GetN(); i++) {
    Double_t max = graph->GetY()[i]+graph->GetEY()[i];
    Double_t min = graph->GetY()[i]-graph->GetEY()[i];
    if (max>yMax)
      yMax = max;
    if (min<yMin)
      yMin = min;
  }
  Double_t yDel = (yMax-yMin)/20.;
  printf(" Y(min,max,delta): %f, %f, %f\n",yMin,yMax,yDel);

  TH1F *frame = new TH1F("Frame","",1,xMin-xDel,xMax+xDel);
  MitRootStyle::InitHist(frame,xtit,ytit);

  frame->SetMinimum(yMin-yDel);
  frame->SetMaximum(yMax+yDel);
  frame->Draw();

  return;
}


void plotxy()
{
  // Get the data

  TString inputFile = TString("../data/q-versus-ptk.dat");
  printf(" Input file: %s\n",inputFile.Data());
  TGraphErrors graph(inputFile,"%lg %lg %lg");
 
  // Start the plotting

  MitRootStyle::Init(-1);                             // Basic style
  TCanvas *cv = new TCanvas("CV","CV",0,0,1200,1200); // Canvas to draw on
  cv->Draw();
  
  MitRootStyle::InitGraph(&graph,"","",kBlue);        // Graph style
  makeFrame(&graph,"kaon p_{T} [GeV]","quality");
  graph.DrawClone("PESame");
  graph.Print();
  // make a png file to save this plot
  cv->SaveAs("../pics/quality.png");
  
  return;
}
