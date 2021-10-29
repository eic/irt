
// ROOT
#include "TTree.h"
#include "TFile.h"

// NPdet
#include "dd4pod/Geant4ParticleCollection.h"
#include "dd4pod/PhotoMultiplierHitCollection.h"

// IRT
#include <CherenkovEvent.h>
#include <CherenkovDetectorCollection.h>

int main(int argc, char** argv) 
{
  // Command line "parser";
  if (argc != 3) {
    printf("usage: %s <root-data-file> <root-config-file>\n", argv[0]);
    exit(0);
  } //if

  // .root file with event tree;
  auto fdata = new TFile(argv[1]);
  TTree *t = dynamic_cast<TTree*>(fdata->Get("events"));

  // .root file with the IRT configuration;
  auto fcfg  = new TFile(argv[2]);
  auto geometry = dynamic_cast<CherenkovDetectorCollection*>(fcfg->Get("CherenkovDetectorCollection"));
  // Assume a single detector (eRICH), and that aerogel was encoded as raditor#1 in ERich_geo.cpp;
  auto detector = geometry->GetDetector(0);
  auto gas      = detector->Radiators()[0];
  auto aerogel1 = detector->Radiators()[1];
  auto acrylic  = detector->Radiators()[2];
  //auto aerogel3 = detector->Radiators()[3];
  // Assume the reference value was close enough in ERich_geo.cpp; since QE was not accounted, 
  // this may not be true; 
  gas     ->m_AverageRefractiveIndex = gas     ->n();
  aerogel1->m_AverageRefractiveIndex = aerogel1->n();
  acrylic ->m_AverageRefractiveIndex = acrylic ->n();
  //aerogel3->m_AverageRefractiveIndex = aerogel3->n();

  // TTree interface variable;
  auto event = new CherenkovEvent();

  // Use MC truth particles, and deal with just eRICH hits here; however the interface 
  // should work for combinations like eRICH+DIRC, eventually; 
  std::vector<dd4pod::Geant4ParticleData>     *tracks = new std::vector<dd4pod::Geant4ParticleData>();
  std::vector<dd4pod::PhotoMultiplierHitData> *hits   = new std::vector<dd4pod::PhotoMultiplierHitData>();
  t->SetBranchAddress("mcparticles", &tracks);
  t->SetBranchAddress("ERICHHits",   &hits);

  // Loop through all events;
  unsigned false_assignment_stat = 0;
  for(int ev=0; ev<t->GetEntries(); ev++) {
    t->GetEntry(ev);

    // Loop through all tracks and populate the internal arrays in a way 
    // IRT code expects; FIXME: this is not dramatically efficient; streamline once debugging is over;
    for(auto track: *tracks) {
      // FIXME: consider only primaries here?;
      if (track.g4Parent /*|| track.pdgID != 211*/) continue;

      auto particle = new ChargedParticle(track.pdgID);
      event->AddChargedParticle(particle);

      for(unsigned irad=0; irad<1; irad++) {
	auto history = new RadiatorHistory();
	// FIXME: yes, for now assume all the photons were produced in aerogel; 
	particle->StartRadiatorHistory(std::make_pair(/*irad ? aerogel3 :*/ aerogel1, history));
	//particle->StartRadiatorHistory(std::make_pair(gas, history));
	{
	  // FIXME: need it not at vertex, but in the radiator; as coded here, this can 
	  // hardly work once the magnetic field is turned on;
	  auto &vtx = track.vs, &p = track.ps;
	  auto x0 = TVector3(vtx.x, vtx.y, vtx.z), p0 = TVector3(p.x, p.y, p.z), n0 = p0.Unit();
	  // So, give the algorithm aerogel surface boundaries as encoded in ERich_geo.cpp;
	  
	  //auto s1 = detector->m_OpticalBoundaries[1], s2 = detector->m_OpticalBoundaries[2];
	  auto s1 = detector->m_OpticalBoundaries[irad ? 5 : 1], s2 = detector->m_OpticalBoundaries[irad ? 6 : 2];
	  //printf("%ld\n", detector->m_OpticalBoundaries.size());
	  
	  TVector3 from, to;
	  s1->GetSurface()->GetCrossing(x0, n0, &from);
	  s2->GetSurface()->GetCrossing(x0, n0, &to);
	  
	  history->AddStep(new ChargedParticleStep(from, p0));
	  history->AddStep(new ChargedParticleStep(  to, p0));
	}

	for(auto hit: *hits) {
	  //printf("New hit!\n");
	  // FIXME: yes, use MC truth here; not really needed I guess; 
	  if (hit.g4ID != track.ID) continue;
	  
	  auto photon = new OpticalPhoton();
	  
	  {
	    auto &x = hit.position;
	    photon->SetDetectionPosition(TVector3(x.x, x.y, x.z));
	  }
	  //<id>system:8,module:12,x:20:16,y:16</id>
	  //printf("%4ld %4ld %4ld %4ld\n", 
	  //     ( hit.cellID        & 0x00FF), 
	  //     ((hit.cellID >>  8) & 0x0FFF),
	  //     ((hit.cellID >> 12) & 0xFFFF),
	  //     ((hit.cellID >> 28) & 0xFFFF));
	  unsigned module = (hit.cellID >>  8) & 0x0FFF;
	  if (module < detector->m_PhotonDetectors.size()) {
	    photon->SetPhotonDetector(detector->m_PhotonDetectors[module]);
	    photon->SetDetected(true);
	    
	    history->AddOpticalPhoton(photon);
	  } //if
	} //for hit
	//printf("%d %d %d\n", track.ID, track.pdgID, track.g4Parent);
      } //for irad	

      // Now that all internal track-level structures are populated, run IRT code;
      {
	CherenkovPID pid;

	// Consider just pi/K case for now;
	pid.AddMassHypothesis(0.140);
	pid.AddMassHypothesis(0.494);

	particle->PIDReconstruction(pid);
	{
	  auto pion = pid.GetHypothesis(0), kaon = pid.GetHypothesis(1);
	  double wt0 = pion->GetWeight(), wt1 = kaon->GetWeight();

	  printf("%10.5f (%5.1f) vs %10.5f (%5.1f) ...  %3d %d\n", 
		 wt0, pion->GetNpe(), wt1, kaon->GetNpe(), track.pdgID, wt0 > wt1);

	  if (wt0 < wt1) false_assignment_stat++;
	}
      }
    } //for track

    event->Reset();
  } //for ev
  printf("%d false out of %lld\n", false_assignment_stat, t->GetEntries());

  return 0;
} // main()
