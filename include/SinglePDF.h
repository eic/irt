
#include <vector>
#include <math.h>

#include <TObject.h>

#ifndef _SINGLE_PDF_
#define _SINGLE_PDF_

class SinglePDF: public TObject {
 public:
  SinglePDF() {};
  ~SinglePDF() {};

  virtual double GetValue   (double x) const = 0;
  virtual bool   WithinRange(double x) const = 0;

  ClassDef(SinglePDF, 1);
};

class UniformPDF: public SinglePDF {
 public:
 UniformPDF(double x0 = 0.0, double x1 = 0.0, double weight = 1.0, double sigma = 0.0):
  m_X0(x0), m_X1(x1), m_Weight(weight), m_Sigma(sigma) {
    if (m_X0 > m_X1) std::swap(m_X0, m_X1);

    // FIXME: sanity check;
    m_Norm = 1.0/(x1 - x0);
  };
  ~UniformPDF() {};

  inline bool WithinRange(double x) const {
    return (x >= m_X0 && x <= m_X1);
  };
  inline double GetValue(double x) const {
    return WithinRange(x) ? m_Norm*m_Weight : 0.0;
  };
  inline double GetRangeIntegral(double x0, double x1) const {
    if (x0 > x1) std::swap(x0, x1);
    
    if (x0 > m_X1) return 0.0;
    if (x1 < m_X0) return 0.0;

    double l = (x0 > m_X0 ? x0 : m_X0), r = (x1 < m_X1 ? x1 : m_X1);

    return m_Norm*m_Weight*(r - l);
  };
  inline double GetGaussianIntegral(double mu, double sigma) const {
    // FIXME: this is indeed not clean, but kind of make sense;
    if (m_Sigma) sigma = sqrt(sigma*sigma + m_Sigma*m_Sigma);

    double q0 = (1 - erf((m_X0 - mu)/(sqrt(2.)*sigma)))/2;
    double q1 = (1 - erf((m_X1 - mu)/(sqrt(2.)*sigma)))/2;
    
    return m_Norm*m_Weight*(q0 - q1);
  };

 private:
  double m_X0, m_X1, m_Weight, m_Norm, m_Sigma;

  ClassDef(UniformPDF, 1);
};

class VectorPDF: public TObject {
 public:
  VectorPDF() {};
  ~VectorPDF() {};
  
  void AddMember(UniformPDF *pdf) { m_Members.push_back(pdf); };
  unsigned GetWithinRangeCount(double x) const {
    unsigned ret = 0;

    for(auto member: m_Members)
      if (member->WithinRange(x))
	ret++;

    return ret;
  };
  double GetValue(double x) const {
    double ret = 0.0;

    for(auto member: m_Members)
      ret += member->GetValue(x);

    return ret;
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

  ClassDef(VectorPDF, 1);
};


#endif
