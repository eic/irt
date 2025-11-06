

#include "DelphesConfigRICH.h"

// -------------------------------------------------------------------------------------

void DelphesConfigRICH::ImportTrackingSmearing(const char *ftheta, const char *fphi)
{
  std::map<std::pair<double, double>, std::pair<double, double>> *sarr[2] = 
    {&m_ThetaSmearing, &m_PhiSmearing};
  const char *farr[2] = {ftheta, fphi};
  //std::map<std::pair<double, double>, std::pair<double, double>> m_ThetaSmearing, m_PhiSmearing;

  for(unsigned iq=0; iq<2; iq++) {
    auto fin = fopen(farr[iq], "r");

    float emin, emax, a, ea, b, eb;
    unsigned lcounter = 0;
    char buffer[1024];
    while (fgets(buffer, 1024-1, fin)) {
      if (lcounter++ <= 1) continue;
      //printf("%s", buffer);

      sscanf(buffer, "%f %f %f %f %f %f", &emin, &emax, &a, &ea, &b, &eb);
      //printf("%f %f %f %f %f %f\n", emin, emax, a, ea, b, eb);

      (*sarr[iq])[std::make_pair(emin, emax)] = std::make_pair(a, b);
    } //while
  } //fr iq

  TVector3 p(0.0, 0.5, -5.0);
  GetTrackingSmearing(p);
} // DelphesConfigRICH::ImportTrackingSmearing()

// -------------------------------------------------------------------------------------

double DelphesConfigRICH::GetTrackingSmearing(double momentum, double eta)
{
  double dtheta = 0.0, dphi = 0.0, theta = 2*atan(exp(-eta));

  // FIXME: very inefficient;
  for(auto &entry: m_ThetaSmearing)
    if (entry.first.first <= eta && eta <= entry.first.second) {
      double A = entry.second.first, B = entry.second.second; 
      dtheta = sqrt(pow(A/momentum, 2) + B*B);
      //printf("%f\n", dtheta);

      break;
    } //for entry .. if
  for(auto &entry: m_PhiSmearing)
    if (entry.first.first <= eta && eta <= entry.first.second) {
      double A = entry.second.first, B = entry.second.second; 
      dphi = sqrt(pow(A/momentum, 2) + B*B) * fabs(tan(theta));//momentum.Theta()));
      //printf("%f\n", fabs(dphi * tan(momentum.Theta())));

      break; 
    } //for entry .. if

  printf("<p> = %8.3f [GeV/c], <eta> = %6.2f -> dtheta: %7.4f, dphi: %7.4f\n", 
	 momentum, eta, dtheta, dphi);  
  // Yes, make it simple for now;
  return sqrt(dtheta*dtheta + dphi*dphi);
} // DelphesConfigRICH::GetTrackingSmearing()

// -------------------------------------------------------------------------------------

static double below_kaon_threshold[3][3] = {
  {1.0, 0.0, 0.0},
  {0.0, 0.5, 0.5},
  {0.0, 0.5, 0.5}};

