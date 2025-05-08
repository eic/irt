
#include <TH1D.h>

#include <CherenkovDetector.h>
#include <Configuration.h>

#define _SPE_THETA_RESOLUTION_DEFAULT_ (0.0040)

// -------------------------------------------------------------------------------------

Configuration::Configuration():
  m_VerboseMode(true), 
  m_DefaultSinglePhotonThetaResolution(_SPE_THETA_RESOLUTION_DEFAULT_)
{
  m_hthph   = new TH1D("thph",   "Cherenkov angle (SPE)",             100,   -30,     30);
  m_hccdfph = new TH1D("ccdfph", "Single photon chi^2 CCDF",           50,     0,      1);
  m_hccdftr = new TH1D("ccdftr", "Track-level chi^2 CCDF",             50,     0,      1);
  m_hccdfev = new TH1D("ccdfev", "Event-level chi^2 CCDF",             50,     0,      1);
  m_hnpetr  = new TH1D("npetr",  "Photon count per track",             50,     0,     50);
  m_hdtph   = new TH1D("dtph",   "Cherenkov timing (SPE)",            100,  -500,    500);
  m_hmatch  = new TH1D("match",  "PID evaluation correctness",          3,     0,      3);
  m_hwl     = new TH1D("wl",     "Wave Length",                       200, 200.0, 1000.0);
  m_hvtx    = new TH1D("z0",     "True emission point",                80,   -40,     40);
  //+m_hri     = new TH1D("ri",     "Refractive Index - 1.0",            500, 0.035,  0.055);
  m_hri     = new TH1D("ri",     "Refractive Index - 1.0",            500, 0.010,  0.030);
} // Configuration::Configuration() 

// -------------------------------------------------------------------------------------

CherenkovRadiator *Configuration::UseRadiator(const char *rname) {//, double smearing) {
  auto radiator = GetMyRICH()->GetRadiator(rname);
  if (radiator) {
    m_SelectedRadiators.insert(radiator);
    radiator->SetGaussianSmearing(m_DefaultSinglePhotonThetaResolution);//smearing);
    
    return radiator;
  } //if
  
  return 0;
} // Configuration::UseRadiator()

// -------------------------------------------------------------------------------------
