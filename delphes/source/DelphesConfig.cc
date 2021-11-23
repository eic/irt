
#include <TDatabasePDG.h>

#include <DelphesConfig.h>

#define _ERROR_(message) { printf("\n\n  %s\n", message); exit(1); }

// -------------------------------------------------------------------------------------

DelphesConfig::DelphesConfig(const char *dname): 
  m_Name(dname),  
  m_EtaMin(0.0), m_EtaMax(0.0),
  m_MomentumMin(0.0), m_MomentumMax(0.0), m_EfficiencyContaminationMode(false),
  //m_PionThreshold(0.0), m_KaonThreshold(0.0), m_ProtonThreshold(0.0)
  m_PtMode(false)
{
  m_DatabasePDG = new TDatabasePDG();
} // DelphesConfig::DelphesConfig()

// -------------------------------------------------------------------------------------

MassHypothesis *DelphesConfig::AddMassHypothesisCore(TParticlePDG *pdg, 
						     double max_contamination_left, 
						     double max_contamination_right) 
{
  // Apply certain sanity checks;
  if (!pdg) return 0;
  auto hypothesis = new MassHypothesis(pdg, max_contamination_left, max_contamination_right);
  if (!hypothesis) return 0;

  // Sanity check;
  MassHypothesis *prev = m_MassHypotheses.size() ? m_MassHypotheses.back() : 0;
  if (prev && pdg->Mass() <= prev->Mass()) {
    printf("Particle masses are not in ascending order (%7.4f is followed by %7.4f)\n", 
	   prev->Mass(), pdg->Mass());
    exit(1);
  } //if
  m_MassHypotheses.push_back(hypothesis);
  //printf("-> %f\n", pdg->Mass());

  if (max_contamination_left != 1.0 || max_contamination_right != 1.0)
    m_EfficiencyContaminationMode = true;

  return hypothesis;
} // DelphesConfig::AddMassHypothesisCore()

// -------------------------------------------------------------------------------------

MassHypothesis *DelphesConfig::AddMassHypothesis(int pdg, 
						 double max_contamination_left, 
						 double max_contamination_right) 
{
  return AddMassHypothesisCore(m_DatabasePDG->GetParticle(pdg), 
			       max_contamination_left, max_contamination_right);
} // DelphesConfig::AddMassHypothesis()

// -------------------------------------------------------------------------------------

MassHypothesis *DelphesConfig::AddMassHypothesis(const char *pname, 
						 double max_contamination_left, 
						 double max_contamination_right) 
{
  return AddMassHypothesisCore(m_DatabasePDG->GetParticle(pname), 
			       max_contamination_left, max_contamination_right);
} // DelphesConfig::AddMassHypothesis()

// -------------------------------------------------------------------------------------

MomentumRange *EtaRange::GetMomentumRange(double min, double max)
{
  for(auto mrange: m_MomentumRanges) 
    if (mrange->GetMin() == min && mrange->GetMax() == max)
      return mrange;

  auto mrange = new MomentumRange(min, max);
  m_MomentumRanges.push_back(mrange);
  
  return mrange;
} // EtaRange::GetMomentumRange()

// -------------------------------------------------------------------------------------

bool DelphesConfig::StoreSigmaEntry(MomentumRange *mrange, int pdg, double measurement, 
				    double sigma)
{
  // Check that at least 'pdg' code is in order;
  unsigned mdim = mrange->GetSigmaCount();

  // This can not be for sure;
  if (mdim == m_MassHypotheses.size()) return false;

  if (pdg != m_MassHypotheses[mdim]->PdgCode()) return false;

  mrange->AddSigmaValue(measurement, sigma);

  return true;
} // DelphesConfig::StoreSigmaEntry()

// -------------------------------------------------------------------------------------

bool DelphesConfig::StoreSigmaEntry(MomentumRange *mrange, int pdg, double sigma)
{
  return StoreSigmaEntry(mrange, pdg, 0.0, sigma);
} // DelphesConfig::StoreSigmaEntry()

