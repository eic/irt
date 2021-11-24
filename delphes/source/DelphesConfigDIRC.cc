
#include <DelphesConfigDIRC.h>
#include <DrcPidFast.h>

// -------------------------------------------------------------------------------------

int DelphesConfigDIRC::DoSigmaCalculations( void )
{
  unsigned hdim = m_MassHypotheses.size();
  //
  // FIXME: sanity check on the input parameters is needed;
  // FIXME: a lot of overlap with the TOF codes;
  //
  assert(m_PtMode);

  double etaStep = (m_EtaMax      - m_EtaMin     ) / m_EtaBinCount;
  double pStep   = (m_MomentumMax - m_MomentumMin) / m_MomentumBinCount;

  // 17mm bar per default;
  auto dpf = new DrcPidFast(0, m_ParameterizationMap.c_str());

  // Loop though all of the requested eta bins;
  for(unsigned ie=0; ie<m_EtaBinCount; ie++) {
    double eta = m_EtaMin + etaStep*ie;
    double theta = 2*std::atan(exp(-eta));

    // Assign eta range; avoid machine accuracy issues when checking continuity;
    auto erange = AddEtaRange(ie ? m_EtaRanges.back()->GetMax() : eta,  eta + etaStep);

    // Loop though all of the requested momentum bins;
    for(unsigned ip=0; ip<m_MomentumBinCount; ip++) {
      // So momentum has a meaning of Pt in this code; see assert() above;
      double pt = m_MomentumMin + pStep*ip, p = pt / sin(theta);
    
      // Assign momentum range; avoid machine accuracy issues when checking continuity;
      auto mrange = erange->GetMomentumRange(ip ? erange->LastMomentumRange()->GetMax() : 
					     pt, pt + pStep);
      // mnemonic rule: 30 MeV/c in 1kGs field -> curvature radius of 1m; convert to [mm];
      double r = 1000*(0.1/m_MagneticField)*(pt/0.03);

      // Particle will not reach the barrel layer; FIXME: may want to give few cm up?; 
      if (2*r < m_InstallationRadius) continue;

      // Looks like one should take them in quadrature;
      double sa = m_TrackerAngularResolutionA / p, sb = m_TrackerAngularResolutionB;
      double sigma = sqrt(sa*sa + sb*sb);

      for(unsigned ih=0; ih<hdim; ih++) {
	auto hypo = m_MassHypotheses[ih];
	
	auto info = dpf->GetInfo(abs(hypo->PdgCode()), p, theta*180/M_PI, sigma);
	
	// Just use sigma and central values;
	double mean  = info.true_cangle;
	double sigma = info.cctr;
		
	bool ret = StoreSigmaEntry(mrange, hypo->PdgCode(), mean, sigma);
      } //for ih
    } //for ip
  } //for ie

  // FIXME: what a crap; soft this out: who is calling what, etc;
  return Check(false);
} // DelphesConfigDIRC::DoSigmaCalculations()

// -------------------------------------------------------------------------------------

int DelphesConfigDIRC::Calculate( void )
{
  unsigned hdim = m_MassHypotheses.size();
   
  for(auto erange: m_EtaRanges) {
    for(auto mrange: erange->m_MomentumRanges) {
      // Matrix will be band-diagonal of course;
      mrange->m_Matrix = new double[hdim*hdim];

      double value[hdim], sigma[hdim];//, t0[hdim-1];
      for(unsigned ih=0; ih<hdim; ih++) {
	value[ih] = mrange->GetMeasurement(ih);
	sigma[ih] = mrange->GetSigma      (ih);
      } //

      double tmid[hdim-1];
      // Go through all pairs and calculate middle points; FIXME: will this work if 
      // there was only one mass hypothesis?;
      for(unsigned ih=0; ih<hdim-1; ih++) {
	double t1 = value[ih], t2 = value[ih+1], s1 = sigma[ih], s2 = sigma[ih+1];
	
	// FIXME: in this code all sigma values should be valid;
	tmid[ih] = s1 && s2 ? (t1*s2*s2 + t2*s1*s1) / (s1*s1 + s2*s2) : 0.0;
      } //for
	  
      // And now loop through all mass nodes again and calculate erf() tails which 
      // should be subtracted from the default w=1, either on one or both sides;
      for(unsigned ih=0; ih<hdim; ih++) {
	double l = 0.0, r = 0.0;
	
	for(unsigned ip=0; ip<2; ip++) {
	  int id = ih + ip;
	  if (!id || id >= hdim) continue;
	  
	  // Otherwise perhaps an unphysical value; FIXME: can it be in this code?;
	  if (tmid[id-1]) {
	    double diff = fabs(value[ih] - tmid[id-1]);
	    (ip ? r : l) = (1.0 - erf(diff/(sqrt(2.)*sigma[ih])))/2;
	  } //if
	} //for ip
	
	if (ih   >   0) 
	  mrange->m_Matrix[ih*hdim+ih-1] = l;
	mrange  ->m_Matrix[ih*hdim+ih  ] = 1.0 - l - r;
	if (ih+1 < hdim) 
	  mrange->m_Matrix[ih*hdim+ih+1] = r;
      } //for ih
    } //for mrange
  } //for erange

  return 0;
} // DelphesConfigDIRC::Calculate()

// -------------------------------------------------------------------------------------
