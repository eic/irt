module IdentificationMap pfRICH {
  set InputArray TrackMerger/tracks
  set OutputArray tracks

    add EfficiencyFormula {11} {11} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.40 || pt * cosh(eta) >=    5.50) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.999955) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.991449) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.946267) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.869975) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.797208) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.742194) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.698612) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.663522) +
      ( -3.50 <= eta && eta <  -2.80) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.637159) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.999911) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.990092) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.941162) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.870021) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.798658) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.745484) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.701168) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.664303) +
      ( -2.80 <= eta && eta <  -1.90) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.638027) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.999993) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.999027) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.976656) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.921009) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.849730) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.783416) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.734606) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.691330) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.659819) +
      ( -1.90 <= eta && eta <  -1.50) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.635836)
    }

    add EfficiencyFormula {11} {211} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.40 || pt * cosh(eta) >=    5.50) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000045) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.008551) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.053733) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.130025) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.202792) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.257806) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.301388) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.336478) +
      ( -3.50 <= eta && eta <  -2.80) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.362841) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000089) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.009908) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.058838) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.129979) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.201342) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.254516) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.298832) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.335697) +
      ( -2.80 <= eta && eta <  -1.90) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.361973) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000007) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000973) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.023344) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.078991) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.150270) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.216584) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.265394) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.308670) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.340181) +
      ( -1.90 <= eta && eta <  -1.50) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.364164)
    }

    add EfficiencyFormula {211} {11} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.40 || pt * cosh(eta) >=    5.50) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000032) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.007502) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.053733) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.125669) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.195789) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.256708) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.296337) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.339030) +
      ( -3.50 <= eta && eta <  -2.80) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.364338) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000067) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.008987) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.056723) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.127859) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.194421) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.254516) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.299828) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.336557) +
      ( -2.80 <= eta && eta <  -1.90) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.363478) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000004) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000815) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.022371) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.080878) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.152919) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.214665) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.266337) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.306935) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.338609) +
      ( -1.90 <= eta && eta <  -1.50) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.362739)
    }

    add EfficiencyFormula {211} {211} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.40 || pt * cosh(eta) >=    5.50) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.999968) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.992498) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.946267) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.874331) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.804211) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.743292) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.703663) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.660970) +
      ( -3.50 <= eta && eta <  -2.80) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.635662) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.999933) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.991013) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.943277) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.872141) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.805579) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.745484) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.700172) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.663443) +
      ( -2.80 <= eta && eta <  -1.90) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.636522) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.999996) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.999185) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.977629) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.919122) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.847081) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.785335) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.733663) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.693065) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.661391) +
      ( -1.90 <= eta && eta <  -1.50) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.637261)
    }

  add EfficiencyFormula {0} {0} { 0.00 }
}
