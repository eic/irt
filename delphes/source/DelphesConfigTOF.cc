
#include <DelphesConfigTOF.h>

// Speed of light; [mm/ps];
#define _SoL_      (  0.29979)

// -------------------------------------------------------------------------------------

double DelphesConfigTOF::tof(double m, double p, double l)
{
  double e = sqrt(p*p + m*m), beta = p/e;//, c = 299.79, t = l / (beta*_SoL_);

  return l / (beta*_SoL_);
} // DelphesConfigTOF::tof()

// -------------------------------------------------------------------------------------

int DelphesConfigTOF::DoSigmaCalculations( void )
{
  //
  // FIXME: sanity check on the input parameters is needed;
  //
  assert(m_PtMode);

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
      double pt = m_MomentumMin + pStep*(ip +0.5), p = pt / sin(theta);
    
      // Assign momentum range; avoid machine accuracy issues when checking continuity;
      auto mrange = erange->GetMomentumRange(ip ? erange->LastMomentumRange()->GetMax() : 
					     pt - pStep/2, pt + pStep/2);
      // mnemonic rule: 30 MeV/c in 1kGs field -> curvature radius of 1m; convert to [mm];
      double r = 1000*(0.1/m_MagneticField)*(pt/0.03);
      // Particle will not reach the barrel layer; FIXME: may want to give few cm up?; 
      if (2*r < m_InstallationRadius) continue;

      // Loop through all of the mass hypotheses and populate entries one by one;
      for(unsigned ih=0; ih<m_MassHypotheses.size(); ih++) {
	auto hypo = m_MassHypotheses[ih];
	double m = m_MassHypotheses[ih]->Mass();

	{
	  double alfa = 2*asin(m_InstallationRadius/(2.0*r));
	  double lxy =  r*alfa, l = lxy/sin(theta);
	  // Use speed of light in [mm/ps] units;
	  double e = sqrt(p*p + m*m), beta = p/e;//, c = 0.29979;
	      
	  double sp = 0.01, t = tof(m, p, l);
	  double p1 = p - 0.1*sp, p2 = p + 0.1*p, dt = tof(m, p2, l) - tof(m, p1, l); 

	  printf("%3d %3d: eta= %5.2f th=%5.1f pt= %5.2f, r= %7.2f, m= %5.3f alfa=%5.1f, beta= %5.3f, l = %5.1f t = %7.4f\n", 
		 ie, ip, eta, theta*180/M_PI, pt, r, m, alfa*180/M_PI, beta, l, t);

	  // Now calculate the uncertainty;
	  double s1 = m_T0Resolution, s2 = m_DetectorResolution, s3 = m_PathLengthResolution / _SoL_, s4 = sp*fabs(dt)/(0.2*sp);
	  printf("   -> %7.2f %7.2f %7.2f %7.2f\n", s1, s2, s3, s4);

	  bool ret = StoreSigmaEntry(mrange, hypo->PdgCode(), sqrt(s1*s1+s2*s2+s3*s3+s4*s4));
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

  return 0;
} // DelphesConfigTOF::Calculate()

// -------------------------------------------------------------------------------------
