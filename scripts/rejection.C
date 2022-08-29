
//#define _DETECTOR_ "DRICH"
#define _DETECTOR_ "PFRICH"
#define _AEROGEL_
//#define _NPE_REFERENCE_ 211
#define _NPE_REFERENCE_ 321
//#define _USE_RECONSTRUCTED_TRACKS_

#define _TSIZE_ 0.05
#define _LSIZE_ 0.04

void rejection(const char *ifname)//, const char *ofname = 0)
{
  const unsigned gdim = 11, qdim = 40, offset = 16;
  double thmin = 184.2, thstep = 0.2;
  unsigned thstat[2][qdim]; memset(thstat, 0x00, sizeof(thstat));

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

  //std::vector<double> thvector, npvector;

  auto np = new TH1D("np", "Photon count",            50,       0,       50);
#ifdef _AEROGEL_
  unsigned id = 0;
  auto th = new TH1D("th", "",                        50,     180,      200);
  auto tq = new TH1D("tq", "",                        50,     180,      200);
  auto ri = new TH1D("ri", "Refractive Index - 1.0",  50,   0.018,    0.020);
  auto dt = new TH1D("dt", "Cherenkov theta diff",    50,     -10,       10);
  //auto dt = new TH1D("dt", "Cherenkov theta diff",    50,      -1,        1);
#else 
  unsigned id = 1;
  auto th = new TH1D("th", "",         50,      35,       41);
  auto tq = new TH1D("tq", "",         50,      35,       41);
  auto ri = new TH1D("ri", "Refractive Index - 1.0",  50, 0.00075,  0.00077);
  auto dt = new TH1D("dt", "Cherenkov theta diff",    50,      -2,        3);
  //auto dt = new TH1D("dt", "Cherenkov theta diff",    50,      -5,        5);
#endif

  // Use MC truth particles for a "main" loop;
  auto mctracks   = new std::vector<dd4pod::Geant4ParticleData>();
  auto rctracks   = new std::vector<edm4eic::ReconstructedParticleData>();
  auto cherenkov  = new std::vector<edm4eic::CherenkovParticleIDData>();
  it->SetBranchAddress("mcparticles", &mctracks);

  // FIXME: or whatever the branches are called;
#ifdef _USE_RECONSTRUCTED_TRACKS_
  it->SetBranchAddress("rcparticles", &rctracks);
#endif
  it->SetBranchAddress((TString(_DETECTOR_) + "PID").Data(),   &cherenkov);
  auto options = new std::vector<edm4eic::CherenkovPdgHypothesis>();
  it->SetBranchAddress((TString(_DETECTOR_) + "PID_0").Data(), &options);
  auto angles  = new std::vector<edm4eic::CherenkovThetaAngleMeasurement>();
  it->SetBranchAddress((TString(_DETECTOR_) + "PID_1").Data(), &angles);

  // Loop through all events;
  unsigned false_assignment_stat[2] = {0};
  for(int ev=0; ev<it->GetEntries(); ev++) {
    it->GetEntry(ev);

#ifdef _USE_RECONSTRUCTED_TRACKS_
    // First populate the reconstructed-to-simulated particle mapping table;
    std::map<eic::Index, const edm4eic::ReconstructedParticleData*> mc2rc;
    for(const auto &rctrack: *rctracks) 
      mc2rc[rctrack.mcID] = &rctrack;
#endif
    
    // Then the Cherenkov-to-reconstructed mapping; FIXME: may want to use Cherenkov-to-simulated 
    // mapping to start with, for the debugging purposes;
    std::map<eic::Index, const edm4eic::CherenkovParticleIDData*> rc2cherenkov;
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



      // BACK !!!;
      double pp = mctrack.ps.mag(), m = 0.493677;//mctrack.mass;



      //continue;
      printf("%f %f (%4d) \n", mctrack.mass, mctrack.ps.mag(), mctrack.pdgID);

      // Loop through all of the mass hypotheses available for this reconstructed track;
      {
	const edm4eic::CherenkovPdgHypothesis *best = 0;

	for(unsigned iq=cherenkov->options_begin; iq<cherenkov->options_end; iq++) {
	  const auto &option = (*options)[iq];

	  if (option.radiator != id) continue;

	  // Skip electron hypothesis; of no interest here;
	  if (abs(option.pdg) == 11) continue;

	  if (abs(option.pdg) == _NPE_REFERENCE_) {
	    np->Fill(option.npe);

	    //if (ofname) npvector.push_back(option.npe);
	  } //if

	  if (!best || option.weight > best->weight) best = &option;
	  printf("radiator %3d (pdg %5d): weight %7.2f, npe %7.2f\n", 
		 option.radiator, option.pdg, option.weight, option.npe);
	} //for
	printf("\n");

	//if (!best) printf("@J@\n");
	// Check whether the true PDG got a highest score;
	if (!best || best->pdg != mctrack.pdgID) {
	  //printf("@J@ %7.2f\n", best->npe); 
	  false_assignment_stat[best->npe >= 0 ? 0 : 1]++;
	}

	// This assumes of course that at least one radiator was requested in juggler;
	double rindex = (*angles)[id].rindex, theta = (*angles)[id].theta;
	double argument = sqrt(pp*pp + m*m)/(rindex*pp);
	double thp = fabs(argument) <= 1.0 ? acos(argument) : theta;

	(mctrack.pdgID == 321 ? th : tq)->Fill(1000*  theta);
	ri->Fill(rindex - 1.0);
	printf("<n> ~ %8.6f, <th> = %7.2f [mrad]\n", rindex - 1.0, 1000*thp);

	{
	  int thbin = (int)floor((1000*theta - thmin)/thstep);
	  if (thbin >= 0 && thbin < qdim)
	    thstat[mctrack.pdgID == 321 ? 0 : 1][thbin]++;
	}
      }
    } //for track
  } //for ev

  printf("%3d (%3d) false out of %lld\n", false_assignment_stat[0],
	 false_assignment_stat[1], it->GetEntries());

  {
    gROOT->Reset();  

    gStyle->SetTextSize(0.02);
    gStyle->SetLabelSize(0.04,"xy");
    gStyle->SetFrameFillColor(0);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetPadBottomMargin(0.10);
    gStyle->SetPadTopMargin(0.04);
    gStyle->SetPadRightMargin(0.05);
    gStyle->SetPadLeftMargin(0.12);
    
    gStyle->SetStatBorderSize(0);
    gStyle->SetStatColor(kWhite);
    gStyle->SetStatFontSize(0.03);
    gStyle->SetStatFont(52);
    gStyle->SetStatW(.13);
    gStyle->SetFitFormat("5.2f");
    
    TCanvas *cv = new TCanvas("cv", "", 0, 0, 1200, 600);
    cv->UseCurrentStyle();
    cv->SetBorderMode(0);
    cv->SetFrameBorderMode(0);
    cv->SetFrameLineColor(kWhite);
    cv->SetFillColor(kWhite);
    
    cv->Divide(2, 1);
    th->SetLineWidth(2); tq->SetLineWidth(2);

    tq->GetXaxis()->SetLabelFont(52);
    tq->GetYaxis()->SetLabelFont(52);
    tq->GetXaxis()->SetTitleFont(52);
    tq->GetYaxis()->SetTitleFont(52);
    
    tq->GetXaxis()->SetTitle("Reconstructed Cherenkov angle, [mrad]");
    tq->GetXaxis()->SetTitleSize(_TSIZE_);
    tq->GetXaxis()->SetLabelSize(_LSIZE_);
    tq->GetYaxis()->SetTitle("Number of Events");
    tq->GetYaxis()->SetTitleSize(_TSIZE_);
    tq->GetYaxis()->SetLabelSize(_LSIZE_);
    tq->GetXaxis()->SetTitleOffset(0.90);
    tq->GetYaxis()->SetTitleOffset(1.10);
    
    tq->GetXaxis()->SetNdivisions(408);
    tq->GetYaxis()->SetNdivisions(808);
    
    cv->cd(1); 
    gPad->SetGrid();
    tq->Draw();       tq->Fit("gaus");
    th->Draw("SAME"); th->Fit("gaus");

    {
      // First calculate total kaon sample stat;
      unsigned kstat = 0, pistat = 0;//, offset = 16;
      for(unsigned iq=0; iq<qdim; iq++) {
	kstat  += thstat[0][iq];
	pistat += thstat[1][iq];
      } //for iq
      // This is not dramatically efficient;
      unsigned kaccu [qdim]; memset( kaccu, 0x00, sizeof( kaccu));
      unsigned piaccu[qdim]; memset(piaccu, 0x00, sizeof(piaccu));
      for(unsigned iq=0; iq<qdim; iq++) {
	kaccu [iq] = (iq ? kaccu [iq-1] : 0) + thstat[0][iq];
	piaccu[iq] = (iq ? piaccu[iq-1] : 0) + thstat[1][iq];
      } //for iq
      for(unsigned iq=0; iq<qdim; iq++)
	printf("%2d: %4d -> %5.3f   %4d -> %5.3f\n", 
	       iq, kaccu[iq], 1.*kaccu[iq]/kstat, piaccu[iq], 1.*piaccu[iq]/pistat);
      
      float   eff[gdim];
      float   prf[gdim];
      for(unsigned ig=0; ig<gdim; ig++) {
	unsigned ibin = ig + offset;
	eff[ig] = 1.* kaccu[ibin]/kstat;
	prf[ig] = 1.*pistat/piaccu[ibin];
      } //for ig
      float  eeff[gdim] = { .001, .001, .001, .001, .001, .001, .001, .001, .001, .001, .001}; 
      float  eprf[gdim] = { .001, .001, .001, .001, .001, .001, .001, .001, .001, .001, .001}; 

      auto greff = new TGraphErrors(gdim, eff, prf, eeff, eprf);
      greff->SetMarkerSize(1.3);
      greff->SetMarkerStyle(21);
      greff->SetMarkerColor(kBlue);

      auto hdum = new TH1D("hdum", "", 10, 0.50, 1.00);

      hdum->GetXaxis()->SetLabelFont(52);
      hdum->GetYaxis()->SetLabelFont(52);
      hdum->GetXaxis()->SetTitleFont(52);
      hdum->GetYaxis()->SetTitleFont(52);
      
      hdum->GetXaxis()->SetTitle("Kaon detection efficiency, [0..1]");
      hdum->GetXaxis()->SetTitleSize(_TSIZE_);
      hdum->GetXaxis()->SetLabelSize(_LSIZE_);
      hdum->GetYaxis()->SetTitle("Pion rejection factor");
      hdum->GetYaxis()->SetTitleSize(_TSIZE_);
      hdum->GetYaxis()->SetLabelSize(_LSIZE_);
      hdum->GetXaxis()->SetTitleOffset(0.90);
      hdum->GetYaxis()->SetTitleOffset(1.10);
      hdum->SetMinimum(    0.00);
      hdum->SetMaximum(  200.);
      
      hdum->GetXaxis()->SetNdivisions(408);
      hdum->GetYaxis()->SetNdivisions(808);

      cv->cd(2); 
      gPad->SetGrid();
      hdum->Draw();
      greff->Draw("PC");
    }
  } //if
} // evaluation()
