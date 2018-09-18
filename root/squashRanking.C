#include <time.h>

#include <fstream>
#include <iostream>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TF1.h>
#include <TArrow.h>
#include <TLatex.h>

#include <MitRootStyle.C>

using namespace std;

long int getSeconds(const char* timestr)
{
  struct tm t = {0,0,0,0,0,0,0,0,0,0,0};  // Initalize to all 0's
  strptime(timestr,"%Y-%m-%dT00:00:00Z", &t);
  time_t timeSinceEpoch = mktime(&t);
  
  return timeSinceEpoch;
}

void squashRanking()
{
  TString inputFile = TString("../data/squashRanking.dat");
  printf(" Input file: %s\n",inputFile.Data());
  ifstream input(inputFile.Data());


  // #2013-2014 	2014-01-08 	Adult M 3.5 	192 	0 	3.9 	0 	US SQUASH
  TString  name,s;
  TString  season,date,age,gender,country,sport;
  Double_t pClass,rank,fake0,rating,fake1;

  Int_t    nLines=0;
  
  // Loop through data section of the file just to count
  while (! input.eof()) {
    // read in
    input >> season >> date >> age >> gender >> pClass >> rank >> fake0 >> rating >> fake1 >> country >> sport;
//    cout << " i: " << nLines << " "
//         << season << " " << date << " " << age << " " << gender << " " << pClass << " "
//	   << rank << " " << fake0 << " " << rating << " " << fake1 << " " << country << " "
//	   << sport << " " << endl;

    // check if we are at the end
    if (input.eof()) {
      printf(" INFO - we are through this file (%d).\n",nLines);
      break;
    }
    nLines++;
  }

  input.close();

  const int numVals = nLines;
  double    xVals[numVals];
  double    yVals[numVals];

  nLines=0;

  Double_t xMin=9999999999, xMax=0, min = 3.5, max = 0.0;

  input.open(inputFile.Data());
  while (! input.eof()) {

    // read in
    input >> season >> date >> age >> gender >> pClass >> rank >> fake0 >> rating >> fake1 >> country >> sport;

    xVals[nLines] = getSeconds(date.Data()) / 1000000.;
    yVals[nLines] = rating;
    printf(" X: %f Y: %f\n",xVals[nLines],yVals[nLines]);

    if (xVals[nLines] < xMin)
      xMin = xVals[nLines];
    if (xVals[nLines] > xMax)
      xMax = xVals[nLines];
    if (yVals[nLines] > max)
      max = yVals[nLines];
    
    // check if we are at the end
    if (input.eof()) {
      printf(" INFO - we are through this file (%d).\n",nLines);
      break;
    }
    nLines++;
  }
  input.close();

  // Start the plotting

  // Make sure we have the right styles
  MitRootStyle::Init(-1);
  
  TCanvas *cv = new TCanvas("CV","CV",0,0,1200,1200);
  cv->Draw();

  Double_t xDel = (xMax-xMin)/20.;
  TH1D *hRating = new TH1D("Rating","Rating",10,xMin-xDel,xMax+xDel);
  hRating->SetMaximum(max*1.1);
  hRating->SetMinimum(min);
  MitRootStyle::InitHist(hRating,"","",kBlue);

  TString titles = TString(";epoch time/1M [sec]") + TString("; squash rating");
  hRating->SetTitle(titles.Data());
  hRating->SetMarkerSize(1.4);
  hRating->Draw();
  
  TGraph* graph = new TGraph(numVals-1,xVals,yVals);
  graph->SetLineColor(2);
  graph->SetLineWidth(2);
  graph->SetMarkerColor(4);
  graph->SetMarkerStyle(21);
  graph->SetMarkerSize(0.4);

  //graph->Draw(); 
  graph->Draw("LP"); 

  // make a png file to save this plot
  cv->SaveAs("rating.png");
  
  return;
}