// -------------------------------------------------------------------------------------

void DelphesConfig::AddZeroSigmaEntries( void )
{
  for(auto erange: m_EtaRanges) 
    for(auto mrange: erange->m_MomentumRanges) {
      unsigned mdim = mrange->GetSigmaCount();

      for(unsigned iq=mdim; iq<m_MassHypotheses.size(); iq++)
	StoreSigmaEntry(mrange, m_MassHypotheses[iq]->PdgCode(), 0.0);
    } //for erange..mrange

  DetermineThresholds();
} // DelphesConfig::AddZeroSigmaEntries()

// -------------------------------------------------------------------------------------

void DelphesConfig::DetermineThresholds( void )
{
  for(unsigned ih=0; ih<m_MassHypotheses.size(); ih++) {
    auto hypo = m_MassHypotheses[ih];

    // Find the smallest non-zero entry;
    for(auto erange: m_EtaRanges) 
      for(auto mrange: erange->m_MomentumRanges) {
	double min = mrange->GetMin();
	// Called from DelphesConfig::AddZeroSigmaEntries() internally -> all 
	// sigma entries for all hypotheses must be present;
	if (mrange->GetSigma(ih) && (!hypo->GetThreshold() || hypo->GetThreshold() > min))
	  hypo->SetThreshold(min);
      } //for erange..mrange

    printf("@T@ %d -> %7.2f\n", hypo->GetThreshold());
  } //for iq
} // DelphesConfig::DetermineThresholds()

// -------------------------------------------------------------------------------------

void DelphesConfig::Print( void )
{
  for(auto erange: m_EtaRanges) {
    printf("@D@ eta %4.2f .. %4.2f\n", erange->GetMin(), erange->GetMax());
 
    for(auto mrange: erange->m_MomentumRanges) {
      printf("@D@       momentum %5.2f .. %5.2f -> sigma:", 
	     mrange->GetMin(), mrange->GetMax());
      
      for(unsigned iq=0; iq<mrange->GetSigmaCount(); iq++)
	printf("      %5.2f (%4d)", mrange->GetSigma(iq), m_MassHypotheses[iq]->PdgCode());
      printf("\n");
    } //for mrange
  } //for erange
} // DelphesConfig::AddZeroSigmaEntries()

// -------------------------------------------------------------------------------------

EtaRange *DelphesConfig::AddEtaRange(double min, double max) 
{
  // First try to find already existing one;
  for(auto erange: m_EtaRanges) 
    if (erange->GetMin() == min && erange->GetMax() == max)
      return erange;

  auto erange = new EtaRange(min, max);
  auto prev = m_EtaRanges.size() ? m_EtaRanges.back() : 0;
  if (prev && min != prev->GetMax()){
    printf("Eta ranges sequence issue: %6.2f is followed by %6.2f\n", 
	   prev->GetMax(), min);
    return 0;
  } //for if
  m_EtaRanges.push_back(erange);
  
  return erange;
} // DelphesConfig::AddEtaRange()

// -------------------------------------------------------------------------------------

int DelphesConfig::Check( void )
{
  if (m_EtaRanges.empty()) _ERROR_("No eta ranges defined!");

  // Define eta(min) and eta(max);
  m_EtaMin = m_EtaRanges.front()->GetMin();
  m_EtaMax = m_EtaRanges.back ()->GetMax();

  // Define momentum ranges; for now there is no reason to assume they can be different 
  // for different eta; generalize later if really needed;
  for(auto erange: m_EtaRanges) {
    if (!erange->GetMomentumRangeCount()) _ERROR_("Empty eta range defined!");

    // First eta range: assign momentum range; 
    if (erange == m_EtaRanges.front()) {
      m_MomentumMin = erange->FirstMomentumRange()->GetMin();
      m_MomentumMax = erange->LastMomentumRange ()->GetMax();
      
      continue;
    } //if
    
    // Subsequent eta ranges: check that momentum range is the same;
    if (erange->FirstMomentumRange()->GetMin() != m_MomentumMin || 
	erange->LastMomentumRange() ->GetMax() != m_MomentumMax) 
      _ERROR_("Full momentum range should be the same in all eta ranges!");

    for(auto mrange: erange->m_MomentumRanges) 
      if (mrange->GetSigmaCount() != m_MassHypotheses.size())
	_ERROR_("Momentum range entry has wrong number of sigma values!"); 
  } //for erange
  
  return 0;
} // DelphesConfig::Check()

