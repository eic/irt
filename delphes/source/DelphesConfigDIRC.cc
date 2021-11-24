
#include <DelphesConfigDIRC.h>
#include <DrcPidFast.h>

// -------------------------------------------------------------------------------------

int DelphesConfigDIRC::Calculate( void )
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
  for(unsigned ie=0; ie<m_EtaBinCount+1; ie++) {
    double eta = m_EtaMin + etaStep*ie;// + 0.5);
    double theta = 2*std::atan(exp(-eta));

    // Assign eta range; avoid machine accuracy issues when checking continuity;
    auto erange = AddEtaRange(ie ? m_EtaRanges.back()->GetMax() : eta - etaStep/2,  eta + etaStep/2);

    // Loop though all of the requested momentum bins;
    for(unsigned ip=0; ip<m_MomentumBinCount+1; ip++) {
      // So momentum has a meaning of Pt in this code; see assert() above;
      double pt = m_MomentumMin + pStep*ip, p = pt / sin(theta);
    
      // Assign momentum range; avoid machine accuracy issues when checking continuity;
      auto mrange = erange->GetMomentumRange(ip ? erange->LastMomentumRange()->GetMax() : 
					     pt - pStep/2, pt + pStep/2);
      // mnemonic rule: 30 MeV/c in 1kGs field -> curvature radius of 1m; convert to [mm];
      double r = 1000*(0.1/m_MagneticField)*(pt/0.03);

      // Particle will not reach the barrel layer; FIXME: may want to give few cm up?; 
      if (2*r < m_InstallationRadius) continue;

      // Matrix will be band-diagonal of course;
      mrange->m_Matrix = new double[hdim*hdim];

      // Looks like one should take them in quadrature;
      double sa = m_TrackerAngularResolutionA / p, sb = m_TrackerAngularResolutionB;
      double sigma = sqrt(sa*sa + sb*sb);

      for(unsigned ih=0; ih<hdim; ih++) {
	auto hypo = m_MassHypotheses[ih];
	
	auto info = dpf->GetInfo(abs(hypo->PdgCode()), p, theta, sigma);

	// FIXME: well, here I'm assigning the full NxN matrix, but only the 
	// neighboring terms will be taken into account in the Write() call;
	double probSum = 0.0, prob[hdim];
	for(unsigned iq=0; iq<hdim; iq++) {
	  auto other = m_MassHypotheses[iq];

	  unsigned id = dpf->get_pid(other->PdgCode());
	  double wt = prob[iq] = info.probability[id];
	  probSum += wt;
	} //for iq

	for(unsigned iq=0; iq<hdim; iq++) 
	  mrange->m_Matrix[ih*hdim+iq] = prob[hdim] / probSum;
      } //for ih
    } //for ip
  } //for ie

  return 0;
} // DelphesConfigDIRC::Calculate()

// -------------------------------------------------------------------------------------
