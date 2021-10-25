#!/usr/bin/env python

# npsim wrapper with eRICh specific tests
# NOTE: must execute this from the top-level repository directory (`./..`)

import sys, getopt, os, re
import subprocess, shlex
import math
import numpy as np


# ARGUMENTS
################################################################

testNum = -1 
detector = 'erich'
particle = 'pi+'
runType = 'run'
numEvents = 10
outputFileName = ''

helpStr = f'''
{sys.argv[0]} <TESTNUM> [OPTIONS]

<REQUIRED ARGUMENTS>:

    TESTNUM:    -t <testnum>: specify which test to run
            acceptance tests:
                1: aim pions at center of aerogel sector
                2: inner edge test
                3: outer edge test
                4: radial scan test
                5: azimuthal+radial scan test
                6: spray pions in one sector
                7: momentum scan
            optics tests:
                10:   focal point, in eRICh acceptance
                        ( recommend: optDbg=1 / mirDbg=0 / sensDbg=1 )
                11:   focal point, broad range test
                        ( recommend: optDbg=1 / mirDbg=1 / sensDbg=1 )
                12:   parallel-to-point focal test
                        ( recommend: optDbg=1 / mirDbg=0 / sensDbg=0 )

[OPTIONAL ARGUMENTS]

    OPTIONS:    -a: athena full simulation
                -e: eRICh only (default)
                -p [particle]: name of particle to throw; some examples:
                    - e- / e+
                    - pi+ / pi-
                    - kaon+ / kaon-
                    - proton / anti_proton
                    - opticalphoton
                -n [numEvents]: number of events (usu. at each point)
                -r: run, instead of visualize (default)
                -v: visualize, instead of run
                -o [output file]: output root file name (written to sim/out/)
                -h: print help
    '''

if(len(sys.argv)<=1):
    print(helpStr)
    sys.exit(2)
try: opts, args = getopt.getopt(sys.argv[1:],'t:aep:n:vro:h')
except getopt.GetoptError:
    print('\n\nERROR: invalid argument\n',helpStr)
    sys.exit(2)
for opt, arg in opts:
    if(opt=='-t'): testNum = int(arg)
    if(opt=='-a'): detector = 'athena'
    if(opt=='-e'): detector = 'erich'
    if(opt=='-p'): particle = arg
    if(opt=='-n'): numEvents = int(arg)
    if(opt=='-r'): runType = 'run'
    if(opt=='-o'): outputFileName = arg
    if(opt=='-v'): runType = 'vis'
    if(opt=='-h'):
        print(helpStr)
        sys.exit()

if(testNum<0):
    print('\n\nERROR: test number required, e.g. `-t3`\n',helpStr)
    sys.exit(2)

### overrides
if(testNum>=10):
    print("optics test, overriding some settings...")
    particle='opticalphoton'
    runType='vis'


sep='-'*40
print(sep)
print(f'testNum = {testNum}')
print(f'detector = {detector}')
print(f'particle = {particle}')
print(f'numEvents = {numEvents}')
print(f'runType = {runType}')
print(sep)

################################################################


### set compact file
if(detector=="athena"):
    compactFile='athena.xml'
elif(detector=="erich"):
    compactFile='compact/subsystem_views/erich_only.xml'
else:
    print('ERROR: unknown detector\n')
    sys.exit(2)

### start macro file
simDir = os.getcwd()+"/sim"
m = open(simDir+"/macro/tmp_erich.mac",'w+')

### common settings
m.write(f'/control/verbose 2\n')
m.write(f'/run/initialize\n')
#m.write(f'/run/useMaximumLogicalCores\n')

### visual settings
if(runType=='vis'):

    m.write(f'/vis/open OGLSQt 800x800-0+0\n') # driver

    m.write(f'/vis/scene/create\n')

    m.write(f'/vis/scene/add/volume\n')
    m.write(f'/vis/scene/add/axes 0 0 0 1 m\n')
    m.write(f'/vis/scene/add/trajectories smooth\n')
    m.write(f'/vis/scene/add/hits\n')

    m.write(f'/vis/sceneHandler/attach\n')

    #m.write(f'/vis/viewer/set/viewpointThetaPhi 115 65\n') # angled view
    #m.write(f'/vis/viewer/set/viewpointThetaPhi 0 0\n') # front view
    m.write(f'/vis/viewer/set/viewpointThetaPhi -90 -89\n') # top view
    #m.write(f'/vis/viewer/set/viewpointThetaPhi 90 0\n') # side view
    #m.write(f'/vis/viewer/zoom 0.5\n')
    m.write(f'/vis/viewer/set/style wireframe\n')

    m.write(f'/vis/modeling/trajectories/create/drawByCharge\n')
    m.write(f'/vis/modeling/trajectories/drawByCharge-0/setRGBA 0 0.8 0 0 1\n')
    m.write(f'/vis/modeling/trajectories/drawByCharge-0/setRGBA 1 0 0.5 0.5 1\n')


