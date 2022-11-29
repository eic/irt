
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class SinglePDF+;
#pragma link C++ class UniformPDF+;
#pragma link C++ class VectorPDF+;

#pragma link C++ class TransientParticle+;
#pragma link C++ class ChargedParticle+;
#pragma link C++ class ChargedParticleStep+;
#pragma link C++ class OpticalPhoton+;

#pragma link C++ class ReflectionPoint+;
#pragma link C++ class RefractionPoint+;

#pragma link C++ class OpticalBoundary+;
#pragma link C++ class IRT+;

#pragma link C++ class CherenkovRadiator+;
#pragma link C++ class CherenkovDetector+;
#pragma link C++ class CherenkovDetectorCollection+;
#pragma link C++ class CherenkovEvent+;
#pragma link C++ class BitMask+;

#pragma link C++ class RadiatorHistory+;

#pragma link C++ class ParametricSurface+;
#pragma link C++ class SphericalSurface+;
#pragma link C++ class CylindricalSurface+;
#pragma link C++ class ConicalSurface+;
#pragma link C++ class LocalCoordinatesXY+;
#pragma link C++ class FlatSurface+;

#pragma link C++ class G4Object+;
#pragma link C++ class G4ObjectCopy+;

#pragma link C++ class SurfaceCopy+;
#pragma link C++ class CherenkovMirror+;
//#pragma link C++ class CherenkovMirrorGroup+;
#pragma link C++ class FlatMirror+;
#pragma link C++ class ConicalMirror+;
#pragma link C++ class CylindricalMirror+;
#pragma link C++ class SphericalMirror+;

#pragma link C++ class CherenkovPhotonDetector+;

// Are these two really needed?;
#pragma link C++ class std::pair<TRef, TRef>+;
#pragma link C++ class std::vector<TRef>+;

#endif
