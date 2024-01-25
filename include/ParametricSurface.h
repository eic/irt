
#include <math.h>

#include <TObject.h>
#include <TVector3.h>

#ifndef _IRT_PARAMETRIC_SURFACE_
#define _IRT_PARAMETRIC_SURFACE_

class ParametricSurface: public TObject {
 public:
 ParametricSurface(): m_Umin(0.0), m_Umax(0.0), m_Vmin(0.0), m_Vmax(0.0) {};
 ParametricSurface(const TVector3 &x0, double umin, double umax, double vmin, double vmax):
  m_Center(x0), m_Umin(umin), m_Umax(umax), m_Vmin(vmin), m_Vmax(vmax) {};
  ~ParametricSurface() {};

  // 2D parameter ranges (call them U&V);
  double Umin( void ) const { return m_Umin; };
  double Umax( void ) const { return m_Umax; };
  double Vmin( void ) const { return m_Vmin; };
  double Vmax( void ) const { return m_Vmax; };
  bool IsInside(double u, double v) const {
    return (u >= m_Umin && u <= m_Umax && v >= m_Vmin && v <= m_Vmax);
  };
  void SetUVranges(double umin, double umax, double vmin, double vmax) {
    m_Umin = umin; m_Umax = umax; m_Vmin = vmin; m_Vmax = vmax;
  };

  virtual double GetU(const TVector3 &xx) const = 0;
  virtual double GetV(const TVector3 &xx) const = 0;
  virtual bool IsInside(const TVector3 &xx) const {
    return IsInside(GetU(xx), GetV(xx));
  };

  virtual TVector3 GetSpacePoint(double u, double v) const = 0;
  // There is no check that the point actually belongs to the surface;
  // it is assumed that GEANT stepping was done correctly, so the point 
  // is more or less close to the 2D surface; for flat and spherical 
  // surfaces this is a mute point anyway since this function is implemented 
  // in a trivial way;
  virtual TVector3 GetNormal(const TVector3 &xx) const = 0;
  // Provide a reasonable wrapper; of course simple surfaces 
  // can to their own calculations;
  virtual TVector3 GetNormal(double u, double v) const {
    auto pt = GetSpacePoint(u, v);
    return GetNormal(pt);
  };
  // default, not overriden by all derived classes
  virtual TVector3 GetNormal( void ) const {
    return TVector3(0.0, 0.0, 0.0);
  };
  const TVector3 &GetCenter( void ) const { return m_Center; };
  virtual double GetDistance(const TVector3 &xx) const = 0;

  // Crossing with the straight line defined by {x0,n0}; 
  virtual bool GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs, 
			   bool check_normal = true) const = 0;
  bool GetQuadraticEquationCaseCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs, 
					bool check_normal, double a, double b, double c) const;

  // Introduce only the transformations needed for the current task;
  virtual ParametricSurface *_Clone(double angle, const TVector3 &axis) const = 0;
  void Shift(const TVector3 &shift) {
    m_Center += shift;
  };
  
 protected:
  // It looks like surfaces I need typically have m_Center defined;
  TVector3 m_Center;

 private:
  double m_Umin, m_Umax, m_Vmin, m_Vmax;

#ifndef DISABLE_ROOT_IO
  ClassDef(ParametricSurface, 1);
#endif
};

class LocalCoordinatesXY: public TObject {
 public:
  LocalCoordinatesXY() {};
  ~LocalCoordinatesXY() {};

  virtual double GetLocalX(const TVector3 &xx) const = 0;
  virtual double GetLocalY(const TVector3 &xx) const = 0;

#ifndef DISABLE_ROOT_IO
  ClassDef(LocalCoordinatesXY, 1);
#endif
}; 

#endif
