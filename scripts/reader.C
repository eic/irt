
void reader(const char *dfname, const char *cfname, const char *dtname = 0)
{
#define _AEROGEL_
#define _DRICH_

// Optionally: mimic low wave length cutoff and average QE x Geometric sensor efficiency;
#define _WAVE_LENGTH_CUTOFF_MIN_ (350.0)
#define _WAVE_LENGTH_CUTOFF_MAX_ (650.0)
  //#define _AVERAGE_PDE_            ( 0.30)

  // .root file with event tree;
  auto fdata = new TFile(dfname);
  TTree *t = dynamic_cast<TTree*>(fdata->Get("events"));

  // .root file with the IRT configuration;
  auto fcfg  = new TFile(cfname);
  CherenkovDetector *detector = 0;
  auto geometry = dynamic_cast<CherenkovDetectorCollection*>(fcfg->Get("CherenkovDetectorCollection"));

  if (dtname) 
    detector = geometry->GetDetector(dtname);
  else {
    // Assume a single detector (ERICH or DRICH);
    auto &detectors = geometry->GetDetectors();
    if (detectors.size() != 1) {
      printf("More than one detector in the provided IRT geometry config .root file!\n");
      exit(0);
    } //if

    detector = (*detectors.begin()).second;
  } //if

  // Either this or that way, the detector should be available;
  if (!detector) {
    printf("Was not able to find a valid Cherenkov detector in the provided IRT geometry config .root file!\n");
    exit(0);
  } //if

  auto nq = new TH1D("nq", "Photon count",            50,      0,    100);
  auto np = new TH1D("np", "Photon count",            50,      0,    100);
  //auto fi = new TH1D("fi", "Cherenkov phi angle",      30,-180.0,  180.0);
  //auto xi = new TH1D("xi", "Chi square CDFC (photon)",100,   0.0,    1.0);
  //auto wi = new TH1D("wi", "Chi square CDFC (track)",  20,   0.0,    1.0);
  //fi->SetMinimum(0); xi->SetMinimum(0); wi->SetMinimum(0);
#ifdef _AEROGEL_
  //auto pl = new TH1D("pl", "Radiator path Length",    100,   0.0,   50.0);
  auto ep = new TH1D("ep", "Emission Point",          100, -30.0,   30.0);
  //auto ri = new TH1D("ri", "1.0 - Refractive Index",  100, 0.015,  0.025);
#else
  //auto pl = new TH1D("pl", "Radiator path Length",    100,   0.0, 2000.0);
  //auto ep = new TH1D("ep", "Emission Point",          100, -70.0,   70.0);
  //auto ri = new TH1D("ri", "1.0 - Refractive Index",  100, 0.015,  0.025);
#endif
  auto wl = new TH1D("wl", "Wave Length",             100, 200.0, 1000.0);
  //auto th = new TH1D("th", "Cherenkov angle",         100, -10,  10);
  //auto tq = new TH1D("tq", "Average Cherenkov angle", 100, -10,  10);

  auto gas      = detector->GetRadiator("GasVolume");
  auto aerogel  = detector->GetRadiator("Aerogel");
  //auto acrylic  = detector->GetRadiator("Filter");
  // Assume the reference value was close enough in ERich_geo.cpp; since QE was not accounted, 
  // this may not be true; 
  gas    ->m_AverageRefractiveIndex = gas    ->n();
  aerogel->m_AverageRefractiveIndex = 1.020;//aerogel->n();
  //acrylic->m_AverageRefractiveIndex = acrylic->n();

  //#ifdef _DRICH_
  aerogel->SetGaussianSmearing(0.001);
  //#else
  //aerogel->SetUniformSmearing(0.005);
  //#endif
  // Be aware, that AddLocations() part should take this into account;
  aerogel->SetTrajectoryBinCount(1);
  // This may be bogus for a blob-like operation mode;
  //gas    ->SetUniformSmearing(0.003);

  // TTree interface variable;
  auto event = new CherenkovEvent();

  // Use MC truth particles, and deal with just eRICH hits here; however the interface 
  // should work for combinations like eRICH+DIRC, eventually; 
  std::vector<dd4pod::Geant4ParticleData>     *tracks = new std::vector<dd4pod::Geant4ParticleData>();
  std::vector<dd4pod::PhotoMultiplierHitData> *hits   = new std::vector<dd4pod::PhotoMultiplierHitData>();
  t->SetBranchAddress("mcparticles", &tracks);
  {
    TString hname; hname.Form("%sHits", detector->GetName());
    t->SetBranchAddress(hname,   &hits);
  }

  // Loop through all events;
  unsigned false_assignment_stat = 0;
  for(int ev=0; ev<t->GetEntries(); ev++) {
    t->GetEntry(ev);

    { 
      unsigned stat = 0;

      for(auto track: *tracks) {
	if (track.pdgID == -22) {
	  auto &p = track.ps;
	  double pmag = sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
	  double wave_length = 1239.84/(1E9*pmag);
	  
#ifdef _WAVE_LENGTH_CUTOFF_MIN_ 
	  if (wave_length < _WAVE_LENGTH_CUTOFF_MIN_) continue;
#endif
#ifdef _WAVE_LENGTH_CUTOFF_MAX_ 
	  if (wave_length > _WAVE_LENGTH_CUTOFF_MAX_) continue;
#endif
#ifdef _AVERAGE_PDE_  
	  if (gRandom->Uniform(0.0, 1.0) > _AVERAGE_PDE_) continue;
#endif
	  stat++;
	} //if
      } //for track

      nq->Fill(stat);
    } 
    //printf("%3ld vs %3ld\n", tracks->size(), hits->size()); 
    // Loop through all tracks and populate the internal arrays in a way 
    // IRT code expects; FIXME: this is not dramatically efficient; streamline once debugging is over;
    for(auto track: *tracks) {
      // FIXME: consider only primaries here?;
      if (track.g4Parent) continue;

      // Create a combined (for all radiators) hit array; eventually will move this
      // structure out of the track loop;
      std::vector<OpticalPhoton*> photons;  
      for(auto hit: *hits) {
	//printf("%3d %3d\n", hit.g4ID, hit.truth.trackID);//MonteCarlo); 
	// FIXME: yes, use MC truth here; not really needed I guess; 
	//if (hit.g4ID != track.ID) continue;
	  
	{
	  auto phtrack = (*tracks)[hit.truth.trackID];
	  //printf("%7.2f\n", phtrack.vs.z);
	  //ep->Fill(phtrack.vs.z + 1515.0);
	  ep->Fill(phtrack.vs.z - 1920.0);
	}

	{
	  auto &p = hit.momentum;
	  double pmag = sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
	  double wave_length = 1239.84/(1E9*pmag);
	  
#ifdef _WAVE_LENGTH_CUTOFF_MIN_ 
	  if (wave_length < _WAVE_LENGTH_CUTOFF_MIN_) continue;
#endif
#ifdef _WAVE_LENGTH_CUTOFF_MAX_ 
	  if (wave_length > _WAVE_LENGTH_CUTOFF_MAX_) continue;
#endif
#ifdef _AVERAGE_PDE_  
	  if (gRandom->Uniform(0.0, 1.0) > _AVERAGE_PDE_) continue;
#endif
	  wl->Fill(wave_length);
	}
	{
	  //auto phtrack = tracks[hit.g4ID];

	} 
	
	auto photon = new OpticalPhoton();
	
	{
	  auto &x = hit.position;
	  photon->SetDetectionPosition(TVector3(x.x, x.y, x.z));
	}

	// A single photodetector type is used;
	photon->SetPhotonDetector(detector->m_PhotonDetectors[0]);
	photon->SetDetected(true);
	// Get cell index; mask out everything apart from {module,sector};
	photon->SetVolumeCopy(hit.cellID & detector->GetReadoutCellMask());
	
	photons.push_back(photon);
      } //for hit

      auto particle = new ChargedParticle(track.pdgID);
      event->AddChargedParticle(particle);

      aerogel->ResetLocations();

      // Create a fake (empty) history; then track locations at the aerogel boundaries;
      particle->StartRadiatorHistory(std::make_pair(aerogel, new RadiatorHistory()));
      {
	// FIXME: need it not at vertex, but in the radiator; as coded here, this can 
	// hardly work once the magnetic field is turned on;
	auto &vtx = track.vs, &p = track.ps;
	auto x0 = TVector3(vtx.x, vtx.y, vtx.z), p0 = TVector3(p.x, p.y, p.z), n0 = p0.Unit();

	// So, give the algorithm aerogel surface boundaries as encoded in ERich_geo.cpp;
	TVector3 from, to;
	aerogel->GetFrontSide(0)->GetCrossing(x0, n0, &from);
	aerogel->GetRearSide (0)->GetCrossing(x0, n0, &to);
	  
	// Move the points a bit inwards;
	TVector3 nn = (to - from).Unit(); from += (0.010)*nn; to -= (0.010)*nn;
	aerogel->AddLocation(from, p0);
	aerogel->AddLocation(  to, p0);
	//printf("@@@ %f %f\n", from.z(), to.z());// - from.z());
      }

      // Now that all internal track-level structures are populated, run IRT code;
      {
	CherenkovPID pid;

	// Consider just pi/K case for now;
	pid.AddMassHypothesis(0.140);
	pid.AddMassHypothesis(0.494);

	particle->PIDReconstruction(pid, &photons);
	{
	  auto pion = pid.GetHypothesis(0), kaon = pid.GetHypothesis(1);
	  double wt0 = pion->GetWeight(aerogel), wt1 = kaon->GetWeight(aerogel);

	  np->Fill(pion->GetNpe(aerogel));

	  printf("%10.3f (%10.3f) vs %10.3f (%10.3f) ...  %3d %d\n", 
		 wt0, pion->GetNpe(aerogel), wt1, kaon->GetNpe(aerogel), particle->GetPDG(), wt0 > wt1);

	  if (wt0 <= wt1) false_assignment_stat++;
	}
      }
    } //for track

    event->Reset();
  } //for ev
  printf("%d false out of %lld\n", false_assignment_stat, t->GetEntries());

  //auto cv = new TCanvas("cv", "", 800, 600);
  //cv->cd(1); np->Draw();
  auto cv = new TCanvas("cv", "", 1600, 800);
  cv->Divide(4,2);
  //cv->cd(1); th->Draw(); 
  cv->cd(2); wl->Draw();
  cv->cd(3); np->Draw();
  cv->cd(4); nq->Draw();
  //cv->cd(4); xi->Draw();
  //cv->cd(5); tq->Draw(); tq->Fit("gaus");
  //cv->cd(6); fi->Draw();
  cv->cd(7); ep->Draw();
  //cv->cd(8); wi->Draw();
} // reader()
