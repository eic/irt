
#include <TH1D.h>
#include <TCanvas.h>
#include <TString.h>

#include "CherenkovRadiator.h"

// -------------------------------------------------------------------------------------

CherenkovRadiatorPlots::CherenkovRadiatorPlots(const char *tag):
  m_Tag(tag),
  m_hvtx(0),
  m_hri(0),
  m_hthtr(0)
{
  m_hnpe    = new TH1D((TString("npe")    + tag).Data(), "Detected MC photons per track",      60,   0.0,   60.0);
  m_hwl     = new TH1D((TString("wl")     + tag).Data(), "Wave Length",                       200, 200.0, 1000.0);
  m_hwl->GetXaxis()->SetTitle("[nm]");
  
  m_hnhits  = new TH1D((TString("nhits")  + tag).Data(), "Used digitized hits per track",      60,   0.0,   60.0);
  m_hthph   = new TH1D((TString("thph")   + tag).Data(), "Cherenkov angle offset (photons)",  100, -20.0,   20.0);
  m_hthph->GetXaxis()->SetTitle("[mrad]");
  m_hccdfph = new TH1D((TString("ccdfph") + tag).Data(), "Single photon chi^2 CCDF",           50,   0.0,    1.0);
  m_hccdfph->SetMinimum(0);
} // CherenkovRadiatorPlots::CherenkovRadiatorPlots()

// -------------------------------------------------------------------------------------

void CherenkovRadiatorPlots::SetRefractiveIndexRange(double min, double max)
{
  if (m_hri) {
    delete m_hri;
    m_hri = 0;
  } //if

  m_hri = new TH1D((TString("ri") + m_Tag).Data(), "Refractive Index - 1.0", 500, min - 1.0, max - 10.);
  m_hri->GetXaxis()->SetNdivisions(408);
} // CherenkovRadiatorPlots::SetRefractiveIndexRange()

// -------------------------------------------------------------------------------------

void CherenkovRadiatorPlots::SetPhotonVertexRange(double min, double max)
{
  if (m_hvtx) {
    delete m_hvtx;
    m_hvtx = 0;
  } //if

  m_hvtx = new TH1D((TString("vtx") + m_Tag).Data(), "Emission point", 100, min, max);
  m_hvtx->GetXaxis()->SetTitle("[mm]");
  m_hvtx->GetXaxis()->SetNdivisions(408);
} // CherenkovRadiatorPlots::SetRefractiveIndexRange()

// -------------------------------------------------------------------------------------

void CherenkovRadiatorPlots::SetCherenkovAngleRange(double min, double max)
{
  if (m_hthtr) {
    delete m_hthtr;
    m_hthtr = 0;
  } //if

  m_hthtr   = new TH1D((TString("thtr")  + m_Tag).Data(),  "Cherenkov angle (tracks)", 100, min, max);
  m_hthtr->GetXaxis()->SetTitle("[mrad]");
} // CherenkovRadiatorPlots::SetCherenkovAngleRange()

// -------------------------------------------------------------------------------------

TCanvas *CherenkovRadiator::DisplayStandardPlots(const char *cname, const char *wname, int wtopx,
						 unsigned wtopy, unsigned wx, unsigned wy) const
{
  if (!Plots()) return 0;

  auto cv = new TCanvas(cname, wname, wtopx, wtopy, wx, wy);
  cv->Divide(4, 2);
  
  cv->cd(1); if (Plots()->hvtx()) Plots()->hvtx()   ->Draw();
  cv->cd(2);                      Plots()->hnpe()   ->Draw();
  cv->cd(3);                      Plots()->hwl()    ->Draw();
  cv->cd(4); if (Plots()->hri())  Plots()->hri()    ->Draw();
  cv->cd(5);                      Plots()->hthph()  ->Fit("gaus");
  cv->cd(6);                      Plots()->hnhits() ->Draw();
  cv->cd(7);                      Plots()->hccdfph()->Draw();
  cv->cd(8);                      Plots()->hthtr()  ->Fit("gaus");

  return cv;
} // CherenkovRadiator::DisplayStandardPlots()

// -------------------------------------------------------------------------------------
