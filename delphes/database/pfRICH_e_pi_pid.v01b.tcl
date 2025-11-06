module IdentificationMap pfRICH {
  set InputArray TrackMerger/tracks
  set OutputArray tracks

    add EfficiencyFormula {11} {11} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.40 || pt * cosh(eta) >=    5.50) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.999307) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.999417) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.977377) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.905071) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.824029) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.761750) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.712463) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.673177) +
      ( -3.50 <= eta && eta <  -2.80) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.644137) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.998847) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.999532) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.976676) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.910607) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.830012) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.768807) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.717503) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.675275) +
      ( -2.80 <= eta && eta <  -1.90) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.645911) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.999281) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.999533) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.981260) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.916096) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.834773) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.773196) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.717491) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.678464) +
      ( -1.90 <= eta && eta <  -1.50) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.649592)
    }

    add EfficiencyFormula {11} {211} {
      (eta< -3.50 || eta>= -1.50 || pt * cosh(eta) <    0.40 || pt * cosh(eta) >=    5.50) * ( 0.00 ) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000693) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.000583) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.022623) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.094929) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.175971) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.238250) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.287537) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.326823) +
      ( -3.50 <= eta && eta <  -2.80) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.355863) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.001153) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.000468) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.023324) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.089393) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.169988) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.231193) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.282497) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.324725) +
      ( -2.80 <= eta && eta <  -1.90) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.354089) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000719) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.000467) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.018740) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.083904) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.165227) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.226804) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.282509) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.321536) +
      ( -1.90 <= eta && eta <  -1.50) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.350408)
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
      ( -3.50 <= eta && eta <  -2.80) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.000374) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.022623) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.089443) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.166922) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.236866) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.281323) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.329845) +
      ( -3.50 <= eta && eta <  -2.80) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.357597) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.000318) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.021305) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.086640) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.160689) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.231193) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.283753) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.325768) +
      ( -2.80 <= eta && eta <  -1.90) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.355864) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (0.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.000344) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.020651) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.088040) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.162106) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.228283) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.279955) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.319340) +
      ( -1.90 <= eta && eta <  -1.50) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.348499)
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
      ( -3.50 <= eta && eta <  -2.80) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (1.000000) +
      ( -3.50 <= eta && eta <  -2.80) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.999626) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.977377) +
      ( -3.50 <= eta && eta <  -2.80) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.910557) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.833078) +
      ( -3.50 <= eta && eta <  -2.80) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.763134) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.718677) +
      ( -3.50 <= eta && eta <  -2.80) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.670155) +
      ( -3.50 <= eta && eta <  -2.80) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.642403) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (1.000000) +
      ( -2.80 <= eta && eta <  -1.90) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.999682) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.978695) +
      ( -2.80 <= eta && eta <  -1.90) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.913360) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.839311) +
      ( -2.80 <= eta && eta <  -1.90) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.768807) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.716247) +
      ( -2.80 <= eta && eta <  -1.90) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.674232) +
      ( -2.80 <= eta && eta <  -1.90) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.644136) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.40 <= pt * cosh(eta) && pt * cosh(eta) <    0.45) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.45 <= pt * cosh(eta) && pt * cosh(eta) <    0.50) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.50 <= pt * cosh(eta) && pt * cosh(eta) <    0.55) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.55 <= pt * cosh(eta) && pt * cosh(eta) <    0.60) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.60 <= pt * cosh(eta) && pt * cosh(eta) <    0.70) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.70 <= pt * cosh(eta) && pt * cosh(eta) <    0.80) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.80 <= pt * cosh(eta) && pt * cosh(eta) <    0.90) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   0.90 <= pt * cosh(eta) && pt * cosh(eta) <    1.00) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.00 <= pt * cosh(eta) && pt * cosh(eta) <    1.50) * (1.000000) +
      ( -1.90 <= eta && eta <  -1.50) * (   1.50 <= pt * cosh(eta) && pt * cosh(eta) <    2.00) * (0.999656) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.00 <= pt * cosh(eta) && pt * cosh(eta) <    2.50) * (0.979349) +
      ( -1.90 <= eta && eta <  -1.50) * (   2.50 <= pt * cosh(eta) && pt * cosh(eta) <    3.00) * (0.911960) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.00 <= pt * cosh(eta) && pt * cosh(eta) <    3.50) * (0.837894) +
      ( -1.90 <= eta && eta <  -1.50) * (   3.50 <= pt * cosh(eta) && pt * cosh(eta) <    4.00) * (0.771717) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.00 <= pt * cosh(eta) && pt * cosh(eta) <    4.50) * (0.720045) +
      ( -1.90 <= eta && eta <  -1.50) * (   4.50 <= pt * cosh(eta) && pt * cosh(eta) <    5.00) * (0.680660) +
      ( -1.90 <= eta && eta <  -1.50) * (   5.00 <= pt * cosh(eta) && pt * cosh(eta) <    5.50) * (0.651501)
    }

  add EfficiencyFormula {0} {0} { 0.00 }
}
