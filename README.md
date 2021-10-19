Indirect Ray Tracing code for ATHENA event reconstruction
=========================================================

  A C++ ROOT-based software library to peform Cherenkov photon ray 
tracing between a loosely defined emission point (which is typically 
unknown in the experiment) and a detection point on a photosensor
matrix, in a configuration with a pre-defined sequence of refractive 
and reflective surfaces. Provides means to perform detailed microscopic 
simulations of Cherenkov imaging detectors in a pure GEANT4 environment, 
as well as an interface to the ATHENA software framework. Given a track 
parameterization along a charged particle trajectory in a sequence of 
Cherenkov radiators, and a collection of single photon hits, allows one 
to perform probabilistic analysis of particle mass hypotheses. 

  Primary application are proximity focusing and / or mirror-focusing RICH 
detectors, with a configurable combination of aerogel and / or gas radiators, 
as well as a set of spherical and / or flat mirrors.  

 Content:

 * [Introduction](#introduction)
 * [Prerequisites](#prerequisites)
 * [Installation](#installation)
 * [eRICH example configuration](#erich-example-configuration)
 * [Simulation pass](#simulation-pass)
 * [Reconstruction pass](#reconstruction-pass)

<br/>

Introduction
------------

  A typical event reconstruction task in a setup with an imaging Cherenkov 
detector is to associate single photon hits with the charged particle tracks, 
and evaluate the Cherenkov light emission angle, which - provided particle 
momentum is evaluated by other means (for instance via tracking) - gives one a probabilistic 
estimate of a particle mass and therefore allows one to e.g. separate charged
pions and kaon in data analysis.

  Current implementation uses MC truth information to associate photon hits and 
tracks, and does not make an attempt to perform either ring finding or noise 
hit suppression in multi-track configurations. Nowehere in the algorithmic
part it tries to reconstruct either average Cherenkov ring radius or average 
emission angle. Instead of that the algorithm just makes an attempt to associate 
*any* detected photon hit with any track, and perform *mass hypothesis ranking* for 
each track. This procedure is supposed to automatically handle random noise, and 
in a typically low track multiplicity environment at the EIC would only require 
a shared hit resolution procedure in addition, to work in an optimal way in case 
of overlapping rings. Well, this needs to be verified of course.

  The CPU overhead is supposed to scale as [N tracks] x [M photon hits], and in 
general can be substantial. Further optimization like detector partitioning in 
independent sectors will be required.

  Output provides estimated photon count and estimated weights for a set of  mass 
hytheses requested by a user.

<br/>

Prerequisites
-------------

  It is assumed that a user ia familiar with the [ATHENA software](https://eic.phy.anl.gov/ip6) 
environment, and a juggler singularity container "jug_xl" is running. It is also assumed 
that dd4hep sources are available either via cvmfs or locally, and athena detector software 
is installed. In the following /tmp/ATHENA is supposed to be a scratch directory. For the sake of 
completeness, the following sequence of commands installs all what is needed under /tmp/ATHENA:

```
mkdir -p /tmp/ATHENA && cd /tmp/ATHENA
git clone https://eicweb.phy.anl.gov/EIC/detectors/athena.git
cd athena && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/tmp/ATHENA ..
make -j2 install

cd /tmp/ATHENA
git clone https://eicweb.phy.anl.gov/EIC/detectors/ip6.git
cd ip6 && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/tmp/ATHENA ..
make -j2 install
```

<br/>

Installation
------------

```
cd /tmp/ATHENA
git clone https://eicweb.phy.anl.gov/EIC/irt.git
cd irt && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/tmp/ATHENA -DREADER=YES ..
make -j2 install
```

  This compiles the IRT library codes and the executable to be later on used to read the .root
files with the GEANT hits after *npsim* simulation pass. 

  The rest of this README builds a minimalistic self-contained example of how to make use of the 
IRT library in application to a basic ATHENA e-endcap proximity focusing aerogel RICH.

<br/>

eRICH example configuration
---------------------------

  See [ERich_geo.cpp](erich/src/ERich_geo.cpp) for a simple API example, in particular the calls
which define gas volume and aerogel radiators, as well as the photosensor geometry.

  The following will compile libathena.so plugin with eRICH detector only:

```
export LD_LIBRARY_PATH=/tmp/ATHENA/lib:${LD_LIBRARY_PATH}
cd /tmp/ATHENA/irt/erich && mkdir -p build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/tmp/ATHENA -DIRT=/tmp/ATHENA ..
make install
```

<br/>

Simulation pass
---------------

  Create a separate sandbox directory. Generate a minimal set of necessary links. Run *npsim*.

```
cd /tmp/ATHENA && mkdir sandbox && cd sandbox

# Links to the directories with the "official" .xml files (depend on your installation);
ln -s /tmp/ATHENA/share/ip6/ip6 .
ln -s /tmp/ATHENA/share/athena/compact .

# These two just to simplify 'npsim' command line:
ln -s /tmp/ATHENA/share/athena/compact/erich.xml .
ln -s /tmp/ATHENA/share/athena/compact/subsystem_views/erich_only.xml .

# Eventually run 'npsim' for 100 events with 8 GeV pions, in a eRICH-only geometry;;
npsim --compactFile=./erich_only.xml --runType=run -G -N=100 --outputFile=./erich-data.root --gun.position "0.0 0.0 0.0" --gun.direction "0.2 0.0 -1.0" --gun.energy 8*GeV --gun.particle="pi+" --part.userParticleHandler=''
```

  A pair of ROOT output files is produced: eRICH detector optics configuration and 
a file with GEANT tracks and photon hits.

<br/>

Reconstruction pass
-------------------

  The following command executed in the same sandbox directory will run through the produced 100 events,
and evaluate pion vs kaon hypothesis for the primary pions. See [reader.cc](reader/reader.cc)
source code for more details.
  
```
/tmp/ATHENA/bin/reader erich-data.root erich-config.root
```