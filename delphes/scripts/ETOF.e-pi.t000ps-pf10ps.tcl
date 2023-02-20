module IdentificationMap ETOF {
  set InputArray TrackMerger/tracks
  set OutputArray tracks

    add EfficiencyFormula {-11} {-11} {
      (eta< -3.50 || eta>= -1.60 || pt * cosh(eta) <    0.20 || pt * cosh(eta) >=    2.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.999997) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.979514) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.877418) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.772918) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.999997) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.979820) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.878134) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.773603) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.999997) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.980462) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.879662) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.775072) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.999998) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.981791) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.882918) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.778227) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.999999) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.984430) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.889817) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.785034)
    }

    add EfficiencyFormula {-11} {211} {
      (eta< -3.50 || eta>= -1.60 || pt * cosh(eta) <    0.20 || pt * cosh(eta) >=    2.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.000003) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.020486) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.122582) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.227082) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.000003) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.020180) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.121866) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.226397) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.000003) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.019538) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.120338) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.224928) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.000002) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.018209) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.117082) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.221773) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.000001) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.015570) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.110183) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.214966)
    }

    add EfficiencyFormula {211} {-11} {
      (eta< -3.50 || eta>= -1.60 || pt * cosh(eta) <    0.20 || pt * cosh(eta) >=    2.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.000003) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.020486) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.122582) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.227082) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.000003) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.020180) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.121866) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.226397) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.000003) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.019538) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.120338) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.224928) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.000002) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.018209) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.117082) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.221773) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.000001) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.015570) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.110183) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.214966)
    }

    add EfficiencyFormula {211} {211} {
      (eta< -3.50 || eta>= -1.60 || pt * cosh(eta) <    0.20 || pt * cosh(eta) >=    2.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.999997) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.979514) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.877418) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.772918) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.999997) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.979820) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.878134) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.773603) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.999997) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.980462) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.879662) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.775072) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.999998) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.981791) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.882918) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.778227) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.999999) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.984430) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.889817) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.785034)
    }

  add EfficiencyFormula {0} {0} { 0.00 }
}
