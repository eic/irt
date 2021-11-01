//==========================================================================
//  dRICh: Dual Ring Imaging Cherenkov Detector
//--------------------------------------------------------------------------
//
// Author: Christopher Dilks (Duke University)
//
// - Design Adapted from Standalone Fun4all and GEMC implementations
//   [ Evaristo Cisbani, Cristiano Fanelli, Alessio Del Dotto, et al. ]
//
//==========================================================================

#include <TFile.h>

#include "DDRec/DetectorData.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"

using namespace dd4hep;
using namespace dd4hep::rec;

#include <ParametricSurface.h>
#include <CherenkovRadiator.h>
#include <OpticalBoundary.h>
#include <CherenkovDetectorCollection.h>
#include <CherenkovPhotonDetector.h>

// create the detector
static Ref_t createDetector(Detector& desc, xml::Handle_t handle, SensitiveDetector sens) {
  xml::DetElement detElem = handle;
  std::string detName = detElem.nameStr();
  int detID = detElem.id();

  DetElement det(detName, detID);
  xml::Component dims = detElem.dimensions();
  OpticalSurfaceManager surfMgr = desc.surfaceManager();

#if _TODAY_
  //@@@ Create output file and a geometry object pointer;
  auto fout = new TFile("drich-config.root", "RECREATE");
  auto geometry = new CherenkovDetectorCollection();
  // Yes, a single detector in this environment;
  geometry->AddNewDetector();
  auto detector = geometry->GetDetector(0);

  // attributes -----------------------------------------------------------
  // - vessel
  double  vesselZmin       =  dims.attr<double>(_Unicode(zmin));
  double  vesselLength     =  dims.attr<double>(_Unicode(length));
  double  vesselRmin0      =  dims.attr<double>(_Unicode(rmin0));
  double  vesselRmin1      =  dims.attr<double>(_Unicode(rmin1));
  double  vesselRmax0      =  dims.attr<double>(_Unicode(rmax0));
  double  vesselRmax1      =  dims.attr<double>(_Unicode(rmax1));
  double  vesselRmax2      =  dims.attr<double>(_Unicode(rmax2));
  double  snoutLength      =  dims.attr<double>(_Unicode(snout_length));
  int     nSectors         =  dims.attr<int>(_Unicode(nsectors));
  double  wallThickness    =  dims.attr<double>(_Unicode(wall_thickness));
  double  windowThickness  =  dims.attr<double>(_Unicode(window_thickness));
  auto    vesselMat        =  desc.material(detElem.attr<std::string>(_Unicode(material)));
  auto    gasvolMat        =  desc.material(detElem.attr<std::string>(_Unicode(gas)));
  auto    vesselVis        =  desc.visAttributes(detElem.attr<std::string>(_Unicode(vis_vessel)));
  auto    gasvolVis        =  desc.visAttributes(detElem.attr<std::string>(_Unicode(vis_gas)));
  // - radiator (applies to aerogel and filter)
  auto    radiatorElem        =  detElem.child(_Unicode(radiator));
  double  radiatorRmin        =  radiatorElem.attr<double>(_Unicode(rmin));
  double  radiatorRmax        =  radiatorElem.attr<double>(_Unicode(rmax));
  double  radiatorPhiw        =  radiatorElem.attr<double>(_Unicode(phiw));
  double  radiatorPitch       =  radiatorElem.attr<double>(_Unicode(pitch));
  double  radiatorFrontplane  =  radiatorElem.attr<double>(_Unicode(frontplane));
  // - aerogel
  auto    aerogelElem       =  radiatorElem.child(_Unicode(aerogel));
  auto    aerogelMat        =  desc.material(aerogelElem.attr<std::string>(_Unicode(material)));
  auto    aerogelVis        =  desc.visAttributes(aerogelElem.attr<std::string>(_Unicode(vis)));
  double  aerogelThickness  =  aerogelElem.attr<double>(_Unicode(thickness));
  // - filter
  auto    filterElem       =  radiatorElem.child(_Unicode(filter));
  auto    filterMat        =  desc.material(filterElem.attr<std::string>(_Unicode(material)));
  auto    filterVis        =  desc.visAttributes(filterElem.attr<std::string>(_Unicode(vis)));
  double  filterThickness  =  filterElem.attr<double>(_Unicode(thickness));
  // - mirror
  auto    mirrorElem       =  detElem.child(_Unicode(mirror));
  auto    mirrorMat        =  desc.material(mirrorElem.attr<std::string>(_Unicode(material)));
  auto    mirrorVis        =  desc.visAttributes(mirrorElem.attr<std::string>(_Unicode(vis)));
  auto    mirrorSurf       =  surfMgr.opticalSurface(mirrorElem.attr<std::string>(_Unicode(surface)));
  double  mirrorBackplane  =  mirrorElem.attr<double>(_Unicode(backplane));
  double  mirrorThickness  =  mirrorElem.attr<double>(_Unicode(thickness));
  double  mirrorRmin       =  mirrorElem.attr<double>(_Unicode(rmin));
  double  mirrorRmax       =  mirrorElem.attr<double>(_Unicode(rmax));
  double  mirrorPhiw       =  mirrorElem.attr<double>(_Unicode(phiw));
  double  focusTuneZ       =  mirrorElem.attr<double>(_Unicode(focus_tune_z));
  double  focusTuneX       =  mirrorElem.attr<double>(_Unicode(focus_tune_x));
  // - sensor module
  auto    sensorElem       =  detElem.child(_Unicode(sensors)).child(_Unicode(module));
  auto    sensorMat        =  desc.material(sensorElem.attr<std::string>(_Unicode(material)));
  auto    sensorVis        =  desc.visAttributes(sensorElem.attr<std::string>(_Unicode(vis)));
  auto    sensorSurf       =  surfMgr.opticalSurface(sensorElem.attr<std::string>(_Unicode(surface)));
  double  sensorSide       =  sensorElem.attr<double>(_Unicode(side));
  double  sensorGap        =  sensorElem.attr<double>(_Unicode(gap));
  double  sensorThickness  =  sensorElem.attr<double>(_Unicode(thickness));
  // - sensor sphere
  auto    sensorSphElem     =  detElem.child(_Unicode(sensors)).child(_Unicode(sphere));
  double  sensorSphRadius   =  sensorSphElem.attr<double>(_Unicode(radius));
  double  sensorSphCenterX  =  sensorSphElem.attr<double>(_Unicode(centerx));
  double  sensorSphCenterZ  =  sensorSphElem.attr<double>(_Unicode(centerz));
  // - sensor sphere patch cuts
  auto    sensorSphPatchElem  =  detElem.child(_Unicode(sensors)).child(_Unicode(sphericalpatch));
  double  sensorSphPatchPhiw  =  sensorSphPatchElem.attr<double>(_Unicode(phiw));
  double  sensorSphPatchRmin  =  sensorSphPatchElem.attr<double>(_Unicode(rmin));
  double  sensorSphPatchRmax  =  sensorSphPatchElem.attr<double>(_Unicode(rmax));
  double  sensorSphPatchZmin  =  sensorSphPatchElem.attr<double>(_Unicode(zmin));
  // - debugging switches
  int   debug_optics_mode  =  detElem.attr<int>(_Unicode(debug_optics));
  bool  debug_mirror       =  mirrorElem.attr<bool>(_Unicode(debug));
  bool  debug_sensors      =  sensorSphElem.attr<bool>(_Unicode(debug));

  // if debugging optics, override some settings
  bool debug_optics = debug_optics_mode > 0;
  if(debug_optics) {
    printout(WARNING,"DRich_geo","DEBUGGING DRICH OPTICS");
    switch(debug_optics_mode) {
      case 1: vesselMat = aerogelMat = filterMat = sensorMat = gasvolMat = desc.material("VacuumOptical"); break;
      case 2: vesselMat = aerogelMat = filterMat = sensorMat = desc.material("VacuumOptical"); break;
      default: printout(FATAL,"DRich_geo","UNKNOWN debug_optics_mode"); return det;
    };
    aerogelVis = sensorVis = mirrorVis;
    gasvolVis = vesselVis = desc.invisible();
  };


  // BUILD VESSEL ====================================================================
  /* - `vessel`: aluminum enclosure, the mother volume of the dRICh
   * - `gasvol`: gas volume, which fills `vessel`; all other volumes defined below
   *   are children of `gasvol`
   * - the dRICh vessel geometry has two regions: the snout refers to the conic region
   *   in the front, housing the aerogel, while the tank refers to the cylindrical
   *   region, housing the rest of the detector components
   */

  // derived attributes
  double tankLength = vesselLength - snoutLength;
  double vesselZmax = vesselZmin + vesselLength;

  // snout solids
  double boreDelta = vesselRmin1 - vesselRmin0;
  Cone vesselSnout(
      snoutLength/2.0,
      vesselRmin0,
      vesselRmax0,
      vesselRmin0 + boreDelta * snoutLength / vesselLength,
      vesselRmax1
      );
  Cone gasvolSnout(
      /* note: `gasvolSnout` extends a bit into the tank, so it touches `gasvolTank`
       * - the extension distance is equal to the tank `windowThickness`, so the
       *   length of `gasvolSnout` == length of `vesselSnout`
       * - the extension backplane radius is calculated using similar triangles
       */
      snoutLength/2.0,
      vesselRmin0 + wallThickness,
      vesselRmax0 - wallThickness,
      vesselRmin0 + boreDelta * (snoutLength-windowThickness) / vesselLength + wallThickness,
      vesselRmax1 - wallThickness + windowThickness * (vesselRmax1 - vesselRmax0) / snoutLength
      );

  // tank solids
  Cone vesselTank(
      tankLength/2.0,
      vesselSnout.rMin2(),
      vesselRmax2,
      vesselRmin1,
      vesselRmax2
      );
  Cone gasvolTank(
      tankLength/2.0 - windowThickness,
      gasvolSnout.rMin2(),
      vesselRmax2 - wallThickness,
      vesselRmin1 + wallThickness,
      vesselRmax2 - wallThickness
      );

  // snout + tank solids
  UnionSolid vesselUnion(
      vesselTank,
      vesselSnout,
      Position(0., 0., -vesselLength/2.)
      );
  UnionSolid gasvolUnion(
      gasvolTank,
      gasvolSnout,
      Position(0., 0., -vesselLength/2. + windowThickness)
      );

  //  extra solids for `debug_optics` only
  Box vesselBox(1001,1001,1001);
  Box gasvolBox(1000,1000,1000);

  // choose vessel and gasvol solids (depending on `debug_optics_mode` (0=disabled))
  Solid vesselSolid, gasvolSolid;
  switch(debug_optics_mode) {
    case 0:  vesselSolid=vesselUnion;  gasvolSolid=gasvolUnion;  break; // `!debug_optics`
    case 1:  vesselSolid=vesselBox;    gasvolSolid=gasvolBox;    break;
    case 2:  vesselSolid=vesselBox;    gasvolSolid=gasvolUnion;  break;
  };

  // volumes
  Volume vesselVol(detName, vesselSolid, vesselMat);
  Volume gasvolVol(detName+"_gas", gasvolSolid, gasvolMat);
  vesselVol.setVisAttributes(vesselVis);
  gasvolVol.setVisAttributes(gasvolVis);

  {
    // FIXME: Z-location does not really matter here, right?; but Z-axis orientation does;
    auto boundary = new FlatSurface(TVector3(0,0,0), TVector3(1,0,0), TVector3(0,-1,0));

    // FIXME: have no connection to GEANT G4LogicalVolume pointers; however all is needed 
    // is to make them unique so that std::map work internally; resort to using integers, 
    // who cares; material pointer can seemingly be '0', and effective refractive index 
    // for all radiators will be assigned at the end by hand; FIXME: should assign it on 
    // per-photon basis, at birth, like standalone GEANT code does;
    for(int isec=0; isec<nSectors; isec++) 
      geometry->SetContainerVolume(detector, isec, (G4LogicalVolume*)(0x0), 0, boundary);
  }
  // How about PlacedVolume::transformation2mars(), guys?; FIXME: make it simple for now, 
  // assuming no rotations involved; [cm];
  double vesselOffset = (vesselZmin + vesselZmax)/2;


  // reference positions
  // - the vessel is created such that the center of the cylindrical tank volume
  //   coincides with the origin; this is called the "origin position" of the vessel
  // - when the vessel (and its children volumes) is placed, it is translated in
  //   the z-direction to be in the proper ATHENA-integration location
  // - these reference positions are for the frontplane and backplane of the vessel,
  //   with respect to the vessel origin position
  auto originFront = Position(0., 0., -tankLength/2.0 - snoutLength );
  auto originBack =  Position(0., 0., tankLength/2.0 );

  // initialize sensor centroids (used for mirror parameterization below); this is
  // the average (x,y,z) of the placed sensors, w.r.t. originFront
  double sensorCentroidX = 0;
  double sensorCentroidZ = 0;
  int sensorCount = 0;


  // sensitive detector type
  sens.setType("photoncounter");

  // place gas volume
  PlacedVolume gasvolPV = vesselVol.placeVolume(gasvolVol,Position(0, 0, 0));
  DetElement gasvolDE(det, "gasvol_de", 0);
  gasvolDE.setPlacement(gasvolPV);

  // place mother volume (vessel)
  Volume motherVol = desc.pickMotherVolume(det);
  // FIXME: usage of this translation assumes gasvolume2vessel translation is (0,0,0);
  auto gasvolume2master = Position(0, 0, vesselZmin) - originFront;
  printf("@M@ %7.1f %7.1f %7.1f\n", gasvolume2master.x()/mm, gasvolume2master.y()/mm, gasvolume2master.z()/mm);
  PlacedVolume vesselPV = motherVol.placeVolume(vesselVol, gasvolume2master);
  //Position(0, 0, vesselZmin) - originFront
  //  );
  vesselPV.addPhysVolID("system", detID);
  det.setPlacement(vesselPV);

  // SECTOR LOOP //////////////////////////////////
  for(int isec=0; isec<nSectors; isec++) {

    // debugging filters, limiting the number of sectors
    if( (debug_mirror||debug_sensors||debug_optics) && isec!=0) continue;

    // sector rotation about z axis
    double sectorRotation = isec * 360/nSectors * degree;
    std::string secName = "sec" + std::to_string(isec);



    // BUILD RADIATOR ====================================================================

    // solid and volume: create aerogel and filter sectors
    Tube aerogelSolid(radiatorRmin, radiatorRmax, aerogelThickness/2, -radiatorPhiw/2.0, radiatorPhiw/2.0);
    Tube filterSolid( radiatorRmin, radiatorRmax, filterThickness/2,  -radiatorPhiw/2.0, radiatorPhiw/2.0);
    Volume aerogelVol( detName+"_aerogel_"+secName, aerogelSolid, aerogelMat );
    Volume filterVol(  detName+"_filter_"+secName,  filterSolid,  filterMat );
    aerogelVol.setVisAttributes(aerogelVis);
    filterVol.setVisAttributes(filterVis);

    // aerogel placement and surface properties
    // TODO [low-priority]: define skin properties for aerogel and filter
    auto radiatorPos = Position(0., 0., radiatorFrontplane) + originFront;
    auto aerogelPV = gasvolVol.placeVolume(aerogelVol,
          RotationZ(sectorRotation) // rotate about beam axis to sector
        * Translation3D(radiatorPos.x(), radiatorPos.y(), radiatorPos.z()) // re-center to originFront
        * RotationY(radiatorPitch) // change polar angle to specified pitch
        );
    DetElement aerogelDE(det, Form("aerogel_de%d", isec), isec);
    aerogelDE.setPlacement(aerogelPV);
    //SkinSurface aerogelSkin(desc, aerogelDE, Form("mirror_optical_surface%d", isec), aerogelSurf, aerogelVol);
    //aerogelSkin.isValid();

    /*if (!isec)*/ {
      TVector3 nx(1,0,0), ny(0,-1,0);
      auto surface = new FlatSurface((1/mm)*TVector3(0,0,vesselOffset+aerogelPV.position().z()+aerogelThickness/2), nx, ny);
      printf("@M@  aerogel %7.2f\n", (vesselOffset+aerogelPV.position().z()+aerogelThickness/2)/mm);

      // This call will create a pair of flat refractive surfaces internally; FIXME: should make
      // a small gas gap at the upstream end of the gas volume;
      geometry->AddFlatRadiator(detector, isec, (G4LogicalVolume*)(0x1), 0, surface, aerogelThickness/mm);
    } //if

    // filter placement and surface properties
#if _LATER_
    if(!debug_optics) {
      auto filterPV = gasvolVol.placeVolume(filterVol,
            RotationZ(sectorRotation) // rotate about beam axis to sector
          * Translation3D(radiatorPos.x(), radiatorPos.y(), radiatorPos.z()) // re-center to originFront
          * RotationY(radiatorPitch) // change polar angle
          * Translation3D(0., 0., (aerogelThickness+filterThickness)/2.) // move to aerogel backplane
          );
      DetElement filterDE(det, Form("filter_de%d", isec), isec);
      filterDE.setPlacement(filterPV);
      //SkinSurface filterSkin(desc, filterDE, Form("mirror_optical_surface%d", isec), filterSurf, filterVol);
      //filterSkin.isValid();

      /*if (!isec)*/ {
	TVector3 nx(1,0,0), ny(0,-1,0);
	
	// FIXME: create a small air gap in the geometry as well;
	auto surface = new FlatSurface((1/mm)*TVector3(0,0,vesselOffset+filterPV.position().z()+0.01*mm), nx, ny);
	geometry->AddFlatRadiator(detector, isec, (G4LogicalVolume*)(0x2), 0, surface, filterThickness/mm);
      } //if
    };
#endif


    // BUILD MIRRORS ====================================================================

    // derive spherical mirror parameters `(zM,xM,rM)`, for given image point
    // coordinates `(zI,xI)` and `dO`, defined as the z-distance between the
    // object and the mirror surface
    // - all coordinates are specified w.r.t. the object point coordinates
    // - this is point-to-point focusing, but it can be used to effectively steer
    //   parallel-to-point focusing
    double zM,xM,rM;
    auto FocusMirror = [&zM,&xM,&rM](double zI, double xI, double dO) {
      zM = dO*zI / (2*dO-zI);
      xM = dO*xI / (2*dO-zI);
      rM = dO - zM;
    };

    // attributes, re-defined w.r.t. IP, needed for mirror positioning
    double zS = sensorSphCenterZ + vesselZmin; // sensor sphere attributes
    double xS = sensorSphCenterX;
    double rS = sensorSphRadius;
    double B = vesselZmax - mirrorBackplane; // distance between IP and mirror back plane

    // focus 1: set mirror to focus IP on center of sensor sphere `(zS,xS)`
    /*double zF = zS;
    double xF = xS;
    FocusMirror(zF,xF,B);*/

    // focus 2: move focal region along sensor sphere radius, according to `focusTuneLong`
    // - specifically, along the radial line which passes through the approximate centroid
    //   of the sensor region `(sensorCentroidZ,sensorCentroidX)`
    // - `focusTuneLong` is the distance to move, given as a fraction of `sensorSphRadius`
    // - `focusTuneLong==0` means `(zF,xF)==(zS,xS)`
    // - `focusTuneLong==1` means `(zF,xF)` will be on the sensor sphere, near the centroid
    /*
    double zC = sensorCentroidZ + vesselZmin;
    double xC = sensorCentroidX;
    double slopeF = (xC-xS) / (zC-zS);
    double thetaF = std::atan(std::fabs(slopeF));
    double zF = zS + focusTuneLong * sensorSphRadius * std::cos(thetaF);
    double xF = xS - focusTuneLong * sensorSphRadius * std::sin(thetaF);
    //FocusMirror(zF,xF,B);

    // focus 3: move along line perpendicular to focus 2's radial line,
    // according to `focusTunePerp`, with the same numerical scale as `focusTuneLong`
    zF += focusTunePerp * sensorSphRadius * std::cos(M_PI/2-thetaF);
    xF += focusTunePerp * sensorSphRadius * std::sin(M_PI/2-thetaF);
    FocusMirror(zF,xF,B);
    */

    // focus 4: use (z,x) coordinates for tune parameters
    double zF = zS + focusTuneZ;
    double xF = xS + focusTuneX;
    FocusMirror(zF,xF,B);

    /*
    // print some calculated parameters, viz. mirror attributes
    //printf("(zC,xC) = ( %.2f, %.2f )\n",zC,xC);
    //printf("zS = %f\n",zS);
    //printf("xS = %f\n",xS);
    //printf("B = %f\n",B);
    printf("zM = %f\n",zM);
    printf("xM = %f\n",xM);
    printf("rM = %f\n",rM);
    */

    // re-define mirror attributes to be w.r.t vessel front plane
    double mirrorCenterZ = zM - vesselZmin;
    double mirrorCenterX = xM;
    double mirrorRadius = rM;

    // spherical mirror patch cuts and rotation
    double mirrorThetaRot = std::asin(mirrorCenterX/mirrorRadius);
    double mirrorTheta1 = mirrorThetaRot - std::asin((mirrorCenterX-mirrorRmin)/mirrorRadius);
    double mirrorTheta2 = mirrorThetaRot + std::asin((mirrorRmax-mirrorCenterX)/mirrorRadius);

    // if debugging, draw full sphere
    if(debug_mirror) { mirrorTheta1=0; mirrorTheta2=M_PI; /*mirrorPhiw=2*M_PI;*/ };

    // solid : create sphere at origin, with specified angular limits;
    // phi limits are increased to fill gaps (overlaps are cut away later)
    Sphere mirrorSolid1(
        mirrorRadius,
        mirrorRadius + mirrorThickness,
        mirrorTheta1,
        mirrorTheta2,
        -40*degree,
        40*degree
        );

    // mirror placement transformation (note: transformations are in reverse order)
    auto mirrorPos = Position(mirrorCenterX, 0., mirrorCenterZ) + originFront;
    Transform3D mirrorPlacement(
          Translation3D(mirrorPos.x(), mirrorPos.y(), mirrorPos.z()) // re-center to specified position
        * RotationY(-mirrorThetaRot) // rotate about vertical axis, to be within vessel radial walls
        );

    // cut overlaps with other sectors using "pie slice" wedges, to the extent specified
    // by `mirrorPhiw`
    Tube pieSlice( 0.01*cm, vesselRmax2, tankLength/2.0, -mirrorPhiw/2.0, mirrorPhiw/2.0);
    IntersectionSolid mirrorSolid2( pieSlice, mirrorSolid1, mirrorPlacement );

    // mirror volume, attributes, and placement
    Volume mirrorVol(detName+"_mirror_"+secName, mirrorSolid2, mirrorMat);
    mirrorVol.setVisAttributes(mirrorVis);
    {
      Transform3D slice2gasvolume = RotationZ(sectorRotation)*Translation3D(0,0,0);
      {
	auto translation = (slice2gasvolume*mirrorPlacement).Translation();//.Vect();
	//auto x = (trans * (Position(0, 0, vesselZmin) - originFront)).Translation();//.Vect();
	//auto rotation    = trans.Rotation();
	//const TGeoMatrix& localToGlobal = filterDE.nominal().worldTransformation();
	//localToGlobal.LocalToMaster(l, g);
	double xx, yy, zz;
	translation.GetComponents(xx, yy, zz);
	printf("@M@ %10.5f %10.5f %10.5f\n", xx/mm, yy/mm, zz/mm);

	auto surface = new SphericalSurface((1/mm)*TVector3(
							    xx+gasvolume2master.x(), 
							    yy+gasvolume2master.y(), 
							    zz+gasvolume2master.z()), 
					    mirrorRadius/mm);
	detector->AddOpticalBoundary(isec, new OpticalBoundary(0x0, surface, false));
      }
      auto mirrorPV2 = gasvolVol.placeVolume(mirrorVol, slice2gasvolume); // rotate about beam axis to sector
      //RotationZ(sectorRotation) // rotate about beam axis to sector
      //					     * Translation3D(0,0,0)
      //				     );
      
      // properties
      DetElement mirrorDE(det, Form("mirror_de%d", isec), isec);
      mirrorDE.setPlacement(mirrorPV2);
      {
	//double l[3] = {0.0, 0.0, 0.0}, g[3], m[3];
	//mirrorPV2.ptr()->LocalToMaster(l, g);
	//vesselPV. ptr()->LocalToMaster(g, m);
	//printf("@M@   %10.5f %10.5f %10.5f\n", m[0]/mm, m[1]/mm, m[2]/mm);
      }
      SkinSurface mirrorSkin(desc, mirrorDE, Form("mirror_optical_surface%d", isec), mirrorSurf, mirrorVol);
      mirrorSkin.isValid();
    }


    // BUILD SENSORS ====================================================================

    // if debugging sphere properties, restrict number of sensors drawn
    if(debug_sensors) { sensorSide = 2*M_PI*sensorSphRadius / 64; };

    // solid and volume: single sensor module
    Box sensorSolid(sensorSide/2., sensorSide/2., sensorThickness/2.);
    Volume sensorVol(detName+"_sensor_"+secName, sensorSolid, sensorMat);
    sensorVol.setVisAttributes(sensorVis);

    auto sensorSphPos = Position(sensorSphCenterX, 0., sensorSphCenterZ) + originFront;

    // sensitivity
    if(!debug_optics) sensorVol.setSensitiveDetector(sens);

    // SENSOR MODULE LOOP ------------------------
    /* ALGORITHM: generate sphere of positions
     * - NOTE: there are two coordinate systems here:
     *   - "global" the main ATHENA coordinate system
     *   - "generator" (vars end in `Gen`) is a local coordinate system for
     *     generating points on a sphere; it is related to the global system by
     *     a rotation; we do this so the "patch" (subset of generated
     *     positions) of sensors we choose to build is near the equator, where
     *     point distribution is more uniform
     * - PROCEDURE: loop over `thetaGen`, with subloop over `phiGen`, each divided evenly
     *   - the number of points to generate depends how many sensors (+`sensorGap`)
     *     can fit within each ring of constant `thetaGen` or `phiGen`
     *   - we divide the relevant circumference by the sensor
     *     size(+`sensorGap`), and this number is allowed to be a fraction,
     *     because likely we don't care about generating a full sphere and
     *     don't mind a "seam" at the overlap point
     *   - if we pick a patch of the sphere near the equator, and not near
     *     the poles or seam, the sensor distribution will appear uniform
     */

    // initialize module number for this sector
    int imod=0;

    // thetaGen loop: iterate less than "0.5 circumference / sensor size" times
    double nTheta = M_PI*sensorSphRadius / (sensorSide+sensorGap);
    for(int t=0; t<(int)(nTheta+0.5); t++) {
      double thetaGen = t/((double)nTheta) * M_PI;

      // phiGen loop: iterate less than "circumference at this latitude / sensor size" times
      double nPhi = 2*M_PI * sensorSphRadius * std::sin(thetaGen) / (sensorSide+sensorGap);
      for(int p=0; p<(int)(nPhi+0.5); p++) {
        double phiGen = p/((double)nPhi) * 2*M_PI - M_PI; // shift to [-pi,pi]

        // determine global phi and theta
        // - convert {radius,thetaGen,phiGen} -> {xGen,yGen,zGen}
        double xGen = sensorSphRadius * std::sin(thetaGen) * std::cos(phiGen);
        double yGen = sensorSphRadius * std::sin(thetaGen) * std::sin(phiGen);
        double zGen = sensorSphRadius * std::cos(thetaGen);
        // - convert {xGen,yGen,zGen} -> global {x,y,z} via rotation
        double x = zGen;
        double y = xGen;
        double z = yGen;
        // - convert global {x,y,z} -> global {phi,theta}
        double phi = std::atan2(y,x);
        double theta = std::acos(z/sensorSphRadius);

        // shift global coordinates so we can apply spherical patch cuts
        double zCheck = z + sensorSphCenterZ;
        double xCheck = x + sensorSphCenterX;
        double yCheck = y;
        double rCheck = std::hypot(xCheck,yCheck);
        double phiCheck = std::atan2(yCheck,xCheck);

        // patch cut
        bool patchCut =
          std::fabs(phiCheck) < sensorSphPatchPhiw
          && zCheck > sensorSphPatchZmin
          && rCheck > sensorSphPatchRmin
          && rCheck < sensorSphPatchRmax;
        if(debug_sensors) patchCut = std::fabs(phiCheck) < sensorSphPatchPhiw;
        if(patchCut) {

          // append sensor position to centroid calculation
          if(isec==0) {
            sensorCentroidX += xCheck;
            sensorCentroidZ += zCheck;
            sensorCount++;
          };

          // placement (note: transformations are in reverse order)
          // - transformations operate on global coordinates; the corresponding
          //   generator coordinates are provided in the comments
          auto sensorPV = gasvolVol.placeVolume(sensorVol,
                RotationZ(sectorRotation) // rotate about beam axis to sector
              * Translation3D(sensorSphPos.x(), sensorSphPos.y(), sensorSphPos.z()) // move sphere to reference position
              * RotationX(phiGen) // rotate about `zGen`
              * RotationZ(thetaGen) // rotate about `yGen`
              * Translation3D(sensorSphRadius, 0., 0.) // push radially to spherical surface
              * RotationY(M_PI/2) // rotate sensor to be compatible with generator coords
              * RotationZ(-M_PI/2) // correction for readout segmentation mapping
              );

	  {
	    // nx[] and ny[] orientation should be the same as in eRICH, right?;
	    double xxl[3] = {0.0, 0.0, 0.0}, bff[3], xxg[3], nxl[3] = {1.0, 0.0, 0.0}, nyl[3] = {0.0, 1.0, 0.0}, nxg[3], nyg[3];
	    sensorPV.ptr()->LocalToMaster(xxl, bff);
	    vesselPV.ptr()->LocalToMaster(bff, xxg);
	    //printf("@G@ %10.5f %10.5f %10.5f\n", xxg[0]/mm, xxg[1]/mm, xxg[2]/mm);

	    sensorPV.ptr()->LocalToMasterVect(nxl, nxg);
	    sensorPV.ptr()->LocalToMasterVect(nyl, nyg);
	    {
	      TVector3 nx(nxg), ny(nyg);
	      //printf("@G@ %10.5f %10.5f %10.5f\n", xxg[0]/mm, xxg[1]/mm, xxg[2]/mm);
	      auto surface = new FlatSurface((1/mm)*TVector3(xxg), nx, ny);

	      // [0,0]: have neither access to G4VSolid nor to G4Material; IRT code does not care; fine;
	      detector->AddPhotonDetector(isec, new CherenkovPhotonDetector(0, 0, surface));
	    }
	  }

          // generate LUT for module number -> sensor position, for readout mapping tests
          //if(isec==0) printf("%d %f %f\n",imod,sensorPV.position().x(),sensorPV.position().y());

          // properties
          sensorPV.addPhysVolID("sector", isec).addPhysVolID("module", imod);
          DetElement sensorDE(det, Form("sensor_de%d_%d", isec, imod), 10000*isec+imod);
          sensorDE.setPlacement(sensorPV);
          if(!debug_optics) {
            SkinSurface sensorSkin(desc, sensorDE, Form("sensor_optical_surface%d", isec), sensorSurf, sensorVol);
            sensorSkin.isValid();
          };

          // increment sensor module number
          imod++;

        }; // end patch cuts
      }; // end phiGen loop
    }; // end thetaGen loop

    // calculate centroid sensor position
    if(isec==0) {
      sensorCentroidX /= sensorCount;
      sensorCentroidZ /= sensorCount;
    };

    // END SENSOR MODULE LOOP ------------------------


  }; // END SECTOR LOOP //////////////////////////


  // place gas volume
#if _MOVED_
  PlacedVolume gasvolPV = vesselVol.placeVolume(gasvolVol,Position(0, 0, 0));
  DetElement gasvolDE(det, "gasvol_de", 0);
  gasvolDE.setPlacement(gasvolPV);

  // place mother volume (vessel)
  Volume motherVol = desc.pickMotherVolume(det);
  PlacedVolume vesselPV = motherVol.placeVolume(vesselVol,
      Position(0, 0, vesselZmin) - originFront
      );
  vesselPV.addPhysVolID("system", detID);
  det.setPlacement(vesselPV);
#endif

  //@@@ Write the geometry out as a custom TObject class instance; FIXME: unify eRICH & dRICH;
  {
    {
      // C2F6, aerogel, acrylic in this sequence; FIXME: import from the geometry database;
      //double n[] = {1.00080, 1.0170, 1.5017};
      double n[] = {1.00000, 1.0170};//, 1.5017};

      for(unsigned ir=0; ir<sizeof(n)/sizeof(n[0]); ir++) {
	if (ir >= detector->GetRadiatorCount()) break;
	
	detector->Radiators()[ir]->SetReferenceRefractiveIndex(n[ir]);
      } //for ir
    }

    geometry->Write();
    fout->Close();
  }
#endif

  return det;
};

// clang-format off
DECLARE_DETELEMENT(athena_DRICH, createDetector)
