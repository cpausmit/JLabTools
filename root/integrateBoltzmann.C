#include <TROOT.h>
#include <TPad.h>
#include <TMath.h>
#include <TF1.h>

Double_t boltzmann(Double_t *x, Double_t *p);

const Double_t kT = 0.0257;    // this is for room temperature: 25 C == 298 K
const Double_t gC = 2.0/TMath::Sqrt(TMath::Pi());
const Double_t wE = 2.1;          // work energy: lowest value to pop electron
//const Double_t wE = 0.0;          // work energy: lowest value to pop electron

Double_t lower  = wE/kT;          // lower border of integration
Double_t upper  = 1000;           // upper value should be inifinity but no need
Double_t epsRel = 0.0001;         // relative precision of integration

Double_t nPars  = 1;              // number of parameters

void integrateBoltzmann()
{
  // Define the function using our new Boltzmann function
  TF1 *f = new TF1("bmann",boltzmann,0,lower*2,nPars);
  f->SetParameter(0,gC);
  
  // Draw it
  f->Draw();
  gPad->SetLogy();

  // Calculate the integral
  Double_t integral = f->Integral(lower,upper,epsRel);

  printf("\n Integral [%f,%f]: %e\n\n",lower,upper,integral);
}

Double_t boltzmann(Double_t *x, Double_t *p)
{
  // This is the normalized boltzmann distribution withinput parameter:
  //
  //      x = E/kT
  //
  // Integrating this function from 0 to infinity will be 1.

  Double_t value = p[0] * TMath::Sqrt(x[0]) * TMath::Exp(-1.0*x[0]);
  return value;
}
