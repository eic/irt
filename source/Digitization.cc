
#include <TH1D.h>

#include <CherenkovDetector.h>

#include <GeantImport.h>
#include <Digitization.h>

// Kind of digitization; may want to comment out (or set very fine) for debugging;
#define _SENSOR_ACTIVE_AREA_PIXELLATION_DEFAULT_      (32)

// Timing digitization in [ns];
#define _SINGLE_PHOTON_TIMING_RESOLUTION_DEFAULT_  (0.050)

// -------------------------------------------------------------------------------------

Digitization::Digitization():
  // '0' means: shut down pad by pad, without summing up (which is kind of not ideal);
  m_BlackoutBlowupValue(0),
  m_SinglePhotonTimingResolution(_SINGLE_PHOTON_TIMING_RESOLUTION_DEFAULT_),
  m_SensorActiveAreaPixellation(_SENSOR_ACTIVE_AREA_PIXELLATION_DEFAULT_)
{

} // Digitization::Digitization()

// -------------------------------------------------------------------------------------

CherenkovRadiator *Digitization::AddBlackoutRadiator(const char *rname) {
  auto radiator = GetMyRICH()->GetRadiator(rname);
  
  if (radiator) m_BlackoutRadiators.insert(radiator);
  
  return radiator;
} // Digitization::AddBlackoutRadiator()

// -------------------------------------------------------------------------------------

