//
//
//

#define _AEROGEL_

#include "HepMC3/GenEvent.h"
#include "HepMC3/ReaderAscii.h"
#include "HepMC3/WriterAscii.h"
#include "HepMC3/Print.h"

#include <iostream>
#include <random>
#include <cmath>
#include <math.h>
#include <TMath.h>

using namespace HepMC3;

void drich_hepmc_loop_writer(int n_events)
{
  auto *DatabasePDG = new TDatabasePDG();

  int pdg[] = {211, 321, 2212};
  unsigned gdim = sizeof(pdg) / sizeof(pdg[0]);

  double eta[] = {1.2, 1.6, 2.0, 2.4, 2.8, 3.0, 3.2, 3.4, 3.5, 3.6};
  unsigned edim = sizeof(eta) / sizeof(eta[0]);

#ifdef _AEROGEL_
  double mom[] = {0.9, 1.4, 2.9,  4.2,  5.5, 10.0, 15.0, 20.0, 27.0, 40.0};
#else 
  double mom[] = {4.0, 5.0, 7.0, 14.0, 17.0, 21.0, 27.0, 30.0, 32.0, 35.0, 40.0, 45.0, 50.0, 55.0, 60.0}; 
#endif
  unsigned mdim = sizeof(mom) / sizeof(mom[0]);

  char fname[128-1];
#ifdef _AEROGEL_
  const char *format = "HEPMC.A/drich-data.%04d..%04.2f-%04.2f..%04.1f-%04.1f..hepmc";
#else
  const char *format = "HEPMC.G/drich-data.%04d..%04.2f-%04.2f..%04.1f-%04.1f..hepmc";
#endif

  unsigned int seed = 0x12345678;
  std::cout << "init seed for random generator is " << seed << std::endl;

  TRandom *rdmn_gen = new TRandom(seed);

  for(unsigned ig=0; ig<gdim; ig++) {
    auto *particle = DatabasePDG->GetParticle(pdg[ig]);

    for(unsigned ie=0; ie<edim-1; ie++)
      for(unsigned im=0; im<mdim-1; im++) {
#ifdef _AEROGEL_
	if (pdg[ig] == 321  && mom[im] <  2.9) continue;
	if (pdg[ig] == 2212 && mom[im] <  5.5) continue;
#else
	if (pdg[ig] == 321  && mom[im] < 14.0) continue;
	if (pdg[ig] == 2212 && mom[im] < 27.0) continue;
#endif

	snprintf(fname, 128-1, format, pdg[ig], eta[ie], eta[ie+1], mom[im], mom[im+1]);

	WriterAscii hepmc_output(fname);
	int events_parsed = 0;
	GenEvent evt(Units::GEV, Units::MM);

	for (events_parsed = 0; events_parsed < n_events; events_parsed++) {
	  GenParticlePtr p1 =
	    std::make_shared<GenParticle>(FourVector(0.0, 0.0,  12.0,  12.0),     11, 4);
	  GenParticlePtr p2 = 
	    std::make_shared<GenParticle>(FourVector(0.0, 0.0, 100.0, 100.004), 2212, 4); 

	  GenVertexPtr v1 = std::make_shared<GenVertex>();
	  v1->add_particle_in(p1);
	  v1->add_particle_in(p2);
	  
	  // type 1 is final state; 
	  Double_t qeta  = rdmn_gen->Uniform(eta[ie], eta[ie+1]);
	  Double_t th    = 2*std::atan(exp(-qeta));
	  Double_t p     = rdmn_gen->Uniform(mom[im], mom[im+1]);
	  Double_t phi   = rdmn_gen->Uniform(0.0, 2*M_PI);
	  
	  Double_t px    = p * std::cos(phi) * std::sin(th);
	  Double_t py    = p * std::sin(phi) * std::sin(th);
	  Double_t pz    = p * std::cos(th);
	  	  
	  GenParticlePtr pq = std::make_shared<GenParticle>(FourVector(
								       px, py, pz,
								       sqrt(p*p + pow(particle->Mass(), 2))),
							    pdg[ig], 1);
	  v1->add_particle_out(pq);

	  evt.add_vertex(v1);
	  
	  if (events_parsed == 0) {
	    std::cout << "First event: " << std::endl;
	    Print::listing(evt);
	  }
	  
	  hepmc_output.write_event(evt);
	  if (events_parsed % 10000 == 0) {
	    std::cout << "Event: " << events_parsed << std::endl;
	  }
	  evt.clear();
	} //for ev

	hepmc_output.close();
	std::cout << "Events parsed and written: " << events_parsed << std::endl;
      } //for ie..im 
  } //for ig  

  exit(0);
}