// -------------------------------------------------------------------------------------

void DelphesConfig::WriteMassHypothesis(FILE *fout, unsigned ih)
{
  unsigned dim = m_MassHypotheses.size();
  const char *pstr = m_PtMode ? "pt" : "pt * cosh(eta)";

  auto prev = ih                              ?     m_MassHypotheses[ih-1] : 0;
  auto hypo =                                       m_MassHypotheses[ih  ];
  auto next = ih == m_MassHypotheses.size()-1 ? 0 : m_MassHypotheses[ih+1];
  MassHypothesis *arr[3] = {prev, hypo, next};

  for(unsigned ip=0; ip<3; ip++) {
    auto hto = arr[ip];

    if (hto) {
      fprintf(fout, "\n");
      fprintf(fout, "    add EfficiencyFormula {%d} {%d} {\n", hypo->PdgCode(), 
	      hto->PdgCode());
      
      // Now the tricky part comes; calculate the actual entries; start with the out-of-area cut;
      fprintf(fout, "      (eta<%6.2f || eta>=%6.2f || %s < %7.2f || %s >= %7.2f) * ( 0.00 ) +\n", 
	      m_EtaMin, m_EtaMax, pstr, m_MomentumMin, pstr, m_MomentumMax);
      
      // Then add eta and momentum range entries;
      for(auto erange: m_EtaRanges) {
	double emin = erange->GetMin(), emax = erange->GetMax();
	
	for(auto mrange: erange->m_MomentumRanges) {
	  fprintf(fout, "      (%6.2f <= eta && eta < %6.2f) * (%7.2f <= %s && %s < %7.2f) * (%8.6f)%s\n",
		  emin, emax, mrange->GetMin(), pstr, pstr, mrange->GetMax(), 
		  mrange->m_Matrix[ih*dim + ih + ip - 1],
		  erange == m_EtaRanges.back() && mrange == erange->m_MomentumRanges.back() ? "" : " +"); 
	  
	} //for mrange
      } //for erange
      
      fprintf(fout, "    }\n");
    } //if
  } //for ip
} // DelphesConfig::WriteMassHypothesis()

// -------------------------------------------------------------------------------------

void DelphesConfig::Write( void )
{
  // First check that the configuration is self-consistent;
  if (Check() || Calculate()) return;

  auto fout = fopen((m_Name + ".tcl").c_str(), "w");
  if (!fout) {
    printf("Failed to open output file!\n");
    exit(1);
  } //if

  // Write header first; do not care much about cleanness;
  fprintf(fout, "module IdentificationMap %s {\n", m_Name.c_str());
  fprintf(fout, "  set InputArray TrackMerger/tracks\n");
  fprintf(fout, "  set OutputArray tracks\n");
  //fprintf(fout, "\n");
  
  // Essential part; loop through all mass hypotheses; the first and the 
  // last one will have two entries, the one(s) in the middle - three; 
  for(unsigned ih = 0; ih<m_MassHypotheses.size(); ih++) 
    WriteMassHypothesis(fout, ih);

  // Write trailer;
  fprintf(fout, "\n");
  fprintf(fout, "  add EfficiencyFormula {0} {0} { 0.00 }\n");
  fprintf(fout, "}\n");
  
  fclose(fout);
} // DelphesConfig::Write()

// -------------------------------------------------------------------------------------

