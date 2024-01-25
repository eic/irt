
#include "DelphesConfigTOF.h"

// Speed of light; [mm/ps];
#define _SoL_      (  0.29979)

// -------------------------------------------------------------------------------------

double DelphesConfigTOF::tof(double m, double p, double l)
{
  double e = sqrt(p*p + m*m), beta = p/e;

  return l / (beta*_SoL_);
} // DelphesConfigTOF::tof()

// -------------------------------------------------------------------------------------

int DelphesConfigTOF::DoSigmaCalculations( void )
{
  //
  // FIXME: sanity check on the input parameters is needed;
  //
  //+assert(m_PtMode);
  assert(!m_PtMode);

  double etaStep = (m_EtaMax      - m_EtaMin     ) / m_EtaBinCount;
  double pStep   = (m_MomentumMax - m_MomentumMin) / m_MomentumBinCount;

  // Loop though all of the requested eta bins;
  for(unsigned ie=0; ie<m_EtaBinCount; ie++) {
    double eta = m_EtaMin + etaStep*(ie + 0.5);
    double theta = 2*std::atan(exp(-eta));

    // Assign eta range; avoid machine accuracy issues when checking continuity;
    auto erange = AddEtaRange(ie ? m_EtaRanges.back()->GetMax() : eta - etaStep/2,  eta + etaStep/2);

    // Loop though all of the requested momentum bins;
    for(unsigned ip=0; ip<m_MomentumBinCount; ip++) {
      // So momentum has a meaning of Pt in this code; see assert() above;
      //+double pt = m_MomentumMin + pStep*(ip +0.5), p = pt / sin(theta);
      double p = m_MomentumMin + pStep*(ip +0.5), pt = p * sin(theta);
    
      // Assign momentum range; avoid machine accuracy issues when checking continuity;
      auto mrange = erange->GetMomentumRange(ip ? erange->LastMomentumRange()->GetMax() : 
					     //+pt - pStep/2, pt + pStep/2);
					     p - pStep/2, p + pStep/2);

      // mnemonic rule: 30 MeV/c in 1kGs field -> curvature radius of 1m; convert to [mm];
      if (m_InstallationRadius) {
	double r = 1000*(0.1/m_MagneticField)*(pt/0.03);

	// Particle will not reach the barrel layer; FIXME: may want to give few cm up?; 
	if (2*r < m_InstallationRadius) continue;
      } //if

      // Loop through all of the mass hypotheses and populate entries one by one;
      for(unsigned ih=0; ih<m_MassHypotheses.size(); ih++) {
	auto hypo = m_MassHypotheses[ih];
	double m = m_MassHypotheses[ih]->Mass();

	{
	  double e = sqrt(p*p + m*m), beta = p/e, l = 0.0;
	  if (m_InstallationRadius) {
	    // FIXME: duplicate line;
	    double r = 1000*(0.1/m_MagneticField)*(pt/0.03);
	    double alfa = 2*asin(m_InstallationRadius/(2.0*r));
	    double lxy =  r*alfa;

	    l = lxy/sin(theta);
	  } else {
	    // FIXME: calculate helix length correctly; suffices for now;
	    l = m_InstallationDistance / cos(theta);
	  } //if
	      
	  double sp = ((0.01*m_MomentumResolutionA)*p + (0.01*m_MomentumResolutionB))*p, t = tof(m, p, l);
	  // A lousy way to calculate dt/dp differential;
	  double p1 = p - 0.1*sp, p2 = p + 0.1*sp, dt = tof(m, p2, l) - tof(m, p1, l); 
	  //printf("%f -> %f %f -> %f %f\n", 100*sp/p, p1, p2, tof(m, p1, l), tof(m, p2, l));

	  // Now calculate the uncertainties; (0.2*sp) comes from the differential calculation;
	  double s1 = m_T0Resolution, s2 = m_DetectorResolution, s3 = m_PathLengthResolution / _SoL_;
	  double s4 = sp ? sp*fabs(dt)/(0.2*sp) : 0.0, s = sqrt(s1*s1+s2*s2+s3*s3+s4*s4);

	  //printf("%3d %3d: eta= %5.2f th=%5.1f pt= %5.2f p= %5.2f, r= %7.2f, m= %6.4f alfa=%5.1f, "
	  //	 "beta= %5.3f, l = %5.1f t = %7.4f  s=%7.4f\n", 
	  //	 ie, ip, eta, theta*180/M_PI, pt, p, r, m, alfa*180/M_PI, beta, l, t, s);
	  //printf("   -> %7.2f %7.2f %7.2f %7.2f\n", s1, s2, s3, s4);

	  bool ret = StoreSigmaEntry(mrange, hypo->PdgCode(), t, s);
	  if (!ret) {
	    printf("Failed to insert a sigma entry\n");
	    exit(0);
	  } //if
	}
      } //for ih
    } //for ip
  } //for ie

  return 0;
} // DelphesConfigTOF::DoSigmaCalculations()

// -------------------------------------------------------------------------------------

int DelphesConfigTOF::Calculate( void )
{
  unsigned hdim = m_MassHypotheses.size();
  // First assign mass nodes;
  //double mass[hdim];
  //for(unsigned ih=0; ih<hdim; ih++)
  //mass[ih] = m_MassHypotheses[ih]->Mass();
   
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
} // DelphesConfigTOF::Calculate()

// -------------------------------------------------------------------------------------