int DelphesConfigRICH::Calculate( void )
{
  unsigned dim = m_MassHypotheses.size();
  // First assign mass nodes;
  double mass[dim];
  for(unsigned ih=0; ih<dim; ih++)
    mass[ih] = m_MassHypotheses[ih]->Mass();

  for(auto erange: m_EtaRanges) {
    double eta = (erange->GetMin() + erange->GetMax())/2;

    for(auto mrange: erange->m_MomentumRanges) {
      mrange->m_MatrixDim = dim*dim;
      // Matrix will be band-diagonal of course;
      mrange->m_Matrix = new double[mrange->m_MatrixDim];//dim*dim];

      if (dim == 3 && mrange->GetMax() <= m_MassHypotheses[0]->GetThreshold()) 
	// Entry below pion threshold -> should not happen; reset to 0;
	for(unsigned ip=0; ip<3; ip++) 
	  for(unsigned iq=0; iq<3; iq++) 
	    mrange->m_Matrix[ip*3+iq] = 0.0;
      else if (dim == 3 && mrange->GetMax() <= m_MassHypotheses[1]->GetThreshold()) 
	// Entry below kaon threshold -> predefined matrix;
	for(unsigned ip=0; ip<3; ip++) 
	  for(unsigned iq=0; iq<3; iq++) 
	    mrange->m_Matrix[ip*3+iq] = below_kaon_threshold[ip][iq];
      else {
	double sigma[dim], t0[dim-1], theta[dim], pp = (mrange->GetMin() + mrange->GetMax())/2;

	// Assign sigma values;
	for(unsigned ih=0; ih<dim; ih++) {
	  double m = mass[ih];
	  //sigma[ih] = sqrt(pow(mrange->GetSigma(ih), 2) + pow(m_AdditionalSmearing, 2));
	  sigma[ih] = sqrt(pow(mrange->GetSigma(ih), 2) + pow(GetTrackingSmearing(pp, eta), 2));
	  
	  double argument = sqrt(pp*pp + m*m)/(m_Rindex*pp);
	  // Convert to [mrad];
	  theta[ih] = 1000*(fabs(argument) <= 1.0 ? acos(argument) : 0.0);
	  if (!theta[ih]) sigma[ih] = 0.0;
	} //for ih
	
	if (m_EfficiencyContaminationMode) {
	  // In this case provide efficiency for the diagonal terms and contamination 
	  // for the off-diagonal ones;
	  
	} else {
	  // Go through all pairs and calculate middle points; FIXME: will this work if 
	  // there was only one mass hypothesis?;
	  for(unsigned ih=0; ih<dim-1; ih++) {
	    double t1 = theta[ih], t2 = theta[ih+1], s1 = sigma[ih], s2 = sigma[ih+1];
	    
	    t0[ih] = s1 && s2 ? (t1*s2*s2 + t2*s1*s1) / (s1*s1 + s2*s2) : 0.0;
	  } //for
	  
	  // And now loop through all mass nodes again and calculate erf() tails which 
	  // should be subtracted from the default w=1, either on one or both sides;
	  for(unsigned ih=0; ih<dim; ih++) {
	    double l = 0.0, r = 0.0;
	    
	    for(unsigned ip=0; ip<2; ip++) {
	      int id = ih + ip;
	      if (!id || id >= dim) continue;
	      
	      // Otherwise perhaps an unphysical value;
	      if (t0[id-1]) {
		double diff = fabs(theta[ih] - t0[id-1]);
		(ip ? r : l) = (1.0 - erf(diff/(sqrt(2.)*sigma[ih])))/2;
	      } //if
	    } //for ip
	    
	    if (ih   >   0) 
	      mrange->m_Matrix[ih*dim+ih-1] = l;
	    mrange  ->m_Matrix[ih*dim+ih  ] = 1.0 - l - r;
	    if (ih+1 < dim) 
	      mrange->m_Matrix[ih*dim+ih+1] = r;
	  } //for ih
	} //if
      } //if

      // Final shot; FIXME: simplify all this logic;
      if (dim == 3 && mrange->GetMax() > m_MassHypotheses[2]->GetThreshold() && 
	  mrange->GetMax() <= m_MassHypotheses[2]->GetThreshold()) {
	mrange->m_Matrix[1*dim+1] = mrange->m_Matrix[2*dim+1] = mrange->m_Matrix[1*dim+2] = 0.0;
	mrange->m_Matrix[2*dim+2] = 1.0;
      } //if
    } //for mrange
  } //for erange

  return 0;
} // DelphesConfigRICH::Calculate()

// -------------------------------------------------------------------------------------

double DelphesConfigRICH::GenerateMeasurement(int pdg, const TVector3 &momentum)
{
  auto hypo = GetMassHypothesis(pdg);
  if (!hypo) return 0.0;
  unsigned ih = GetHypoIndex(hypo);

  double m = hypo->Mass(), pp = momentum.Mag(), eta = momentum.Eta();

  auto mrange = GetEtaMomentumRange(eta, pp);
  if (!mrange) return 0;

  double argument = sqrt(pp*pp + m*m)/(m_Rindex*pp);
  // Convert to [mrad];
  double thnom = 1000*(fabs(argument) <= 1.0 ? acos(argument) : 0.0);
  //double sigma = sqrt(pow(mrange->GetSigma(ih), 2) + pow(m_AdditionalSmearing, 2));
  double sigma = sqrt(pow(mrange->GetSigma(ih), 2) + pow(GetTrackingSmearing(pp, eta), 2));
  
  return m_rndm.Gaus(thnom, sigma);
} // DelphesConfigRICH::GenerateMeasurement()

// -------------------------------------------------------------------------------------

//
// FIXME: duplicate code;
//

MassHypothesis *DelphesConfigRICH::FindBestHypothesis(const TVector3 &momentum, double theta)
{
  MassHypothesis *ret = 0;

  for(unsigned ih=0; ih<m_MassHypotheses.size(); ih++) {
    auto hypo = m_MassHypotheses[ih];

    double m = hypo->Mass(), pp = momentum.Mag(), eta = momentum.Eta();

    auto mrange = GetEtaMomentumRange(eta, pp);
    if (!mrange) continue;

    double argument = sqrt(pp*pp + m*m)/(m_Rindex*pp);
    // Convert to [mrad];
    double thnom = 1000*(fabs(argument) <= 1.0 ? acos(argument) : 0.0);
    if (!thnom) continue;
    //double sigma = sqrt(pow(mrange->GetSigma(ih), 2) + pow(m_AdditionalSmearing, 2));
    double sigma = sqrt(pow(mrange->GetSigma(ih), 2) + pow(GetTrackingSmearing(pp, eta), 2));

    hypo->m_Diff      = (theta - thnom)/sigma;
    hypo->m_ChiSquare = pow(hypo->m_Diff, 2);

    if (!ret || hypo->m_ChiSquare < ret->m_ChiSquare) ret = hypo;
  } //for ih

  return ret;
} // DelphesConfigRICH::FindBestHypothesis()

// -------------------------------------------------------------------------------------
