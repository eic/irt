
// 
// ! Make sure libDELPHES.so is in LD_LIBRARY_PATH
//
// root -l reader.C
//

void reader( void )
{
  auto ff = new TFile("pfRICH.root");
  auto dconfig = dynamic_cast<DelphesConfig*>(ff->Get("DelphesConfigRICH"));
  ff->Close();

  auto hypo = dconfig->GetMassHypothesis(211);
  printf("%d %f\n", hypo->PdgCode(), hypo->Mass()); 

  unsigned hdim = dconfig->GetMassHypothesisCount();

  printf("%u; eta range %7.2f .. %7.2f\n", hdim, dconfig->GetEtaMin(), dconfig->GetEtaMax());
  
  {
    int harr[hdim];

    dconfig->GetMassHypotheses(harr);
    for(unsigned ih=0; ih<hdim; ih++)
      printf("%4d\n", harr[ih]);
  }

  {
    double hmtx[hdim*hdim];

    // This should also work;
    //TVector3 p(0.0, 1.0, -10.); 
    //int ret = dconfig->GetSmearingMatrix(p, hmtx);
    int ret = dconfig->GetSmearingMatrix(-2.0, 13.0, hmtx);
    for(unsigned i0=0; i0<hdim; i0++) {
      for(unsigned i1=0; i1<hdim; i1++) 
	printf("%8.4f ", hmtx[i0*hdim + i1]);
      
      printf("\n");
    } //for i0
  }
} // reader()
