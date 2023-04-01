
#include <vector>
#include <math.h>

#include <TObject.h>

#ifndef _SINGLE_PDF_
#define _SINGLE_PDF_

class SinglePDF: public TObject {
 public:
  SinglePDF() {};
  ~SinglePDF() {};

  virtual double GetValue   (double x)                  const = 0;
  virtual double GetAverage(void)                       const = 0;
  virtual bool   WithinRange(double x, double dx = 0.0) const = 0;

#ifndef DISABLE_ROOT_IO
  ClassDef(SinglePDF, 1);
#endif
};

class UniformPDF: public SinglePDF {
 public:
 UniformPDF(double x0 = 0.0, double x1 = 0.0, double weight = 1.0, double sigma = 0.0):
  m_X0(x0), m_X1(x1), m_Weight(weight), m_Sigma(sigma) {
    if (m_X0 > m_X1) std::swap(m_X0, m_X1);

    // FIXME: sanity check;
    m_Norm = 1.0/(m_X1 - m_X0);
  };
  ~UniformPDF() {};

  inline bool WithinRange(double x, double dx = 0.0) const {
    if (!dx) return (x >= m_X0 && x <= m_X1);

    double xl = x - dx, xr = x + dx;
    if (xl > m_X1 || xr < m_X0) return false;

    return true;
  };
  inline double GetValue(double x) const {
    return WithinRange(x) ? m_Norm*m_Weight : 0.0;
  };
  double GetAverage(void) const {
    return (m_X0 + m_X1)/2;
  };
  double GetWeight(void) const { return m_Weight; };

  inline double GetRangeIntegral(double x0, double x1) const {
    if (x0 > x1) std::swap(x0, x1);
    
    if (x0 > m_X1) return 0.0;
    if (x1 < m_X0) return 0.0;

    //printf("%f %f\n", m_Norm, m_Weight);
    double l = (x0 > m_X0 ? x0 : m_X0), r = (x1 < m_X1 ? x1 : m_X1);

    return m_Norm*m_Weight*(r - l);
  };
  inline double GetGaussianIntegral(double mu, double sigma) const {
    // FIXME: this is indeed not clean, but kind of makes sense;
    if (m_Sigma) sigma = sqrt(sigma*sigma + m_Sigma*m_Sigma);

    // FIXME: need to think about normalization;
    double q0 = (1 - erf((m_X0 - mu)/(sqrt(2.)*sigma)))/2;
    double q1 = (1 - erf((m_X1 - mu)/(sqrt(2.)*sigma)))/2;
    
    return m_Norm*m_Weight*(q0 - q1);
  };

 private:
  double m_X0, m_X1, m_Weight, m_Norm, m_Sigma;

#ifndef DISABLE_ROOT_IO
  ClassDef(UniformPDF, 1);
#endif
};

class VectorPDF: public TObject {
 public:
  VectorPDF() {};
  ~VectorPDF() {};
  
  void AddMember(UniformPDF *pdf) { m_Members.push_back(pdf); };
  unsigned GetWithinRangeCount(double x, double dx = 0.0) const {
    unsigned ret = 0;

    for(auto member: m_Members)
      if (member->WithinRange(x, dx))
	ret++;

    return ret;
  };
  double GetValue(double x) const {
    double ret = 0.0;

    for(auto member: m_Members)
      ret += member->GetValue(x);

    return ret;
  };
  double GetAverage( void ) const {
    double avg = 0.0, wtsum = 0.0;

    for(auto member: m_Members) {
      avg   += member->GetWeight() * member->GetAverage();
      wtsum += member->GetWeight();
    } //for member

    return (wtsum ? avg / wtsum : 0.0);
  };
  inline double GetRangeIntegral(double x0, double x1) const {
    double ret = 0.0;

    for(auto member: m_Members)
      ret += member->GetRangeIntegral(x0, x1);

    return ret;
  };
  inline double GetGaussianIntegral(double mu, double sigma) const {
    double ret = 0.0;

    for(auto member: m_Members)
      ret += member->GetGaussianIntegral(mu, sigma);

    return ret;
  };

 private:
  std::vector<UniformPDF*> m_Members;

#ifndef DISABLE_ROOT_IO
  ClassDef(VectorPDF, 1);
#endif
};


#endif
