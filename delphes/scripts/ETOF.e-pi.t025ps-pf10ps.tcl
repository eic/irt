module IdentificationMap ETOF {
  set InputArray TrackMerger/tracks
  set OutputArray tracks

    add EfficiencyFormula {-11} {-11} {
      (eta< -3.50 || eta>= -1.60 || pt * cosh(eta) <    0.20 || pt * cosh(eta) >=    2.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.959622) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.786432) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.674203) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.614404) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.960081) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.787134) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.674697) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.614742) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.961053) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.788638) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.675759) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.615468) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.963086) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.791865) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.678046) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.617036) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.967230) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.798818) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.683025) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.620455)
    }

    add EfficiencyFormula {-11} {211} {
      (eta< -3.50 || eta>= -1.60 || pt * cosh(eta) <    0.20 || pt * cosh(eta) >=    2.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.040378) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.213568) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.325797) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.385596) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.039919) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.212866) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.325303) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.385258) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.038947) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.211362) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.324241) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.384532) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.036914) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.208135) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.321954) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.382964) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.032770) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.201182) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.316975) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.379545)
    }

    add EfficiencyFormula {211} {-11} {
      (eta< -3.50 || eta>= -1.60 || pt * cosh(eta) <    0.20 || pt * cosh(eta) >=    2.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.040378) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.213568) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.325797) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.385596) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.039919) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.212866) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.325303) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.385258) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.038947) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.211362) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.324241) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.384532) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.036914) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.208135) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.321954) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.382964) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.032770) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.201182) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.316975) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.379545)
    }

    add EfficiencyFormula {211} {211} {
      (eta< -3.50 || eta>= -1.60 || pt * cosh(eta) <    0.20 || pt * cosh(eta) >=    2.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.959622) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.786432) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.674203) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.614404) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.960081) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.787134) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.674697) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.614742) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.961053) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.788638) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.675759) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.615468) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.963086) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.791865) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.678046) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.617036) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.967230) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.798818) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.683025) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.620455)
    }

  add EfficiencyFormula {0} {0} { 0.00 }
}
