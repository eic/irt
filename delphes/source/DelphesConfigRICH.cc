

#include <DelphesConfigRICH.h>

// -------------------------------------------------------------------------------------

int DelphesConfigRICH::Calculate( void )
{
  unsigned dim = m_MassHypotheses.size();
  // First assign mass nodes;
  double mass[dim];
  for(unsigned ih=0; ih<dim; ih++)
    mass[ih] = m_MassHypotheses[ih]->Mass();
    
  for(auto erange: m_EtaRanges) {
    for(auto mrange: erange->m_MomentumRanges) {
      // Matrix will be band-diagonal of course;
      mrange->m_Matrix = new double[dim*dim];
      double sigma[dim], t0[dim-1], theta[dim], pp = (mrange->GetMin() + mrange->GetMax())/2;

      // Assign sigma values;
      for(unsigned ih=0; ih<dim; ih++) {
	double m = mass[ih];
	sigma[ih] = sqrt(pow(mrange->GetSigma(ih), 2) + pow(m_AdditionalSmearing, 2));

	double argument = sqrt(pp*pp + m*m)/(m_Rindex*pp);
	// Convert to [mrad];
	theta[ih] = 1000*(fabs(argument) <= 1.0 ? acos(argument) : 0.0);
	if (!theta[ih]) sigma[ih] = 0.0;
      } //for ih

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
    } //for mrange
  } //for erange

  return 0;
} // DelphesConfigRICH::Calculate()

// -------------------------------------------------------------------------------------
