
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class IRT2::SinglePDF+;
#pragma link C++ class IRT2::UniformPDF+;
#pragma link C++ class IRT2::VectorPDF+;

#pragma link C++ class IRT2::TransientParticle+;
#pragma link C++ class IRT2::ChargedParticle+;
#pragma link C++ class IRT2::ChargedParticleStep+;
#pragma link C++ class IRT2::OpticalPhoton+;

#pragma link C++ class IRT2::ReflectionPoint+;
#pragma link C++ class IRT2::RefractionPoint+;

#pragma link C++ class IRT2::OpticalBoundary+;
#pragma link C++ class IRT2::IRT+;

#pragma link C++ class IRT2::CherenkovRadiator+;
#pragma link C++ class IRT2::CherenkovRadiatorCalibration+;
#pragma link C++ class IRT2::CherenkovDetector+;
#pragma link C++ class IRT2::CherenkovDetectorCollection+;
#pragma link C++ class IRT2::CherenkovEvent+;
#pragma link C++ class IRT2::BitMask+;

#pragma link C++ class IRT2::RadiatorHistory+;

#pragma link C++ class IRT2::ParametricSurface+;
#pragma link C++ class IRT2::SphericalSurface+;
#pragma link C++ class IRT2::ToricSurface+;
#pragma link C++ class IRT2::CylindricalSurface+;
#pragma link C++ class IRT2::ConicalSurface+;
#pragma link C++ class IRT2::LocalCoordinatesXY+;
#pragma link C++ class IRT2::FlatSurface+;

#pragma link C++ class IRT2::G4Object+;
#pragma link C++ class IRT2::G4ObjectCopy+;

#pragma link C++ class IRT2::SurfaceCopy+;
#pragma link C++ class IRT2::CherenkovMirror+;
//#pragma link C++ class CherenkovMirrorGroup+;
#pragma link C++ class IRT2::FlatMirror+;
#pragma link C++ class IRT2::ConicalMirror+;
#pragma link C++ class IRT2::CylindricalMirror+;
#pragma link C++ class IRT2::SphericalMirror+;

#pragma link C++ class IRT2::CherenkovPhotonDetector+;

// Are these two really needed?;
#pragma link C++ class std::pair<TRef, TRef>+;
#pragma link C++ class std::vector<TRef>+;

#pragma link C++ class IRT2::ReconstructionFactory+;

#endif
