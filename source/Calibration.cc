
#include <TF1.h>
#include <TH1D.h>
#include <TDatabasePDG.h>

#include <Calibration.h>
#include <CherenkovDetector.h>

// FIXME: yes, fix it please;
static unsigned hdim = 100;
static double hmin = -30.0, hmax = 30.0, hbin = (hmax - hmin)/hdim, hwnd = 15.0; 

#define _SPE_THETA_RESOLUTION_DEFAULT_ (0.0040)

// -------------------------------------------------------------------------------------

Calibration::Calibration():
  m_AutomaticCalibrationRequired(true),//false),
  m_CurrentEvent(0), 
  m_UseActsTracking(true),//false),
  m_DefaultSinglePhotonThetaResolution(_SPE_THETA_RESOLUTION_DEFAULT_)
{
  m_DatabasePDG = new TDatabasePDG();

  memset(m_CalibrationBinStat, 0x00, sizeof(m_CalibrationBinStat));
} // Calibration::Calibration()

// -------------------------------------------------------------------------------------

CherenkovEvent *Calibration::GetNextEvent(bool calibration)
{
  if (m_CurrentEvent < GetInputTreeEntryCount()) 
    return GetEvent(m_CurrentEvent++, calibration);
  else
    return 0;
} // Calibration::GetNextEvent()

// -------------------------------------------------------------------------------------
#if _OBSOLETE_
void Calibration::ImportTrackingSmearing(const char *ftheta, const char *fphi)
{
  std::map<std::pair<double, double>, std::pair<double, double>> *sarr[2] = 
    {&m_ThetaSmearing, &m_PhiSmearing};
  const char *farr[2] = {ftheta, fphi};
  //std::map<std::pair<double, double>, std::pair<double, double>> m_ThetaSmearing, m_PhiSmearing;

  for(unsigned iq=0; iq<2; iq++) {
    auto fin = fopen(farr[iq], "r");

    //float emin, emax, a, ea, b, eb;
    float emin, emax, a, b;//ea, b, eb;
    //unsigned lcounter = 0;
    char buffer[1024];
    while (fgets(buffer, 1024-1, fin)) {
      //-if (lcounter++ <= 1) continue;
      //printf("%s", buffer);

      //sscanf(buffer, "%f %f %f %f %f %f", &emin, &emax, &a, &ea, &b, &eb);
      sscanf(buffer, "%f %f %f %f", &emax, &emin, &a, &b);//ea, &b, &eb);
      //printf("%f %f %f %f\n", emin, emax, a, b);//ea, b, eb);

      (*sarr[iq])[std::make_pair(emin, emax)] = std::make_pair(a, b);
    } //while
  } //fr iq

  //exit(0);//
  //TVector3 p(0.0, 0.5, -5.0);
  //GetTrackingSmearing(p);
} // Calibration::ImportTrackingSmearing()

// -------------------------------------------------------------------------------------

std::pair<double, double> Calibration::GetTrackingSmearing(double momentum, double eta)
{
  double dtheta = 0.0, dphi = 0.0, theta = 2*atan(exp(-eta));

  // FIXME: very inefficient;
  for(auto &entry: m_ThetaSmearing)
    if (entry.first.first <= eta && eta <= entry.first.second) {
      double A = entry.second.first, B = entry.second.second; 
      dtheta = 1E-3*sqrt(pow(A/momentum, 2) + B*B);
      //printf("%f %f -> %f\n", entry.first.first, entry.first.second, dtheta);

      break;
    } //for entry .. if
  for(auto &entry: m_PhiSmearing)
    if (entry.first.first <= eta && eta <= entry.first.second) {
      double A = entry.second.first, B = entry.second.second; 
      dphi = 1E-3*sqrt(pow(A/momentum, 2) + B*B) * fabs(tan(theta));//momentum.Theta()));
      //printf("%f\n", fabs(dphi * tan(momentum.Theta())));

      break; 
    } //for entry .. if

  //printf("<p> = %8.3f [GeV/c], <eta> = %6.2f -> dtheta: %7.5f, dphi: %7.5f\n", 
  //	 momentum, eta, dtheta, dphi);  
  // Yes, make it simple for now;
  //return sqrt(dtheta*dtheta + dphi*dphi);
  return std::make_pair(dtheta, dphi);//*dtheta + dphi*dphi);
} // Calibration::GetTrackingSmearing()
#endif
// -------------------------------------------------------------------------------------

