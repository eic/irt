

void evaluation(const char *dfname)
{
#define _DETECTOR_ "DRICH"
#define _AEROGEL_
  //#define _USE_RECONSTRUCTED_TRACKS_

  // .root file with event tree;
  auto fdata = new TFile(dfname);
  if (!fdata) {
    printf("input file '%s' does not exist\n", dfname);
    exit(0);
  } //if
  TTree *t = dynamic_cast<TTree*>(fdata->Get("events"));
  if (!t) {
    printf("input file '%s' does not have \"events\" tree\n", dfname);
    exit(0);
  } //if

  auto np = new TH1D("np", "Photon count",            50,       0,       50);
#ifdef _AEROGEL_
  unsigned id = 0;
  auto th = new TH1D("th", "Cherenkov theta",        100,     180,      200);
  auto ri = new TH1D("ri", "Refractive Index - 1.0", 100,   0.017,    0.022);
#else 
  unsigned id = 1;
  auto th = new TH1D("th", "Cherenkov theta",        100,      33,       43);
  auto ri = new TH1D("ri", "Refractive Index - 1.0", 100, 0.00074,  0.00079);
  //auto th = new TH1D("th", "Cherenkov theta",        100,    33,     53);
#endif

  // Use MC truth particles for a "main" loop;
  auto mctracks   = new std::vector<dd4pod::Geant4ParticleData>();
  auto rctracks   = new std::vector<eic::ReconstructedParticleData>();
  auto cherenkov  = new std::vector<eic::CherenkovParticleIDData>();
  t->SetBranchAddress("mcparticles", &mctracks);

  // FIXME: or whatever the branches are called;
#ifdef _USE_RECONSTRUCTED_TRACKS_
  t->SetBranchAddress("rcparticles", &rctracks);
#endif
  t->SetBranchAddress((TString(_DETECTOR_) + "PID").Data(),   &cherenkov);
  auto options = new std::vector<eic::CherenkovPdgHypothesis>();
  t->SetBranchAddress((TString(_DETECTOR_) + "PID_0").Data(), &options);
  auto angles  = new std::vector<eic::CherenkovThetaAngleMeasurement>();
  t->SetBranchAddress((TString(_DETECTOR_) + "PID_1").Data(), &angles);

  // Loop through all events;
  unsigned false_assignment_stat[2] = {0};
  for(int ev=0; ev<t->GetEntries(); ev++) {
    t->GetEntry(ev);

#ifdef _USE_RECONSTRUCTED_TRACKS_
    // First populate the reconstructed-to-simulated particle mapping table;
    std::map<eic::Index, const eic::ReconstructedParticleData*> mc2rc;
    for(const auto &rctrack: *rctracks) 
      mc2rc[rctrack.mcID] = &rctrack;
#endif
    
    // Then the Cherenkov-to-reconstructed mapping; FIXME: may want to use Cherenkov-to-simulated 
    // mapping to start with, for the debugging purposes;
    std::map<eic::Index, const eic::CherenkovParticleIDData*> rc2cherenkov;
    for(const auto &pid: *cherenkov) 
      rc2cherenkov[pid.recID] = &pid;
    
    //printf("Here! %d\n", mctracks->size());
    // Loop through all MC tracks; 
    for(auto mctrack: *mctracks) {
      // FIXME: consider only primaries for now?;
      if (mctrack.g4Parent) continue;

#ifdef _USE_RECONSTRUCTED_TRACKS_
      // Find a matching reconstructed track;
      auto rctrack = mc2rc.find(mctrack.ID) == mc2rc.end() ? 0 : mc2rc[mctrack.ID];
      if (!rctrack) continue;

      // Find a matching Cherenkov PID record;
      auto cherenkov = rc2cherenkov.find(rctrack.ID) == rc2cherenkov.end() ? 0 : rc2cherenkov[rctrack.ID];
#else
      auto cherenkov = rc2cherenkov.find(mctrack.ID) == rc2cherenkov.end() ? 0 : rc2cherenkov[mctrack.ID];
#endif
      if (!cherenkov) continue;

      // Loop through all of the mass hypotheses available for this reconstructed track;
      {
	const eic::CherenkovPdgHypothesis *best = 0;

	for(unsigned iq=cherenkov->options_begin; iq<cherenkov->options_end; iq++) {
	  const auto &option = (*options)[iq];

	  if (option.radiator != id) continue;

	  // Skip electron hypothesis; of no interest here;
	  if (abs(option.pdg) == 11) continue;

	  if (abs(option.pdg) == 211) np->Fill(option.npe);

	  if (!best || option.weight > best->weight) best = &option;
	  printf("radiator %3d (pdg %5d): weight %7.2f, npe %7.2f\n", 
		 option.radiator, option.pdg, option.weight, option.npe);
	} //for
	printf("\n");

	// Check whether the true PDG got a highest score;
	if (!best || best->pdg != mctrack.pdgID) 
	  false_assignment_stat[best->npe >= 3 ? 0 : 1]++;
      }

      // This assumes of course that at least one radiator was requested in juggler;
      th->Fill(1000* (*angles)[id].theta);
      ri->Fill(      (*angles)[id].rindex - 1.0);
      printf("%f\n", (*angles)[id].rindex - 1.0);
    } //for track
  } //for ev

  printf("%3d (%3d) false out of %lld\n", false_assignment_stat[0],
	 false_assignment_stat[1], t->GetEntries());

  auto cv = new TCanvas("cv", "", 1500, 600);
  cv->Divide(3, 1);
  cv->cd(1); np->Draw(); np->Fit("gaus");
  cv->cd(2); th->Draw(); th->Fit("gaus");
  cv->cd(3); ri->Draw(); ri->Fit("gaus");
} // evaluation()
