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

void dropboxTransfers(Int_t nBins = 24)
{
  TString inputFile = TString("../data/DropboxUploadMonitor.dat");

  // Now open our input
  printf(" Input file: %s\n",inputFile.Data());

  ifstream input;
  input.open(inputFile.Data());

  TString  name,s;
  Int_t    nLines=0;
  Double_t     usedGb=0,lastUsedGb=0;
  Int_t      startSec=0,    endSec=0;
  Int_t  lastStartSec=0,lastEndSec=0;
  
  // Loop through data section of the file just to count
  while (1) {
    // read in
    input >> usedGb >> startSec >> endSec; 
    // show what we are reading
    if (nLines < 10)
      cout << usedGb << " " << startSec << " " << endSec << " " << endl;
    // check it worked
    if (! input.good()) {
      break;
    }
    nLines++;
  }
  input.close();

  // Now read the complete file
  printf(" nLines: %d\n",nLines);

  const int numVals = nLines;
  double xVals[numVals];
  double yVals[numVals];

  nLines=0; lastUsedGb=0; lastStartSec=0; lastEndSec=0;

  Double_t xMax=0, xMin=9999999999, max = 0;
  input.open(inputFile.Data());
  while (1) {
    // read in
    input >> usedGb >> startSec >> endSec; 
    // show what we are reading
    if (nLines < 5)
      cout << usedGb << " " << startSec << " " << endSec << " " << endl;

    // check it worked
    if (! input.good()) {
      break;
    }
    
    // keep track of the total sum
    if (lastUsedGb > 0) {
      xVals[nLines-1] = startSec;
      yVals[nLines-1] = (usedGb - lastUsedGb)/(startSec - lastStartSec)*1000.;

      if (yVals[nLines-1] < 0)
	printf(" Data decreased!! %f -> %f\n",lastUsedGb,usedGb);

      
      if (startSec < xMin)
	xMin = startSec;
      if (startSec > xMax)
	xMax = startSec;
      if (yVals[nLines-1] > max)
	max = yVals[nLines-1];
    }
      
    // update the last values
    lastUsedGb = usedGb;
    lastStartSec = startSec;
    lastEndSec = endSec;
    
    nLines++;
  }
  input.close();

  // Start the plotting

  // Make sure we have the right styles
  MitRootStyle::Init(-1);
  
  TCanvas *cv = new TCanvas();
  cv->Draw();

  Double_t xDel = (xMax-xMin)/20.;
  TH1D *hRate = new TH1D("Rate","Rate",nBins,xMin-xDel,xMax+xDel);
  hRate->SetMaximum(max*1.2);
  MitRootStyle::InitHist(hRate,"","",kBlue);

  TString titles = TString(";epoch time [sec]") + TString("; instantaneous rate [MB/sec]");
  hRate->SetTitle(titles.Data());
  hRate->SetMarkerSize(1.4);
  hRate->Draw();
  
  TGraph* graph = new TGraph(numVals-1,xVals,yVals);
  graph->SetLineColor(2);
  graph->SetLineWidth(2);
  graph->SetMarkerColor(4);
  graph->SetMarkerStyle(21);
  graph->SetMarkerSize(0.4);

  //graph->Draw(); 
  graph->Draw("LP"); 

  // make a png file to save this plot
  cv->SaveAs("../pics/rate.png");
  
  return;
}
