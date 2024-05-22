// Fast reconstruction for EIC Barrel DIRC
// original author: r.dzhygadlo at gsi.de

#ifndef DrcPidFast_h
#define DrcPidFast_h 1

#include <TFile.h>
#include <TH2F.h>
#include <TVector3.h>
#include <TRandom.h>
#include <TF1.h>

#include <iostream>

// probability - normalized to 1 probability for e,mu,pi,k,p
// sigma - deviation of the determined Cherenkov angle from expected in terms of Cherenkov track
// resolution cangle - Cherenkov angle cctr -  combined Cherenkov track resolution
struct DrcPidInfo {
  double probability[4];
  double sigma[4];
  double cangle;
  double cctr;

  double true_cangle;
};

class DrcPidFast {

 public:

  // barid = 0 for 17 mm thickness of the radiator
  // barid = 1 for 10 mm thickness of the radiator
  DrcPidFast(int barid /*= 0*/, const char *fmap);
  ~DrcPidFast() {}

  // read Cherenkov track resolution map from a file
  void ReadMap(TString name);

  // pdg - Particle Data Group code of the particle
  // mom - 3-momentum of the particle [GeV/c]
  // track_err - error assosiated with track direction [mrad]
  DrcPidInfo GetInfo(int pdg, TVector3 mom, double track_err = 0);

  // p - momentum of the particle [GeV/c]
  // theta - polar angle of the particle [deg]
  DrcPidInfo GetInfo(int pdg, double p, double theta, double track_err = 0);
  TH2F *GetTrrMap() { return fTrrMap; }

  int get_pid(int pdg);

 private:
  TH2F *fTrrMap;
  double fMass[4];
  TF1 *fMs_mom, *fMs_thickness;
  double fMs_thickness_max;
};

#endif
