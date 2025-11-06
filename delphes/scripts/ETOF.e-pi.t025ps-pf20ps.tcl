module IdentificationMap ETOF {
  set InputArray TrackMerger/tracks
  set OutputArray tracks

    add EfficiencyFormula {-11} {-11} {
      (eta< -3.50 || eta>= -1.60 || pt * cosh(eta) <    0.20 || pt * cosh(eta) >=    2.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.929483) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.748360) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.648252) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.596820) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.930085) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.749009) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.648681) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.597109) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.931366) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.750399) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.649603) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.597729) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.934071) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.753388) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.651591) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.599066) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.939696) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.759856) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.655923) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.601987)
    }

    add EfficiencyFormula {-11} {211} {
      (eta< -3.50 || eta>= -1.60 || pt * cosh(eta) <    0.20 || pt * cosh(eta) >=    2.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.070517) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.251640) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.351748) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.403180) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.069915) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.250991) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.351319) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.402891) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.068634) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.249601) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.350397) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.402271) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.065929) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.246612) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.348409) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.400934) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.060304) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.240144) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.344077) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.398013)
    }

    add EfficiencyFormula {211} {-11} {
      (eta< -3.50 || eta>= -1.60 || pt * cosh(eta) <    0.20 || pt * cosh(eta) >=    2.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.070517) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.251640) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.351748) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.403180) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.069915) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.250991) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.351319) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.402891) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.068634) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.249601) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.350397) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.402271) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.065929) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.246612) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.348409) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.400934) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (0.000000) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.060304) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.240144) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.344077) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.398013)
    }

    add EfficiencyFormula {211} {211} {
      (eta< -3.50 || eta>= -1.60 || pt * cosh(eta) <    0.20 || pt * cosh(eta) >=    2.00) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.929483) +
      ( -3.50 <= eta && eta <  -3.12) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.748360) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.648252) +
      ( -3.50 <= eta && eta <  -3.12) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.596820) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.930085) +
      ( -3.12 <= eta && eta <  -2.74) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.749009) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.648681) +
      ( -3.12 <= eta && eta <  -2.74) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.597109) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.931366) +
      ( -2.74 <= eta && eta <  -2.36) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.750399) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.649603) +
      ( -2.74 <= eta && eta <  -2.36) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.597729) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.934071) +
      ( -2.36 <= eta && eta <  -1.98) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.753388) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.651591) +
      ( -2.36 <= eta && eta <  -1.98) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.599066) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.20 <= pt * cosh(eta) && pt * cosh(eta) <    0.56) * (1.000000) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.56 <= pt * cosh(eta) && pt * cosh(eta) <    0.92) * (0.939696) +
      ( -1.98 <= eta && eta <  -1.60) * (   0.92 <= pt * cosh(eta) && pt * cosh(eta) <    1.28) * (0.759856) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.28 <= pt * cosh(eta) && pt * cosh(eta) <    1.64) * (0.655923) +
      ( -1.98 <= eta && eta <  -1.60) * (   1.64 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.601987)
    }

  add EfficiencyFormula {0} {0} { 0.00 }
}