void Calibration::PerformCalibration(unsigned stat)
{
  unsigned nEvents = GetInputTreeEntryCount();
  unsigned evmax = stat ? (stat < nEvents ? stat : nEvents) : nEvents;

  // Reset the quantities for ALL radiators described in the detector geometry;
  for(auto rptr: GetMyRICH()->Radiators()) {
    auto radiator = rptr.second;

    radiator->SetGaussianSmearing(m_DefaultSinglePhotonThetaResolution);
    
    radiator->m_CalibrationPhotonCount = radiator->m_DetectedPhotonCount = 0;

    radiator->m_Calibrations.clear();
    for(unsigned iq=0; iq<_THETA_BIN_COUNT_; iq++) {
      radiator->m_Calibrations.push_back(CherenkovRadiatorCalibration());

      // FIXME: getting the same pointer in a rather awkward way;
      auto *calib = &radiator->m_Calibrations[iq];
      for(unsigned ir=0; ir<GetMyRICH()->Radiators().size(); ir++)
	calib->m_AverageRefractiveIndices.push_back(0.0);
    } //for iq
  } //for rptr

  memset(m_CalibrationBinStat, 0x00, sizeof(m_CalibrationBinStat));

  // Assume that calibration only needs to be performed in eta bins, and particle
  // eta at birth (at the IP) is representative enough (solenoidal field, radiators 
  // perpendicular to the beam axis); the rationale is: emitted 
  // (and detected!) Cherenkov photon spectrum shape to first order does not depend on 
  // either momentum or particle species, as long as we are far from the acceptance 
  // edge; FIXME: there the case is getting complicated, ignore for now;
  {
    double thmin = M_PI, thmax = 0.0;

    for(unsigned ev=0; ev<evmax; ev++) {
      GetInputTreeEntry(ev);
    
      for(auto mcparticle: Event()->ChargedParticles()) {
	double theta = mcparticle->GetVertexMomentum().Theta();

	if (theta < thmin) thmin = theta;
	if (theta > thmax) thmax = theta;

	// FIXME: cannot be out of range, right?;
	unsigned ibin = (unsigned)floor(theta / _THETA_BIN_WIDTH_);
	m_CalibrationBinStat[ibin]++;
      } //for mcparticle
    } //for ev
  }

  // First calculate average refractive index for all detected photons for every radiator;
  for(unsigned ev=0; ev<evmax; ev++) {
    GetInputTreeEntry(ev);
    
    for(auto mcparticle: Event()->ChargedParticles()) {
      for(auto rhptr: mcparticle->GetRadiatorHistory()) {
	auto radiator = mcparticle->GetRadiator(rhptr);
	auto history  = mcparticle->GetHistory (rhptr);
	
	// FIXME: this is not dramatically efficient;
	if (!GetMyRICH()->RadiatorRegistered(radiator)) continue;

	double theta = mcparticle->GetVertexMomentum().Theta();
	unsigned ibin = (unsigned)floor(theta / _THETA_BIN_WIDTH_);

	for(auto photon: history->Photons()) {
	  if (photon->IsUsefulForCalibration()) radiator->m_CalibrationPhotonCount++;
	  if (photon->WasDetected())            radiator->m_DetectedPhotonCount++;

	  // Choose only (unused) photons, which follow the same QE(lambda) distribution as the 
	  // detected ones, but did not make to be detected because of the efficiency cut;
	  if (!photon->IsUsefulForCalibration()) continue;

	  auto *calib = &radiator->m_Calibrations[ibin];
	  calib->m_Stat++;
	  //?calib->m_AverageRefractiveIndex += photon->GetVertexRefractiveIndex();
	  //for(auto [name,rad]: GetMyRICH()->Radiators())
	  {
	    auto vec = photon->StoredRefractiveIndices();
	    
	    for(unsigned ir=0; ir<vec.size(); ir++)
	      calib->m_AverageRefractiveIndices[ir] += vec[ir];
	  }
	  calib->m_AverageZvtx            += photon->GetVertexPosition().Z();
	} //for photon
      } //for radiator
    } //for mcparticle   
  } //for ev
  
  // Well, this loop is also across ALL radiators (not just selected ones?);
  for(auto rptr: GetMyRICH()->Radiators()) {
    auto radiator = rptr.second;

    //printf("--> %s -> %d %d\n", rptr.first.Data(), radiator->m_CalibrationPhotonCount, radiator->m_DetectedPhotonCount);
        
    radiator->m_DetectedToCalibrationPhotonRatio = radiator->m_CalibrationPhotonCount ?
      1.0*radiator->m_DetectedPhotonCount/radiator->m_CalibrationPhotonCount : 0.0;
    
    for(auto &calib: radiator->m_Calibrations)
      if (calib.m_Stat) {
	for(unsigned ir=0; ir<calib.m_AverageRefractiveIndices.size(); ir++)
	  calib.m_AverageRefractiveIndices[ir] /= calib.m_Stat;
	
	calib.m_AverageZvtx            /= calib.m_Stat;
      } //for calib .. if
  } //for rptr

  // Then run exactly the same event loop as for the "real" (detected) photons, but use the 
  // ones which did not make it through the QE selection during GEANT pass;
  if (AutomaticCalibrationRequired()) {
    // Define 1D histograms to store SPE theta residuals; 
    for(auto rptr: GetMyRICH()->Radiators()) {
      auto radiator = rptr.second;

      for(auto &calib: radiator->m_Calibrations)
	calib.m_hcalib  = new TH1D("",  "Cherenkov calibration angle (SPE)",hdim, hmin,   hmax);
    } //for rptr

    // Run full reconstruction chain on these events; 'true': calibration mode;
    for(unsigned ev=0; ev<evmax; ev++) 
      GetEvent(ev, true);
    
    for(auto rptr: GetMyRICH()->Radiators()) {
      auto radiator = rptr.second;

      if (radiator->m_YieldStat) {
	radiator->m_YieldCff /= radiator->m_YieldStat;
	//printf("%4d %f\n", radiator->m_YieldStat, radiator->m_YieldCff);
      } //if
    } //for radiator

    for(auto rptr: GetMyRICH()->Radiators()) {
      auto radiator = rptr.second;

      for(auto &calib: radiator->m_Calibrations) {
	auto *hcalib = calib.m_hcalib;
	//printf("Calibration: %4d entries\n", hcalib->GetEntries());
	if (!hcalib->GetEntries()) continue;

	int max = hcalib->GetMaximumBin();
	double mean = hmin + max*hbin;
	TF1 fq("fq", "gaus(0)", mean - hwnd/2, mean + hwnd/2);
	fq.SetParameters(1000, mean, 4.0);
	hcalib->Fit("fq");
	double par[3];
	fq.GetParameters(par);
	calib.m_Coffset = par[1] / 1000;
	calib.m_Csigma  = par[2] / 1000;
      } //for calib
    } //for radiator
    
    // Reset event counter;
    m_CurrentEvent = 0;
  } //if
} // Calibration::PerformCalibration()