void Digitization::ProduceDigitizedHits(bool calibration)
{
  // First mix all photons from all particles and "digitize" them; FIXME: add stray background;
  // NB: first iteration makes use of the "calibration" photons, for various purposes; the second
  // one deals with the "real" (detected) ones;
  for(unsigned itr=0; itr</*2*/2; itr++) {
    for(auto mcparticle: Event()->ChargedParticles())
      for(auto rhptr: mcparticle->GetRadiatorHistory()) 
	for(auto photon: mcparticle->GetHistory(rhptr)->Photons()) {
	  //+auto pd = photon->GetPhotonDetector();
	  auto pd = GetMyRICH()->m_PhotonDetectors[0];
	  
	  auto radiator = mcparticle->GetRadiator(rhptr);

	  //if (!photon->WasDetected() /*|| photon->m_ReflectionPoints.size() != 1*/) continue;

	  // In the first iteration select "calibration" photons no matter the "calibration" 
	  // argument was "true" or "false"; avoid as a plague using "real" photons for any 
	  // of the calibration business;
	  if (!itr && !photon->IsUsefulForCalibration())                 continue;
	  // In the second one digitize either the "real" or "calibration" photons;
	  if ( itr && !photon->WasDetected()            && !calibration) continue;
	  if ( itr && !photon->IsUsefulForCalibration() &&  calibration) continue;

	  // This is a bit misleading: ALL "real" photons in the radiators will be used; essentially 
	  // this is the last place where individual photons are available; FIXME: may want to keep 
	  // participating photon pointers in the DigitizedHit class, and populate this histogram later,
	  // using only the hits which were attached to one of the tracks in the PID hypothesis 
	  // combination with the minimal chi^2;
	  if ( itr &&  photon->WasDetected()            && !calibration && IsSelected(radiator)) {
	    //m_SelectedRadiators.find(radiator) != m_SelectedRadiators.end()) {
	    m_hwl->Fill(1239.8/(photon->GetVertexMomentum().Mag()));
	    // FIXME: offset hardcoded;
	    //+m_hvtx->Fill(photon->GetVertexPosition().Z() + 1185.5);
	    //m_hvtx->Fill(photon->GetVertexPosition().Z() - 1300.0);
	    m_hvtx->Fill(photon->GetVertexPosition().Z() + 1300.0);
	    m_hri->Fill(photon->GetVertexRefractiveIndex() - 1.0);
	  } //if

	  {
	    DigitizedHit hit, *duplicate = 0;
	    
	    hit.m_PhotonDetector = pd;
	    hit.m_Copy = photon->GetVolumeCopy();
	    printf("%d %p %f\n", hit.m_Copy, pd, photon->GetVertexRefractiveIndex());
	    
	    hit.m_IRTs = pd->GetIRTs(photon->GetVolumeCopy());
	    if (!hit.m_IRTs) {
	      printf("No photosensor with this cellID found!\n");
	      continue;
	    } //if
	    
#if 1//_TODAY_
	    TVector3 phx = photon->GetDetectionPosition();
	    //printf("@D@ %f %f %f\n", phx.x(), phx.y(), phx.z());
	    if (m_SensorActiveAreaPixellation) {
	      double size = pd->GetActiveAreaSize(), half = size/2;
	      //printf("%f\n", size);
	      // All IRTs are terminated at the photosensor; use the first one; extract 3D 
	      // parameterization of this particular photosensor;
	      auto sensor = dynamic_cast<const FlatSurface*>((*hit.m_IRTs)[0]->tail()->GetSurface());
	      //printf("%f %f\n", sensor->GetCenter().X(), sensor->GetCenter().Y());
	      double lx = sensor->GetLocalX(phx)/* + half*/, ly = sensor->GetLocalY(phx)/* + half*/;
	      //printf("%f %f\n", lx, ly);
	      // Assume 1um out is not an issue;
	      if (fabs(lx) > half + 1E-3 || fabs(ly) > half + 1E-3) {
		//printf("%15.10f %15.10f\n", 
		printf("ERROR: hit outside of the sensor active area: %15.10f %15.10f!\n", lx, ly);
		continue;//goto _next_hit;
	      } //if 
	      lx += half; ly += half;//sensor->GetLocalY(phx) + half;
	      
	      // Smear the coordinate measurement (call it digitization :-); 
	      double pitch = size / m_SensorActiveAreaPixellation, x0 = -half, y0 = -half;
	      hit.m_iX = (int)floor(lx/pitch);
	      hit.m_iY = (int)floor(ly/pitch);
	      // Correct for a possible out of range problem (see above);
	      if (hit.m_iX < 0) hit.m_iX = 0; 
	      if (hit.m_iX >= (int)m_SensorActiveAreaPixellation) hit.m_iX = m_SensorActiveAreaPixellation - 1;
	      if (hit.m_iY < 0) hit.m_iY = 0; 
	      if (hit.m_iY >= (int)m_SensorActiveAreaPixellation) hit.m_iY = m_SensorActiveAreaPixellation - 1;
	      //if (hit.m_iX < 0 || hit.m_iX >= (int)m_SensorActiveAreaPixellation || 
	      //  hit.m_iY < 0 || hit.m_iY >= (int)m_SensorActiveAreaPixellation)
	      //printf("%2d %2d\n", hit.m_iX, hit.m_iY);
	      double dx = pitch*(hit.m_iX + 0.5), dy = pitch*(hit.m_iY + 0.5);
#if 1//_BACK_
	      phx = sensor->GetSpacePoint(x0 + dx, y0 + dy);
#endif
	      
	      // FIXME: very inefficient;
	      for(auto &ptr: m_Hits)
		if (ptr.m_PhotonDetector == pd && ptr.m_Copy == hit.m_Copy && 
		    ptr.m_iX == hit.m_iX && ptr.m_iY == hit.m_iY) {
		  duplicate = &ptr;
		  break;
		} //for ptr .. if
	    } //if

	    {
	      unsigned div = 0x1 << m_BlackoutBlowupValue;

	      switch (itr) {
	      case 0:
		if (m_BlackoutRadiators.find(radiator) != m_BlackoutRadiators.end())
		  // NB: this lookup table is global for a given event, namely it is built on the 
		  // event-level (for all tracks at once rather than on a track-by-track basis);
		  m_BlackoutCells.insert(BlackoutCell(pd, hit.m_Copy, hit.m_iX/div, hit.m_iY/div));
		break;
	      case 1:
		// FIXME: this comparison is inefficient;
		if (m_BlackoutCells.find(BlackoutCell(pd, hit.m_Copy, hit.m_iX/div, hit.m_iY/div)) == m_BlackoutCells.end()) {
		  (duplicate ? duplicate : &hit)->m_DirectionSeeds.push_back(photon->GetVertexMomentum().Unit());
		  // Convert to [ps];
		  double timing = photon->GetDetectionTime() + 
		    (m_SinglePhotonTimingResolution ? m_rndm.Gaus(0.0, m_SinglePhotonTimingResolution) : 0.0);
		  (duplicate ? duplicate : &hit)->m_DetectionTimes.push_back(timing);
		  
		  if (!duplicate) {
		    // FIXME: a hack for the time being;
		    //hit.m_PhotonVertexPosition = photon->GetVertexPosition();
		    //hit.m_PhotonVertexMomentum = photon->GetVertexMomentum();
		    
		    // NB: in case of digitization, for duplicate hits this setting will be identical;
		    //printf("@D@ %f %f %f\n", phx.x(), phx.y(), phx.z());
		    hit.m_DetectionPosition = phx;
		    
		    m_Hits.push_back(hit);
		  } //if
		} //if
		else
		  // Would not hurt to set individual blackout flag as well for those photons which 
		  // were "too close" on the sensor matrix to the ones produce in the window or in the 
		  // gas radiator; what is important though is that such photons will not produce ***hits*** 
		  // for ring imaging purposes;
		  photon->SetBlackoutFlag();
		break;
	      } //switch
	    }
#endif
	  }
	} //for particle..photon    
  } //for itr
} // Digitization::ProduceDigitizedHits()

// -------------------------------------------------------------------------------------
