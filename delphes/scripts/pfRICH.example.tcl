module IdentificationMap pfRICH {
  set InputArray TrackMerger/tracks
  set OutputArray tracks

    add EfficiencyFormula {211} {211} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    8.00 || pt * cosh(eta) >=   11.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.00) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.977341) +
      ( -3.50 <= eta && eta <  -3.00) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.968842) +
      ( -3.00 <= eta && eta <  -1.70) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.989482) +
      ( -3.00 <= eta && eta <  -1.70) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.968842) +
      ( -1.70 <= eta && eta <  -1.50) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <    9.00) * (0.995251) +
      ( -1.70 <= eta && eta <  -1.50) * (   9.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.980599) +
      ( -1.70 <= eta && eta <  -1.50) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.968842)
    }

    add EfficiencyFormula {211} {321} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    8.00 || pt * cosh(eta) >=   11.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.00) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.022659) +
      ( -3.50 <= eta && eta <  -3.00) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.031158) +
      ( -3.00 <= eta && eta <  -1.70) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.010518) +
      ( -3.00 <= eta && eta <  -1.70) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.031158) +
      ( -1.70 <= eta && eta <  -1.50) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <    9.00) * (0.004749) +
      ( -1.70 <= eta && eta <  -1.50) * (   9.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.019401) +
      ( -1.70 <= eta && eta <  -1.50) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.031158)
    }

    add EfficiencyFormula {321} {211} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    8.00 || pt * cosh(eta) >=   11.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.00) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.022659) +
      ( -3.50 <= eta && eta <  -3.00) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.031158) +
      ( -3.00 <= eta && eta <  -1.70) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.007666) +
      ( -3.00 <= eta && eta <  -1.70) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.031158) +
      ( -1.70 <= eta && eta <  -1.50) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <    9.00) * (0.003213) +
      ( -1.70 <= eta && eta <  -1.50) * (   9.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.014961) +
      ( -1.70 <= eta && eta <  -1.50) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.031158)
    }

    add EfficiencyFormula {321} {321} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    8.00 || pt * cosh(eta) >=   11.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.00) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.977341) +
      ( -3.50 <= eta && eta <  -3.00) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.968831) +
      ( -3.00 <= eta && eta <  -1.70) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.992334) +
      ( -3.00 <= eta && eta <  -1.70) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.968831) +
      ( -1.70 <= eta && eta <  -1.50) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <    9.00) * (0.996787) +
      ( -1.70 <= eta && eta <  -1.50) * (   9.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.985039) +
      ( -1.70 <= eta && eta <  -1.50) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.968831)
    }

    add EfficiencyFormula {321} {2212} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    8.00 || pt * cosh(eta) >=   11.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.00) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -3.50 <= eta && eta <  -3.00) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.000010) +
      ( -3.00 <= eta && eta <  -1.70) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -3.00 <= eta && eta <  -1.70) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.000010) +
      ( -1.70 <= eta && eta <  -1.50) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <    9.00) * (0.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (   9.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.000010)
    }

    add EfficiencyFormula {2212} {321} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    8.00 || pt * cosh(eta) >=   11.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.00) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -3.50 <= eta && eta <  -3.00) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.000000) +
      ( -3.00 <= eta && eta <  -1.70) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -3.00 <= eta && eta <  -1.70) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <    9.00) * (0.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (   9.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.000000)
    }

    add EfficiencyFormula {2212} {2212} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    8.00 || pt * cosh(eta) >=   11.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.00) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (1.000000) +
      ( -3.50 <= eta && eta <  -3.00) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (1.000000) +
      ( -3.00 <= eta && eta <  -1.70) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (1.000000) +
      ( -3.00 <= eta && eta <  -1.70) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (1.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (   8.00 <= pt * cosh(eta) && pt * cosh(eta) <    9.00) * (1.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (   9.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (1.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (1.000000)
    }

  add EfficiencyFormula {0} {0} { 0.00 }
}
