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
 * [Prerequisites and installation](#prerequisites-and-installation)
 * [pfRICH example configuration](#pfrich-example-configuration)
 * [Simulation pass](#simulation-pass)
 * [No-juggler reconstruction pass](#no-juggler-reconstruction-pass)
 * [Juggler reconstruction pass](#juggler-reconstruction-pass)
 * [DRICH case](#drich-case)
 * [HEPMC writer](#hepmc-writer)

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

Prerequisites and installation
------------------------------

  It is assumed that a user ia familiar with the [ATHENA software](https://eic.phy.anl.gov/ip6) 
environment, and a juggler singularity container "jug_xl" is running. It is also assumed 
that dd4hep sources are available either via cvmfs or locally, and athena detector software 
is installed. For the sake of completeness, the following sequence of commands installs all what is needed 
under /tmp, assuming that eic-shell was just started, see [ATHENA software](https://eic.phy.anl.gov/ip6) 
for further details:

```
# In the following /tmp/ATHENA is supposed to be a *link* to a safe scratch directory
# <your-safe-scratch-area> somewhere on a (local) filesystem:

cd /tmp && ln -s <your-safe-scratch-area> ATHENA && cd ATHENA
```

  The rest of the commands in this README can be grabbed by mouse and executed 
block by block.

```
# Do not want to mess up with the initial software installation in the container;
unset ATHENA_PREFIX

export LD_LIBRARY_PATH=/tmp/ATHENA/lib:${LD_LIBRARY_PATH}

# Install a particular branch of the EIC data model;
cd /tmp/ATHENA
git clone https://eicweb.phy.anl.gov/EIC/eicd.git --branch irt-init-v01
cd eicd && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/tmp/ATHENA ..
make -j2 install
```

```
# Install the IRT library itself;
cd /tmp/ATHENA
git clone https://eicweb.phy.anl.gov/EIC/irt.git --branch irt-init-v01
cd irt && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/tmp/ATHENA -DEVALUATION=YES ..
make -j2 install
```

  This compiles the IRT library codes and the executables to be later on used to read the .root
files with the GEANT hits after *npsim* simulation pass. 

```
# Install "athena" detector description;
git clone https://eicweb.phy.anl.gov/EIC/detectors/athena.git --branch irt-init-v01
cd athena && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/tmp/ATHENA ..
make -j2 install

# Install "ip6" description;
cd /tmp/ATHENA
git clone https://eicweb.phy.anl.gov/EIC/detectors/ip6.git
cd ip6 && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/tmp/ATHENA ..
make -j2 install

```

  The rest of this README builds a minimalistic self-contained example of how to make use of the 
IRT library in application to a basic ATHENA e-endcap proximity focusing aerogel RICH.


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
ln -s /tmp/ATHENA/share/athena/compact/pfrich.xml .
ln -s /tmp/ATHENA/share/athena/compact/subsystem_views/pfrich_only.xml .

# Eventually run 'npsim' for 100 events with 8 GeV pions, in a pfRICH-only geometry;
npsim --compactFile=./pfrich_only.xml --runType=run -G -N=100 --outputFile=./pfrich-data.root --gun.position "0.0 0.0 0.0" --gun.direction "0.2 0.0 -1.0" --gun.energy 8*GeV --gun.particle="pi+" --part.userParticleHandler='' --random.seed 0x12345678 --random.enableEventSeed

```

  A pair of ROOT output files is produced: pfRICH detector optics configuration and 
a file with GEANT tracks and photon hits.

<br/>

No-juggler reconstruction pass
------------------------------

  A simplified executable, using the same IRT engine, but with hardcoded (optional) QE 
and low wavelength cutoff.

```
cd /tmp/ATHENA/sandbox
# Loop through the events in the raw GEANT4 hit file. See [reader.cc](evaluation/reader.cc)
/tmp/ATHENA/bin/reader pfrich-data.root pfrich-config.root

```

Juggler reconstruction pass
---------------------------

  Install Juggler first:

```
cd /tmp/ATHENA
git clone https://eicweb.phy.anl.gov/EIC/juggler.git --branch irt-init-v01
cd juggler && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/tmp/ATHENA ..

# Fix an issue with LD_LIBRARY_PATH in jugglerenv.sh; may be required more than once (?);
sed -i.bak 's/\:\/usr\/local\/lib\:/\:/g' jugglerenv.sh && echo "export LD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:/usr/local/lib && export PYTHONPATH=\${PYTHONPATH}:/usr/local/lib" >> jugglerenv.sh
# Compile with a single thread unless have a plenty of memory;
make -j1 install

```

```
cd /tmp/ATHENA/sandbox
# Run Juggler with a simplified pfrich-testIRT.py options file provided with IRT distribution; 
xenv -x ../Juggler.xenv gaudirun.py ../irt/pfrich-testIRT.py

# Loop through the events in the reconstructed file. See [evaluation.cc](evaluation/evaluation.cc)
/tmp/ATHENA/bin/evaluation pfrich-reco.root

```

DRICH case
----------

It is assumed that 'athena/ip6/compact' links in /tmp/ATHENA/sandbox directory are created already.
The rest is pretty much similar to the pfRICH case, except for perhaps a .C script usage instead 
of a .cc executable:

```
cd /tmp/ATHENA/sandbox
ln -s /tmp/ATHENA/share/athena/compact/drich.xml .
ln -s /tmp/ATHENA/share/athena/compact/subsystem_views/drich_only.xml .
```

```
npsim --compactFile=./drich_only.xml --runType=run -G -N=500 --outputFile=./drich-data.root --gun.position "0.0 0.0 0.0" --gun.direction "0.27 0.0 1.0" --gun.energy 12*GeV --gun.particle="pi+" --part.userParticleHandler='' --random.seed 0x12345678 --random.enableEventSeed
```
```
xenv -x ../Juggler.xenv gaudirun.py ../irt/drich-testIRT.py
root -l '../irt/scripts/evaluation.C("drich-reco.root")'
```

HEPMC writer
------------

It is of course way more convenient to create a .hepmc file with a collection of events / tracks, 
than to use a limited in functionality npsim command line interface. Here is an example:

```
cd /tmp/ATHENA/sandbox
root -l '../irt/scripts/drich-hepmc-writer.C("drich-data.hepmc", 300)'

npsim --compactFile=./drich_only.xml --runType=run -G -N=300 --inputFiles ./drich-data.hepmc --outputFile=./drich-data.root --part.userParticleHandler='' --random.seed 0x12345678 --random.enableEventSeed

xenv -x ../Juggler.xenv gaudirun.py ../irt/drich-testIRT.py
root -l '../irt/scripts/evaluation.C("drich-reco.root")'
```
