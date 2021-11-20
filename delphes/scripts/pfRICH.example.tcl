module IdentificationMap pfRICH {
  set InputArray TrackMerger/tracks
  set OutputArray tracks

    add EfficiencyFormula {211} {211} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.90 || pt * cosh(eta) >=   11.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.00) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (1.000000) +
      ( -3.50 <= eta && eta <  -3.00) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.995807) +
      ( -3.00 <= eta && eta <  -1.70) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (1.000000) +
      ( -3.00 <= eta && eta <  -1.70) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.995807) +
      ( -1.70 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (1.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (1.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.995807)
    }

    add EfficiencyFormula {211} {321} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.90 || pt * cosh(eta) >=   11.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.00) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -3.50 <= eta && eta <  -3.00) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.004193) +
      ( -3.00 <= eta && eta <  -1.70) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -3.00 <= eta && eta <  -1.70) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.004193) +
      ( -1.70 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.004193)
    }

    add EfficiencyFormula {321} {211} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.90 || pt * cosh(eta) >=   11.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.00) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -3.50 <= eta && eta <  -3.00) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.004193) +
      ( -3.00 <= eta && eta <  -1.70) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -3.00 <= eta && eta <  -1.70) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.004193) +
      ( -1.70 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.004193)
    }

    add EfficiencyFormula {321} {321} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.90 || pt * cosh(eta) >=   11.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.00) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (1.000000) +
      ( -3.50 <= eta && eta <  -3.00) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.995807) +
      ( -3.00 <= eta && eta <  -1.70) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (1.000000) +
      ( -3.00 <= eta && eta <  -1.70) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.995807) +
      ( -1.70 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (1.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (1.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.995807)
    }

    add EfficiencyFormula {321} {2212} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.90 || pt * cosh(eta) >=   11.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.00) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -3.50 <= eta && eta <  -3.00) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.000001) +
      ( -3.00 <= eta && eta <  -1.70) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -3.00 <= eta && eta <  -1.70) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.000001) +
      ( -1.70 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.000001)
    }

    add EfficiencyFormula {2212} {321} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.90 || pt * cosh(eta) >=   11.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.00) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -3.50 <= eta && eta <  -3.00) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.000000) +
      ( -3.00 <= eta && eta <  -1.70) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -3.00 <= eta && eta <  -1.70) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (0.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (0.000000)
    }

    add EfficiencyFormula {2212} {2212} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.90 || pt * cosh(eta) >=   11.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.00) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (1.000000) +
      ( -3.50 <= eta && eta <  -3.00) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (1.000000) +
      ( -3.00 <= eta && eta <  -1.70) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (1.000000) +
      ( -3.00 <= eta && eta <  -1.70) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (1.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (1.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <   10.00) * (1.000000) +
      ( -1.70 <= eta && eta <  -1.50) * (  10.00 <= pt * cosh(eta) && pt * cosh(eta) <   11.00) * (1.000000)
    }

  add EfficiencyFormula {0} {0} { 0.00 }
}
