module IdentificationMap pfRICH {
  set InputArray TrackMerger/tracks
  set OutputArray tracks

    add EfficiencyFormula {11} {11} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.05 || pt * cosh(eta) >=    5.50) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.05 <= pt * cosh(eta) && pt * cosh(eta) <    0.10) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.10 <= pt * cosh(eta) && pt * cosh(eta) <    0.15) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.15 <= pt * cosh(eta) && pt * cosh(eta) <    0.20) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.25) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.25 <= pt * cosh(eta) && pt * cosh(eta) <    0.30) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.30 <= pt * cosh(eta) && pt * cosh(eta) <    0.35) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.35 <= pt * cosh(eta) && pt * cosh(eta) <    0.40) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.999994) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.995244) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.956733) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.880732) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.805326) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.748109) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.702808) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.666449) +
      ( -3.50 <= eta && eta <  -2.80) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.639269) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.05 <= pt * cosh(eta) && pt * cosh(eta) <    0.10) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.10 <= pt * cosh(eta) && pt * cosh(eta) <    0.15) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.15 <= pt * cosh(eta) && pt * cosh(eta) <    0.20) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.25) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.25 <= pt * cosh(eta) && pt * cosh(eta) <    0.30) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.30 <= pt * cosh(eta) && pt * cosh(eta) <    0.35) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.35 <= pt * cosh(eta) && pt * cosh(eta) <    0.40) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.999999) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.997324) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.962280) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.892478) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.815694) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.758031) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.709886) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.670105) +
      ( -2.80 <= eta && eta <  -1.90) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.642139) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.05 <= pt * cosh(eta) && pt * cosh(eta) <    0.10) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.10 <= pt * cosh(eta) && pt * cosh(eta) <    0.15) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.15 <= pt * cosh(eta) && pt * cosh(eta) <    0.20) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.25) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.25 <= pt * cosh(eta) && pt * cosh(eta) <    0.30) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.30 <= pt * cosh(eta) && pt * cosh(eta) <    0.35) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.35 <= pt * cosh(eta) && pt * cosh(eta) <    0.40) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.998315) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.972088) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.903469) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.824676) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.765578) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.712375) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.674821) +
      ( -1.90 <= eta && eta <  -1.50) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.646890)
    }

    add EfficiencyFormula {11} {211} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.05 || pt * cosh(eta) >=    5.50) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.05 <= pt * cosh(eta) && pt * cosh(eta) <    0.10) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.10 <= pt * cosh(eta) && pt * cosh(eta) <    0.15) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.15 <= pt * cosh(eta) && pt * cosh(eta) <    0.20) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.25) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.25 <= pt * cosh(eta) && pt * cosh(eta) <    0.30) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.30 <= pt * cosh(eta) && pt * cosh(eta) <    0.35) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.35 <= pt * cosh(eta) && pt * cosh(eta) <    0.40) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000006) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.004756) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.043267) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.119268) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.194674) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.251891) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.297192) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.333551) +
      ( -3.50 <= eta && eta <  -2.80) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.360731) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.05 <= pt * cosh(eta) && pt * cosh(eta) <    0.10) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.10 <= pt * cosh(eta) && pt * cosh(eta) <    0.15) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.15 <= pt * cosh(eta) && pt * cosh(eta) <    0.20) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.25) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.25 <= pt * cosh(eta) && pt * cosh(eta) <    0.30) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.30 <= pt * cosh(eta) && pt * cosh(eta) <    0.35) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.35 <= pt * cosh(eta) && pt * cosh(eta) <    0.40) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000001) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.002676) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.037720) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.107522) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.184306) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.241969) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.290114) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.329895) +
      ( -2.80 <= eta && eta <  -1.90) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.357861) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.05 <= pt * cosh(eta) && pt * cosh(eta) <    0.10) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.10 <= pt * cosh(eta) && pt * cosh(eta) <    0.15) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.15 <= pt * cosh(eta) && pt * cosh(eta) <    0.20) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.25) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.25 <= pt * cosh(eta) && pt * cosh(eta) <    0.30) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.30 <= pt * cosh(eta) && pt * cosh(eta) <    0.35) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.35 <= pt * cosh(eta) && pt * cosh(eta) <    0.40) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.001685) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.027912) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.096531) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.175324) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.234422) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.287625) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.325179) +
      ( -1.90 <= eta && eta <  -1.50) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.353110)
    }

    add EfficiencyFormula {211} {11} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.05 || pt * cosh(eta) >=    5.50) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.05 <= pt * cosh(eta) && pt * cosh(eta) <    0.10) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.10 <= pt * cosh(eta) && pt * cosh(eta) <    0.15) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.15 <= pt * cosh(eta) && pt * cosh(eta) <    0.20) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.25) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.25 <= pt * cosh(eta) && pt * cosh(eta) <    0.30) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.30 <= pt * cosh(eta) && pt * cosh(eta) <    0.35) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.35 <= pt * cosh(eta) && pt * cosh(eta) <    0.40) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000003) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.003967) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.043267) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.114567) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.187086) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.250711) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.291806) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.336240) +
      ( -3.50 <= eta && eta <  -2.80) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.362297) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.05 <= pt * cosh(eta) && pt * cosh(eta) <    0.10) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.10 <= pt * cosh(eta) && pt * cosh(eta) <    0.15) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.15 <= pt * cosh(eta) && pt * cosh(eta) <    0.20) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.25) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.25 <= pt * cosh(eta) && pt * cosh(eta) <    0.30) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.30 <= pt * cosh(eta) && pt * cosh(eta) <    0.35) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.35 <= pt * cosh(eta) && pt * cosh(eta) <    0.40) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.002201) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.035557) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.105050) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.176144) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.241969) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.291244) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.330848) +
      ( -2.80 <= eta && eta <  -1.90) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.359503) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.05 <= pt * cosh(eta) && pt * cosh(eta) <    0.10) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.10 <= pt * cosh(eta) && pt * cosh(eta) <    0.15) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.15 <= pt * cosh(eta) && pt * cosh(eta) <    0.20) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.25) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.25 <= pt * cosh(eta) && pt * cosh(eta) <    0.30) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.30 <= pt * cosh(eta) && pt * cosh(eta) <    0.35) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.35 <= pt * cosh(eta) && pt * cosh(eta) <    0.40) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.001393) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.029973) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.100385) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.172467) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.235783) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.285248) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.323115) +
      ( -1.90 <= eta && eta <  -1.50) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.351303)
    }

    add EfficiencyFormula {211} {211} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.05 || pt * cosh(eta) >=    5.50) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.05 <= pt * cosh(eta) && pt * cosh(eta) <    0.10) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.10 <= pt * cosh(eta) && pt * cosh(eta) <    0.15) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.15 <= pt * cosh(eta) && pt * cosh(eta) <    0.20) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.25) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.25 <= pt * cosh(eta) && pt * cosh(eta) <    0.30) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.30 <= pt * cosh(eta) && pt * cosh(eta) <    0.35) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.35 <= pt * cosh(eta) && pt * cosh(eta) <    0.40) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.999997) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.996033) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.956733) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.885433) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.812914) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.749289) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.708194) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.663760) +
      ( -3.50 <= eta && eta <  -2.80) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.637703) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.05 <= pt * cosh(eta) && pt * cosh(eta) <    0.10) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.10 <= pt * cosh(eta) && pt * cosh(eta) <    0.15) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.15 <= pt * cosh(eta) && pt * cosh(eta) <    0.20) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.25) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.25 <= pt * cosh(eta) && pt * cosh(eta) <    0.30) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.30 <= pt * cosh(eta) && pt * cosh(eta) <    0.35) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.35 <= pt * cosh(eta) && pt * cosh(eta) <    0.40) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.997799) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.964443) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.894950) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.823856) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.758031) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.708756) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.669152) +
      ( -2.80 <= eta && eta <  -1.90) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.640497) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.05 <= pt * cosh(eta) && pt * cosh(eta) <    0.10) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.10 <= pt * cosh(eta) && pt * cosh(eta) <    0.15) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.15 <= pt * cosh(eta) && pt * cosh(eta) <    0.20) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.25) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.25 <= pt * cosh(eta) && pt * cosh(eta) <    0.30) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.30 <= pt * cosh(eta) && pt * cosh(eta) <    0.35) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.35 <= pt * cosh(eta) && pt * cosh(eta) <    0.40) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.998607) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.970027) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.899615) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.827533) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.764217) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.714752) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.676885) +
      ( -1.90 <= eta && eta <  -1.50) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.648697)
    }

  add EfficiencyFormula {0} {0} { 0.00 }
}