// -------------------------------------------------------------------------------------

void Calibration::CalibratePhotonEmissionPoints( void )
{
  for(auto mcparticle: Event()->ChargedParticles()) {
    double theta = mcparticle->GetVertexMomentum().Theta();
    unsigned ibin = (unsigned)floor(theta / _THETA_BIN_WIDTH_);
    double p = mcparticle->GetVertexMomentum().Mag();
    double m = m_DatabasePDG->GetParticle(mcparticle->GetPDG())->Mass();
    double beta = 1./sqrt(1. + pow(m/p, 2));

    for(auto rhptr: mcparticle->GetRadiatorHistory()) {
      auto history  = mcparticle->GetHistory (rhptr);
      auto radiator = mcparticle->GetRadiator(rhptr);
      
      // FIXME: this is not dramatically efficient;
      if (!GetMyRICH()->RadiatorRegistered(radiator)) continue;

      double z0 = radiator->m_Calibrations[ibin].m_AverageZvtx;

      std::map<double, OpticalPhoton*> dpoints, tpoints;

      if (m_UseActsTracking && history->StepCount()) {
	for(unsigned iq=0; iq<history->StepCount(); iq++)
	  history->m_AverageParentMomentum += history->GetStep(iq)->GetMomentum();
	
	history->m_AverageParentMomentum *= 1./history->StepCount();
      } //if
      
      {
	unsigned stat = 0;

	for(auto photon: history->Photons()) {
	  // Choose only (unused) photons, which follow the same QE(lambda) distribution as the 
	  // detected ones, but did not make to be detected because of the efficiency cut;
	  if (!photon->IsUsefulForCalibration()) continue;

	  if (!m_UseActsTracking) {
	    stat++;
	    history->m_AverageParentMomentum += photon->GetVertexParentMomentum();
	  } //if
	    
	  // Ordered by distance from the most probable Z-vtx location;
	  dpoints[fabs(photon->GetVertexPosition().Z() - radiator->m_Calibrations[ibin].m_AverageZvtx)] = photon;
	  // Ordered along the trajectory;
	  tpoints[fabs(photon->GetVertexPosition().Z())]                                                = photon;
	} //for photon

	if (!m_UseActsTracking && stat) history->m_AverageParentMomentum *= 1./stat;
      } 	

      if (dpoints.size() >= 2) {
	auto *ph1 = (*dpoints.begin()).second, *ph2 = (*std::next(dpoints.begin())).second;
	
	double z1 = ph1->GetVertexPosition().Z(), z2 = ph2->GetVertexPosition().Z();
	double t1 = ph1->GetVertexTime(), t2 = ph2->GetVertexTime();
	double a = (t2 - t1) / (z2 - z1), t0 = t1 + a*(z0 - z1);
	history->m_EstimatedPath = beta*300*t0;
      } //if

      {
	unsigned qstat = 0;
	
	for(auto it = tpoints.begin(); it != tpoints.end(); it++) {
	  auto next = std::next(it);
	  
	  if (next != tpoints.end()) {
	    auto &v1 = it->second->GetVertexPosition(), &v2 = next->second->GetVertexPosition();
	    double z1 = v1.Z(), z2 = v2.Z();
	    auto  dv = (1/(z2 - z1)) * (v2 - v1);
	    
	    qstat++;
	    history->m_EstimatedVertex += v1 + (z0 - z1)*dv;
	  } //if
	} //for it

	if (qstat) history->m_EstimatedVertex *= 1./qstat;
      }
    } //for radiator
  } //for particle
} // Calibration::CalibratePhotonEmissionPoints()

// -------------------------------------------------------------------------------------

void Calibration::UpdateYields( void )
{
  for(auto mcparticle: Event()->ChargedParticles())
    for(auto rhptr: mcparticle->GetRadiatorHistory()) {
      auto radiator = mcparticle->GetRadiator(rhptr);
      
      if (!radiator->UsedInRingImaging()) continue;
      
      // FIXME: should one also account for 0 stat cases?;
      unsigned npe = mcparticle->GetRecoCherenkovPhotonCount (radiator);
      double mctheta = mcparticle->GetMocaCherenkovAverageTheta(radiator);
      
      radiator->m_YieldStat++;
      // No matter what the particle type it was, yield scales with npe ~ sin^2(theta);
      radiator->m_YieldCff += npe ? npe / pow(sin(mctheta), 2) : 0.0;
    } //for mcparticle 
} // Calibration:UpdateYields()

// -------------------------------------------------------------------------------------
