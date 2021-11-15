
#include <stdio.h>
#include <stdlib.h>

// ROOT
#include "TTree.h"
#include "TFile.h"

//#define _USE_RECONSTRUCTED_TRACKS_

// NPdet
#include "dd4pod/Geant4ParticleCollection.h"
#include "eicd/ReconstructedParticleCollection.h"
#include "eicd/CherenkovParticleIDCollection.h"
#include "eicd/CherenkovPdgHypothesis.h"

int main(int argc, char** argv) 
{
  // Command line "parser";
  if (argc != 2) {
    printf("usage: %s <root-data-file>\n", argv[0]);
    exit(0);
  } //if

  // .root file with event tree;
  auto fdata = new TFile(argv[1]);
  if (!fdata) {
    printf("input file '%s' does not exist\n", argv[1]);
    exit(0);
  } //if
  TTree *t = dynamic_cast<TTree*>(fdata->Get("events"));
  if (!t) {
    printf("input file '%s' does not have \"events\" tree\n", argv[1]);
    exit(0);
  } //if

  // Use MC truth particles for a "main" loop;
  auto mctracks   = new std::vector<dd4pod::Geant4ParticleData>();
  auto rctracks   = new std::vector<eic::ReconstructedParticleData>();
  auto cherenkov  = new std::vector<eic::CherenkovParticleIDData>();
  t->SetBranchAddress("mcparticles", &mctracks);

  // FIXME: or whatever the branches are called;
#ifdef _USE_RECONSTRUCTED_TRACKS_
  t->SetBranchAddress("rcparticles", &rctracks);
#endif
  t->SetBranchAddress("ERICHPID",   &cherenkov);
  auto options = new std::vector<eic::CherenkovPdgHypothesis>();
  t->SetBranchAddress("ERICHPID_0", &options);

  // Loop through all events;
  unsigned false_assignment_stat = 0;
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

	//printf("%d %d\n", cherenkov->options_begin, cherenkov->options_end);
	for(unsigned iq=cherenkov->options_begin; iq<cherenkov->options_end; iq++) {
	  const auto &option = (*options)[iq];

	  // Skip electron hypothesis; of no interest here;
	  if (abs(option.pdg) == 11) continue;

	  if (!best || option.weight > best->weight) best = &option;
	  printf("radiator %3d (pdg %5d): weight %7.2f, npe %7.2f\n", 
		 option.radiator, option.pdg, option.weight, option.npe);
	} //for
	printf("\n");

	// Check whether the true PDG got a highest score;
	if (!best || best->pdg != mctrack.pdgID) false_assignment_stat++;
      }
    } //for track
  } //for ev

  printf("%d false out of %lld\n", false_assignment_stat, t->GetEntries());

  return 0;
} // main()
