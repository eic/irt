
//#define _DETECTOR_ "DRICH"
#define _DETECTOR_ "ERICH"

#define _AEROGEL_

#define _NPE_REFERENCE_ 211
//#define _NPE_REFERENCE_ (11)
//#define _NPE_REFERENCE_ 321

void evaluation(const char *ifname, const char *ofname = 0)
{
  // .root file with event tree;
  auto ifdata = new TFile(ifname);
  if (!ifdata) {
    printf("input file '%s' does not exist\n", ifname);
    exit(0);
  } //if
  TTree *it = dynamic_cast<TTree*>(ifdata->Get("events"));
  if (!it) {
    printf("input file '%s' does not have \"events\" tree\n", ifname);
    exit(0);
  } //if

  std::vector<double> thvector, npvector;

  auto np = new TH1D("np", "Photon count",            50,       0,       50);
#ifdef _AEROGEL_
  unsigned id = 0;
  auto th = new TH1D("th", "Cherenkov theta",         50,     180,      200);
  auto ri = new TH1D("ri", "Refractive Index - 1.0",  50,   0.018,    0.020);
  auto dt = new TH1D("dt", "Cherenkov theta diff",    50,     -10,       10);
  //auto dt = new TH1D("dt", "Cherenkov theta diff",    50,      -1,        1);
#else 
  unsigned id = 1;
  auto th = new TH1D("th", "",         50,      35,       41);
  auto tq = new TH1D("tq", "",         50,      35,       41);
  auto ri = new TH1D("ri", "Refractive Index - 1.0",  50, 0.00075,  0.00077);
  auto dt = new TH1D("dt", "Cherenkov theta diff",    50,      -3,        3);
  //auto dt = new TH1D("dt", "Cherenkov theta diff",    50,      -5,        5);
#endif
  auto wl = new TH1D("wl", "Wave length",             50,     350,      900);

  // Use MC truth particles for a "main" loop;
  auto mctracks   = new std::vector<dd4pod::Geant4ParticleData>();
  auto rctracks   = new std::vector<eic::ReconstructedParticleData>();
  auto cherenkov  = new std::vector<eic::CherenkovParticleIDData>();
  it->SetBranchAddress("mcparticles", &mctracks);

  it->SetBranchAddress((TString(_DETECTOR_) + "PID").Data(),   &cherenkov);
  auto options = new std::vector<eic::CherenkovPdgHypothesis>();
  it->SetBranchAddress((TString(_DETECTOR_) + "PID_0").Data(), &options);
  auto angles  = new std::vector<eic::CherenkovThetaAngleMeasurement>();
  it->SetBranchAddress((TString(_DETECTOR_) + "PID_1").Data(), &angles);

  // Loop through all events;
  unsigned false_assignment_stat[2] = {0};
  for(int ev=0; ev<it->GetEntries(); ev++) {
    it->GetEntry(ev);
    
    // Then the Cherenkov-to-reconstructed mapping; FIXME: may want to use Cherenkov-to-simulated 
    // mapping to start with, for the debugging purposes;
    std::map<eic::Index, const eic::CherenkovParticleIDData*> rc2cherenkov;
    for(const auto &pid: *cherenkov) 
      rc2cherenkov[pid.recID] = &pid;
    
    // Loop through all MC tracks; 
    for(auto mctrack: *mctracks) {
      // FIXME: consider only primaries for now?;
      if (mctrack.g4Parent) continue;

      auto cherenkov = rc2cherenkov.find(mctrack.ID) == rc2cherenkov.end() ? 0 : rc2cherenkov[mctrack.ID];
      if (!cherenkov) continue;

      double pp = mctrack.ps.mag(), m = mctrack.mass;

      //printf("m=%5.3f p=%5.1f (%4d) \n", mctrack.mass, mctrack.ps.mag(), mctrack.pdgID);

      // Loop through all of the mass hypotheses available for this reconstructed track;
      {
	const eic::CherenkovPdgHypothesis *best = 0;

	for(unsigned iq=cherenkov->options_begin; iq<cherenkov->options_end; iq++) {
	  const auto &option = (*options)[iq];

	  if (option.radiator != id) continue;

	  // Skip electron hypothesis; of no interest here;
	  //if (abs(option.pdg) == 11) continue;

	  if (abs(option.pdg) == _NPE_REFERENCE_) {
	    np->Fill(option.npe);

	    if (ofname) npvector.push_back(option.npe);
	  } //if

	  if (!best || option.weight > best->weight) best = &option;
	  printf("radiator %3d (pdg %5d): weight %7.2f, npe %7.2f\n", 
		 option.radiator, option.pdg, option.weight, option.npe);
	} //for ih
	printf("\n");

	// Check whether the true PDG got a highest score;
	if (!best || best->pdg != mctrack.pdgID) false_assignment_stat[best->npe >= 5 ? 0 : 1]++;

	// This assumes of course that at least one radiator was requested in juggler;
	double rindex = (*angles)[id].rindex, theta = (*angles)[id].theta, lambda = (*angles)[id].wavelength;
	double argument = sqrt(pp*pp + m*m)/(rindex*pp);
	double thp = fabs(argument) <= 1.0 ? acos(argument) : theta;

	th->Fill(1000 * theta);
	/*if (mctrack.pdgID == 321)*/ dt->Fill(1000* (theta - thp));
	ri->Fill(rindex - 1.0);
	wl->Fill(lambda);//rindex - 1.0);
	printf("<n> ~ %8.6f, <th> = %7.2f [mrad]\n", rindex - 1.0, 1000*thp);

	if (ofname) thvector.push_back(theta - thp);
      }
    } //for track
  } //for ev

  printf("%3d (%3d) false out of %lld\n", false_assignment_stat[0],
	 false_assignment_stat[1], it->GetEntries());

  if (ofname) {
    ifdata->Close();

    auto *ofdata = new TFile(ofname, "RECREATE");

    if (!ofdata) {
      printf("was not able to create output file '%s'\n", ofname);
      exit(0);
    } //if
    auto *ot = new TTree("t", "My tree");

    double thbff, npbff;
    ot->Branch("th", &thbff, "th/D");
    ot->Branch("np", &npbff, "np/D");

    for(unsigned iq=0; iq<thvector.size(); iq++) {
      thbff = thvector[iq];
      npbff = npvector[iq];

      ot->Fill();
    } //for iq

    ot->Write();
    ofdata->Close();
    exit(0);
  } else {
    auto cv = new TCanvas("cv", "", 1700, 500);
    cv->Divide(5, 1);
    cv->cd(1); np->Draw();       np->Fit("gaus");
    cv->cd(2); th->Draw("SAME"); th->Fit("gaus");
    cv->cd(3); ri->Draw();       ri->Fit("gaus");
    cv->cd(4); dt->Draw();       dt->Fit("gaus");
    cv->cd(5); wl->Draw();       //dt->Fit("gaus");
  } //if
} // evaluation()
