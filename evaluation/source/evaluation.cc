
#include <stdio.h>
#include <stdlib.h>

// ROOT
#include "TTree.h"
#include "TFile.h"

// NPdet
#include "dd4pod/Geant4ParticleCollection.h"
#include "eicd/ReconstructedParticleCollection.h"
#include "eicd/CherenkovPIDCollection.h"
#include "eicd/CherenkovMassHypothesisCollection.h"

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
  auto cherenkov  = new std::vector<eic::CherenkovPIDData>();
  auto hypotheses = new std::vector<eic::CherenkovMassHypothesisData>();
  t->SetBranchAddress("mcparticles", &mctracks);

  // FIXME: or whatever the branches are called;
  t->SetBranchAddress("rcparticles", &rctracks);
  t->SetBranchAddress("cherenkov",   &cherenkov);
  t->SetBranchAddress("hypothesis",  &hypotheses);

  // Loop through all events;
  unsigned false_assignment_stat = 0;
  for(int ev=0; ev<t->GetEntries(); ev++) {
    t->GetEntry(ev);

    // First populate the reconstructed-to-simulated particle mapping table;
    std::map<eic::Index, const eic::ReconstructedParticleData*> mc2rc;
    for(const auto &rctrack: *rctracks) 
      mc2rc[rctrack.mcID] = &rctrack;
    
    // Then the Cherenkov-to-reconstructed mapping; FIXME: may want to use Cherenkov-to-simulated 
    // mapping to start with, for the debugging purposes;
    std::map<eic::Index, const eic::CherenkovPIDData*> rc2cherenkov;
    for(const auto &pid: *cherenkov) 
      rc2cherenkov[pid.recID] = &pid;
    
    // Loop through all MC tracks; 
    for(auto mctrack: *mctracks) {
      // FIXME: consider only primaries for now?;
      if (mctrack.g4Parent) continue;

      // Find a matching reconstructed track;
      auto rctrack = mc2rc.find(mctrack.ID) == mc2rc.end() ? 0 : mc2rc[mctrack.ID];
      if (!rctrack) continue;

      // Find a matching Cherenkov PID record;
      auto cherenkov = rc2cherenkov.find(rctrack->ID) == rc2cherenkov.end() ? 0 : rc2cherenkov[rctrack->ID];
      if (!cherenkov) continue;

      // Loop through all of the mass hypotheses available for this reconstructed track;
      {
	const eic::CherenkovMassHypothesisData *best = 0;

	for(auto hypo: cherenkov->hypothesis) {
	  const auto &current = (*hypotheses)[hypo.value];

	  if (!best || current.weight > best->weight) best = &current;
	} //for hypo

	// Check whether the true PDG got a highest score;
	if (!best || best->pdg != mctrack.pdgID) false_assignment_stat++; 
      }	
    } //for track
  } //for ev

  printf("%d false out of %lld\n", false_assignment_stat, t->GetEntries());

  return 0;
} // main()