### set particle energy
energy = '8.0 GeV'
if(particle=="opticalphoton"): energy = '3.0 eV'

### append particle info
m.write(f'/gps/verbose 2\n')
m.write(f'/gps/particle {particle}\n')
m.write(f'/gps/number 1\n')
if(testNum!=7): m.write(f'/gps/ene/mono {energy}\n')
#m.write(f'/gps/ene/type Gauss\n')
#m.write(f'/gps/ene/sigma 3.0 GeV\n')

### append source settings
m.write(f'/gps/position 0 0 0 cm\n')

### define envelope acceptance limits [units=cm]
rMin = 8.0 + 4.0
rMax = 93.0 - 5.0
zMax = 210.0 - 20.0 # (keep positive, despite it's actually at negative z)

### derived attributes
thetaMin = math.atan2(rMin,zMax)
thetaMax = math.atan2(rMax,zMax)
etaMin = -math.log(math.tan(0.5*thetaMin))
etaMax = -math.log(math.tan(0.5*thetaMax))
print('ENVELOPE\n')
print(f'rMin = {rMin} cm')
print(f'rMax = {rMax} cm')
print(f'zMax = {zMax} cm\n')
print(f'thetaMin = {math.degrees(thetaMin)} deg')
print(f'thetaMax = {math.degrees(thetaMax)} deg\n')
print(f'etaMin = {etaMin}')
print(f'etaMax = {etaMax}\n')
print(sep)



### TESTS ############################

if( testNum == 1 ):
    m.write(f'\n# aim at +x eRICh sector\n')
    m.write(f'/gps/direction 0.25 0.0 -1.0\n')
    m.write(f'/run/beamOn {numEvents}\n')

elif( testNum == 2 ):
    m.write(f'\n# inner edge of acceptance\n')
    m.write(f'/gps/direction {rMin} 0.0 -{zMax}\n')
    m.write(f'/run/beamOn {numEvents}\n')

elif( testNum == 3 ):
    m.write(f'\n# outer edge of acceptance\n')
    m.write(f'/gps/direction {rMax} 0.0 -{zMax}\n')
    m.write(f'/run/beamOn {numEvents}\n')

elif( testNum == 4 ):
    numRad = 4 # number of radial steps
    m.write(f'\n# radial scan test\n')
    if(runType=="vis"):
        m.write(f'/vis/scene/endOfEventAction accumulate\n')
        m.write(f'/vis/scene/endOfRunAction accumulate\n')
    for r in list(np.linspace(rMin,rMax,numRad)):
        m.write(f'/gps/direction {r} 0.0 -{zMax}\n')
        m.write(f'/run/beamOn {numEvents}\n')

elif( testNum == 5 ):
    numRad = 4 # number of radial steps
    numPhi = 18 # number of phi steps, prefer odd multiple of 6 (18,30,42)
    m.write(f'\n# azimuthal+radial scan test\n')
    if(runType=="vis"):
        m.write(f'/vis/scene/endOfEventAction accumulate\n')
        m.write(f'/vis/scene/endOfRunAction accumulate\n')
    for r in list(np.linspace(rMin,rMax,numRad)):
        for phi in list(np.linspace(0,2*math.pi,numPhi,endpoint=False)):
            #if(phi>math.pi/6 and phi<(2*math.pi-math.pi/6)): continue # restrict to one sector
            print(phi*180/math.pi)
            x = r*math.cos(phi)
            y = r*math.sin(phi)
            m.write(f'/gps/direction {x} {y} -{zMax}\n')
            m.write(f'/run/beamOn {numEvents}\n')

