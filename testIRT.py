from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units

from Configurables import ApplicationMgr, EICDataSvc, PodioOutput, GeoSvc   
from Configurables import PodioInput
from Configurables import Jug__PID__IRTAlgorithm as IRTAlgorithm 

# Well, for now only need eRICH geometry for this example;
geo_service = GeoSvc("GeoSvc", detectors=["erich_only.xml"])

# Input file after 'npsim' pass;
podioevent = EICDataSvc("EventDataSvc", inputs=["erich-data.root"])

# S13660-3050AE-08 SiPM quantum efficiency [(wavelength [nm], q.e.)]
# Note: this is consistent with S13361-3050AE-08 (for eRICH)
qe_data = [
    (325, 0.04),
    (340, 0.10),
    (350, 0.20),
    (370, 0.30),
    (400, 0.35),
    (450, 0.40),
    (500, 0.38),
    (550, 0.35),
    (600, 0.27),
    (650, 0.20),
    (700, 0.15),
    (750, 0.12),
    (800, 0.08),
    (850, 0.06),
    (900, 0.04)
]

podioinput = PodioInput(
        "PodioReader",
        # Input collections: MC truth tracks and eRICH raw hits (photons);
        collections=["mcparticles", "ERICHHits"],
        OutputLevel=DEBUG
        )

irtrec = IRTAlgorithm(
        # Input collections: MC truth tracks and eRICH raw hits (photons);
        inputMCParticles="mcparticles",
        inputHitCollection="ERICHHits",

        # Output collection: eRICH PID decisions; 
        outputCherenkovPID="ERICHPID",

        # SiPM QE and geometric efficiency; FIXME: units.eV coefficient gives extra x1000 (?);
        QEcurve=[ ((1239.84/a), b) for a, b in qe_data ],
        GeometricEfficiency="0.85",
        # Rebin the QE in that many equidistant bins;
        QEbins="100",

        # eRICH optics configuration produced by ERich_geo.cpp code along with the dd4hep XML file;
        ConfigFile="erich-config.root"
        )

# Output ROOT file; keep the input collections as well, append eRICH PID tables;
out = PodioOutput("out", filename="erich-reco.root")
out.outputCommands = ["keep *"]

ApplicationMgr(
        TopAlg = [podioinput, irtrec, out],
        EvtSel = 'NONE',
        # Process that many events;
        EvtMax = 100,
        ExtSvc = [podioevent],
        OutputLevel = DEBUG,
        PluginDebugLevel = 2
        )

