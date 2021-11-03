//----------------------------------
//  eRICH: Electron endcap RICH
//  Author: C. Dilks
//----------------------------------

#include <TFile.h>

#include "DDRec/DetectorData.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"

#include <ParametricSurface.h>
#include <CherenkovRadiator.h>
#include <OpticalBoundary.h>
#include <CherenkovDetectorCollection.h>
#include <CherenkovPhotonDetector.h>

using namespace dd4hep;

// create the detector
static Ref_t createDetector(Detector& desc, xml::Handle_t handle, SensitiveDetector sens) {
  xml::DetElement detElem = handle;
  std::string detName = detElem.nameStr();
  int detID = detElem.id();

  DetElement det(detName, detID);
  xml::Component dims = detElem.dimensions();
  OpticalSurfaceManager surfMgr = desc.surfaceManager();

  //@@@ Create output file and a geometry object pointer; 
  std::string str = detName; std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  auto fout = new TFile((str + "-config.root").c_str(), "RECREATE");
  auto geometry = new CherenkovDetectorCollection();
  // Yes, a single detector per .root file in this environment;
  auto detector = geometry->AddNewDetector(detName.c_str());

  // attributes -----------------------------------------------------------
  // - vessel
  double  vesselLength     =  dims.attr<double>(_Unicode(length));
  double  vesselZmin       =  dims.attr<double>(_Unicode(zmin));
  double  vesselZmax       =  dims.attr<double>(_Unicode(zmax));
  double  vesselRmin0      =  dims.attr<double>(_Unicode(rmin0));
  double  vesselRmin1      =  dims.attr<double>(_Unicode(rmin1));
  double  vesselRmax0      =  dims.attr<double>(_Unicode(rmax0));
  double  vesselRmax1      =  dims.attr<double>(_Unicode(rmax1));
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
  // - sensor module
  auto    sensorElem       =  detElem.child(_Unicode(sensors)).child(_Unicode(module));
  auto    sensorMat        =  desc.material(sensorElem.attr<std::string>(_Unicode(material)));
  auto    sensorVis        =  desc.visAttributes(sensorElem.attr<std::string>(_Unicode(vis)));
  auto    sensorSurf       =  surfMgr.opticalSurface(sensorElem.attr<std::string>(_Unicode(surface)));
  double  sensorSide       =  sensorElem.attr<double>(_Unicode(side));
  double  sensorGap        =  sensorElem.attr<double>(_Unicode(gap));
  double  sensorThickness  =  sensorElem.attr<double>(_Unicode(thickness));
  // - sensor plane
  auto    sensorPlaneElem        =  detElem.child(_Unicode(sensors)).child(_Unicode(plane));
  double  sensorPlaneFrontplane  =  sensorPlaneElem.attr<double>(_Unicode(frontplane));
  double  sensorPlaneRmin        =  sensorPlaneElem.attr<double>(_Unicode(rmin));
  double  sensorPlaneRmax        =  sensorPlaneElem.attr<double>(_Unicode(rmax));
  // - debugging switches
  int   debug_optics_mode  =  detElem.attr<int>(_Unicode(debug_optics));

  // if debugging optics, override some settings
  bool debug_optics = debug_optics_mode > 0;
  if(debug_optics) {
    printout(WARNING,"ERich_geo","DEBUGGING ERICH OPTICS");
    switch(debug_optics_mode) {
      case 1: vesselMat = aerogelMat = filterMat = sensorMat = gasvolMat = desc.material("VacuumOptical"); break;
      case 2: vesselMat = aerogelMat = filterMat = sensorMat = desc.material("VacuumOptical"); break;
      default: printout(FATAL,"ERich_geo","UNKNOWN debug_optics_mode"); return det;
    };
    aerogelVis = sensorVis;
    gasvolVis = vesselVis = desc.invisible();
  };

  // BUILD VESSEL //////////////////////////////////////
  /* - `vessel`: aluminum enclosure, the mother volume of the eRICh
   * - `gasvol`: gas volume, which fills `vessel`; all other volumes defined below
   *   are children of `gasvol`
   */

  // tank solids
  Cone vesselTank(
      vesselLength/2.0,
      vesselRmin1,
      vesselRmax1,
      vesselRmin0,
      vesselRmax0
      );
  Cone gasvolTank(
      vesselLength/2.0 - windowThickness,
      vesselRmin1 + wallThickness,
      vesselRmax1 - wallThickness,
      vesselRmin0 + wallThickness,
      vesselRmax0 - wallThickness
      );

  //  extra solids for `debug_optics` only
  Box vesselBox(1001,1001,1001);
  Box gasvolBox(1000,1000,1000);

  // choose vessel and gasvol solids (depending on `debug_optics_mode` (0=disabled))
  Solid vesselSolid, gasvolSolid;
  switch(debug_optics_mode) {
    case 0:  vesselSolid=vesselTank;   gasvolSolid=gasvolTank;   break; // `!debug_optics`
    case 1:  vesselSolid=vesselBox;    gasvolSolid=gasvolBox;    break;
    case 2:  vesselSolid=vesselBox;    gasvolSolid=gasvolTank;   break;
  };

  // volumes
  Volume vesselVol(detName, vesselSolid, vesselMat);
  Volume gasvolVol(detName+"_gas", gasvolSolid, gasvolMat);
  vesselVol.setVisAttributes(vesselVis);
  gasvolVol.setVisAttributes(gasvolVis);

  // Used in several places;
  TVector3 nx(1,0,0), ny(0,1,0);

  {
    // FIXME: Z-location does not really matter here, right?; but Z-axis orientation does;
    auto boundary = new FlatSurface(TVector3(0,0,vesselZmin), nx, ny);

    // FIXME: have no connection to GEANT G4LogicalVolume pointers; however all is needed 
    // is to make them unique so that std::map works internally; resort to using integers, 
    // who cares; material pointer can seemingly be '0', and the effective refractive index 
    // for all radiators will be assigned at the end by hand; FIXME: should assign it on 
    // per-photon basis, at birth, like standalone GEANT code does;
    geometry->SetContainerVolume(detector, "GasVolume", 0, (G4LogicalVolume*)(0x0), 0, boundary);
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
  auto originFront = Position(0., 0.,  vesselLength/2.0 );
  auto originBack =  Position(0., 0., -vesselLength/2.0 );

  // sensitive detector type
  sens.setType("photoncounter");

  // place gas volume
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

  // SECTOR LOOP //////////////////////////////////
  for(int isec=0; isec<nSectors; isec++) {

    // debugging filters, limiting the number of sectors
    //if( debug_optics && isec!=0) continue;

    // sector rotation about z axis
    double sectorRotation = isec * 360/nSectors * degree;
    std::string secName = "sec" + std::to_string(isec);


    // BUILD RADIATOR //////////////////////////////////////

    // solid and volume: create aerogel and filter sectors
    Tube aerogelSolid(radiatorRmin, radiatorRmax, aerogelThickness/2, -radiatorPhiw/2.0, radiatorPhiw/2.0);
    Tube filterSolid( radiatorRmin, radiatorRmax, filterThickness/2,  -radiatorPhiw/2.0, radiatorPhiw/2.0);
    Volume aerogelVol( detName+"_aerogel_"+secName, aerogelSolid, aerogelMat );
    Volume filterVol(  detName+"_filter_"+secName,  filterSolid,  filterMat );
    aerogelVol.setVisAttributes(aerogelVis);
    filterVol.setVisAttributes(filterVis);

    // aerogel placement and surface properties
    // TODO [low-priority]: define skin properties for aerogel and filter
    auto radiatorPos = Position(0., 0., radiatorFrontplane-0.5*aerogelThickness) + originFront;
    auto aerogelPV = gasvolVol.placeVolume(aerogelVol,
          RotationZ(sectorRotation) // rotate about beam axis to sector
        * Translation3D(radiatorPos.x(), radiatorPos.y(), radiatorPos.z()) // re-center to originFront
        * RotationY(radiatorPitch) // change polar angle to specified pitch
        );
    DetElement aerogelDE(det, Form("aerogel_de%d", isec), isec);
    aerogelDE.setPlacement(aerogelPV);
    //SkinSurface aerogelSkin(desc, aerogelDE, Form("mirror_optical_surface%d", isec), aerogelSurf, aerogelVol);
    //aerogelSkin.isValid();

    if (!isec) {
      auto surface = new FlatSurface((1/mm)*TVector3(0,0,vesselOffset+aerogelPV.position().z()), nx, ny);

      // This call will create a pair of flat refractive surfaces internally; FIXME: should make
      // a small gas gap at the upstream end of the gas volume;
      geometry->AddFlatRadiator(detector, "Aerogel", isec, (G4LogicalVolume*)(0x1), 
				0, surface, aerogelThickness/mm);
    } //if

    // filter placement and surface properties
    if(!debug_optics) {
      auto filterPV = gasvolVol.placeVolume(filterVol,
            RotationZ(sectorRotation) // rotate about beam axis to sector
          * Translation3D(radiatorPos.x(), radiatorPos.y(), radiatorPos.z()) // re-center to originFront
          * RotationY(radiatorPitch) // change polar angle
          * Translation3D(0., 0., -(aerogelThickness+filterThickness)/2.) // move to aerogel backplane
          );
      DetElement filterDE(det, Form("filter_de%d", isec), isec);
      filterDE.setPlacement(filterPV);
      //SkinSurface filterSkin(desc, filterDE, Form("mirror_optical_surface%d", isec), filterSurf, filterVol);
      //filterSkin.isValid();

      if (!isec) {
	// FIXME: create a small air gap in the geometry as well;
	auto surface = new FlatSurface((1/mm)*TVector3(0,0,vesselOffset+filterPV.position().z()-0.01*mm), nx, ny);
	geometry->AddFlatRadiator(detector, "Filter", isec, (G4LogicalVolume*)(0x2), 0, surface, filterThickness/mm);
      } //if
    } //if
  }; // END SECTOR LOOP //////////////////////////


  // BUILD SENSORS ///////////////////////

  // solid and volume: single sensor module
  Box sensorSolid(sensorSide/2., sensorSide/2., sensorThickness/2.);
  Volume sensorVol(detName+"_sensor", sensorSolid, sensorMat);
  sensorVol.setVisAttributes(sensorVis);

  // sensitivity
  if(!debug_optics) sensorVol.setSensitiveDetector(sens);

  // sensor plane positioning: we want |`sensorPlaneFrontplane`| to be the distance between the
  // aerogel backplane (i.e., aerogel/filter boundary) and the sensor active surface (e.g, photocathode)
  double sensorZpos = radiatorFrontplane - aerogelThickness + sensorPlaneFrontplane - 0.5*sensorThickness;
  auto sensorPlanePos = Position(0., 0., sensorZpos) + originFront; // reference position
  // miscellaneous
  int imod=0; // module number
  double tBoxMax = vesselRmax1; // sensors will be tiled in tBox, within annular limits

  // [0,0]: have neither access to G4VSolid nor to G4Material; IRT code does not care; fine;
  auto pd = new CherenkovPhotonDetector(0, 0);
  // FIXME: '0' stands for the unknown (and irrelevant) G4LogicalVolume;
  geometry->AddPhotonDetector(detector, 0, pd);

  // SENSOR MODULE LOOP ------------------------
  /* cartesian tiling loop
   * - start at (x=0,y=0), to center the grid
   * - loop over positive-x positions; for each, place the corresponding negative-x sensor too
   * - nested similar loop over y positions
   */
  double sx,sy;
  for(double usx=0; usx<=tBoxMax; usx+=sensorSide+sensorGap) {
    for(int sgnx=1; sgnx>=(usx>0?-1:1); sgnx-=2) {
      for(double usy=0; usy<=tBoxMax; usy+=sensorSide+sensorGap) {
        for(int sgny=1; sgny>=(usy>0?-1:1); sgny-=2) {

	  //if (imod) continue;

          // sensor (x,y) center
          sx = sgnx*usx;
          sy = sgny*usy;

          // annular cut
          if(std::hypot(sx,sy)<sensorPlaneRmin || std::hypot(sx,sy)>sensorPlaneRmax) continue;

          // placement (note: transformations are in reverse order)
          auto sensorPV = gasvolVol.placeVolume(sensorVol,Transform3D(
                Translation3D(sensorPlanePos.x(), sensorPlanePos.y(), sensorPlanePos.z()) // move to reference position
              * Translation3D(sx, sy, 0.) // move to grid position
              ));

          // generate LUT for module number -> sensor position, for readout mapping tests
          //printf("%d %f %f\n",imod,sensorPV.position().x(),sensorPV.position().y());

	  {
	    // Assume that photosensors can have different 3D surface parameterizations;
	    auto surface = new FlatSurface((1/mm)*TVector3(0.0, 0, vesselOffset+sensorPV.position().z()), nx, ny);

	    detector->CreatePhotonDetectorInstance(0, pd, imod, surface);

	    // Yes, since there are no mirrors in this detector, just close the gas radiator volume by hand (once), 
	    // assuming that all the sensors will be sitting at roughly the same location along the beam line anyway;
	    if (!imod) detector->GetRadiator("GasVolume")->m_Borders[0].second = 
			 dynamic_cast<ParametricSurface*>(surface);
	  } //if

          // properties
          sensorPV.addPhysVolID("module", imod);
          DetElement sensorDE(det, Form("sensor_de_%d", imod), imod);//10000*imod); // TODO: what is this 10000?
          sensorDE.setPlacement(sensorPV);
          if(!debug_optics) {
            SkinSurface sensorSkin(desc, sensorDE, "sensor_optical_surface", sensorSurf, sensorVol); // TODO: 3rd arg needs `imod`?
            sensorSkin.isValid();
          };

          // increment sensor module number
          imod++;
        };
      };
    };
  };
  // END SENSOR MODULE LOOP ------------------------

  //@@@ Write the geometry out as a custom TObject class instance;
  {
    // FIXME: import from the geometry database; FIXME: crappy style in general;
    const char *name[] = {"GasVolume", "Aerogel", "Filter"};
    double         n[] = {     1.0013,    1.0170,   1.5017};
    
    for(unsigned ir=0; ir<sizeof(n)/sizeof(n[0]); ir++) {
      auto radiator = detector->GetRadiator(name[ir]);
      
      if (radiator) radiator->SetReferenceRefractiveIndex(n[ir]);
    } //for ir

    geometry->Write();
    fout->Close();
  }

  return det;
};

// clang-format off
DECLARE_DETELEMENT(athena_ERICH, createDetector)
