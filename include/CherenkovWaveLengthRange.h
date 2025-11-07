
#ifndef _CHERENKOV_WAVELENGTH_RANGE_
#define _CHERENKOV_WAVELENGTH_RANGE_

namespace IRT2 {

class CherenkovWaveLengthRange {
public:
 CherenkovWaveLengthRange(unsigned wldim, double numin, double nustep): 
  m_WLDim(wldim) {
    m_PhotonEnergies = new double[wldim];

    for(unsigned iq=0; iq<wldim; iq++)
      m_PhotonEnergies[iq] = numin + iq*nustep;
  };
  virtual ~CherenkovWaveLengthRange() {};

  unsigned GetWLDim( void ) const { return m_WLDim; };
  double GetPhotonEnergy(unsigned bin) const { 
    return bin < m_WLDim ? m_PhotonEnergies[bin] : 0.0;
  };
  double *GetPhotonEnergies( void ) const { return m_PhotonEnergies; };

private:
  unsigned m_WLDim;
  double *m_PhotonEnergies;
};

} // namespace IRT2

#endif