elif( testNum == 6 ):
    m.write(f'\n# pion spray test, erich range\n')
    if(runType=="vis"):
        m.write(f'/vis/scene/endOfEventAction accumulate\n')
    m.write(f'/gps/pos/type Point\n')
    m.write(f'/gps/pos/radius 0.1 mm\n')
    m.write(f'/gps/ang/type iso\n')
    m.write(f'/gps/ang/mintheta {math.pi-thetaMax} rad\n')
    m.write(f'/gps/ang/maxtheta {math.pi-thetaMin} rad\n')
    m.write(f'/gps/ang/minphi {math.pi} rad\n')
    m.write(f'/gps/ang/maxphi {math.pi+0.01} rad\n')
    m.write(f'/run/beamOn {numEvents}\n')

elif( testNum == 7 ):
    m.write(f'\n# momentum scan\n')
    m.write(f'/gps/direction 0.25 0.0 -1.0\n')
    for en in list(np.linspace(1,60,10)):
        m.write(f'/gps/ene/mono {en} GeV\n')
        m.write(f'/run/beamOn {numEvents}\n')

elif( testNum == 10 ):
    m.write(f'\n# opticalphoton scan test, erich range\n')
    m.write(f'/vis/scene/endOfEventAction accumulate\n')
    m.write(f'/gps/pos/type Point\n')
    m.write(f'/gps/pos/radius 0.1 mm\n')
    m.write(f'/gps/ang/type iso\n')
    m.write(f'/gps/ang/mintheta {math.pi-thetaMax} rad\n')
    m.write(f'/gps/ang/maxtheta {math.pi-thetaMin} rad\n')
    m.write(f'/gps/ang/minphi {math.pi} rad\n')
    m.write(f'/gps/ang/maxphi {math.pi+0.01} rad\n')
    m.write(f'/run/beamOn {numEvents}\n')

elif( testNum == 11 ):
    m.write(f'\n# opticalphoton scan test, broad range\n')
    m.write(f'/vis/scene/endOfEventAction accumulate\n')
    m.write(f'/gps/pos/type Point\n')
    m.write(f'/gps/pos/radius 0.1 mm\n')
    m.write(f'/gps/ang/type iso\n')
    m.write(f'/gps/ang/mintheta {math.pi/2} rad\n')
    m.write(f'/gps/ang/maxtheta {math.pi-thetaMin} rad\n')
    m.write(f'/gps/ang/minphi {math.pi} rad\n')
    m.write(f'/gps/ang/maxphi {math.pi+0.01} rad\n')
    m.write(f'/run/beamOn {numEvents}\n')

elif( testNum == 12 ):
    m.write(f'\n# opticalphoton parallel-to-point focusing\n')
    m.write(f'/vis/scene/endOfEventAction accumulate\n')
    m.write(f'/vis/scene/endOfRunAction accumulate\n')
    m.write(f'/gps/pos/type Beam\n')
    m.write(f'/gps/ang/type beam1d\n')
    for rVal in list(np.linspace(rMin,rMax,5)): # number of beams within theta acceptance
        m.write(f'/gps/ang/rot1 -{zMax} 0 {rVal}\n')
        m.write(f'/gps/pos/rot1 -{zMax} 0 {rVal}\n')
        m.write(f'/gps/pos/halfx 8 cm\n') # parallel beam width
        m.write(f'/run/beamOn {numEvents}\n')

else:
    print("ERROR: unknown test number\n")
    m.close()
    sys.exit(2)

### final info
if(runType=="vis"):
    m.write(f'/vis/viewer/flush\n')
    m.write(f'/vis/viewer/refresh\n')


### print macro and close stream
m.seek(0,0)
print(m.read())
print(sep)
m.close()

#########################################################
# MACRO FILE BUILT


### set output file
if outputFileName=='': outputFile = simDir+"/out/sim_"+runType+".root"
else: outputFile = simDir+"/out/"+outputFileName

### simulation executable and arguments
cmd = "npsim"
cmd += " --runType " + runType
cmd += " --compactFile " + compactFile
#cmd += " --random.seed 1 "
cmd += " --macro " + m.name
cmd += " --outputFile " + outputFile
cmd += " --enableG4GPS"
#if(runType=="vis"): cmd += " --enableQtUI"

### run simulation
print(sep)
print(cmd)
print(sep)
subprocess.call( shlex.split(cmd), cwd='athena' )
os.system('mv -v athena/erich-config.root sim/irtgeo/')
