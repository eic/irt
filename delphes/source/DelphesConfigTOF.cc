
#include <DelphesConfigTOF.h>

// -------------------------------------------------------------------------------------

int DelphesConfigTOF::DoSigmaCalculations( void )
{
  //
  // FIXME: sanity check on the input parameters is needed;
  //

  double etaStep = (m_EtaMax      - m_EtaMin     ) / m_EtaBinCount;
  double pStep   = (m_MomentumMax - m_MomentumMin) / m_MomentumBinCount;

  // Loop though all of the requested eta bins;
  for(unsigned ie=0; ie<m_EtaBinCount; ie++) {
    double eta = etaStep*(ie - (m_EtaBinCount-1)/2.);

    auto erange = AddEtaRange(eta - etaStep/2, eta + etaStep/2);

    // Loop though all of the requested momentum bins;
    for(unsigned ip=0; ip<m_MomentumBinCount; ip++) {
      double p = pStep*(ip - (m_MomentumBinCount-1)/2.);
    
      auto mrange = erange->GetMomentumRange(p - pStep/2, p + pStep/2);

      // Loop through all of the mass hypotheses and populate entries one by one;
      for(unsigned ih=0; ih<m_MassHypotheses.size(); ih++) {
	auto hypo = m_MassHypotheses[ih];
	//mass[ih] = m_MassHypotheses[ih]->Mass();
      
	bool ret = StoreSigmaEntry(mrange, hypo->PdgCode(), 0.030);
	if (!ret) {
	  printf("Failed to insert a sigma entry\n");
	  exit(0);
	} //if
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
