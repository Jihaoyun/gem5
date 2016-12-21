# GEM5 with fault injection

This is a fork of the [gem5](http://gem5.org) simulator.
The main purpose of this project is to add a fault injection system to the simulation.

Currently onlye these kind of injection are possibile:
  - Branch Prediction Unit fault injection
  - Register File fault injection

## Installation

1. Clone the project:
```
git clone https://github.com/cancro7/gem5.git
cd gem5
```
2. Install the dependencies:
```
sudo apt-get update; sudo apt-get upgrade
sudo apt-get install scons swig gcc m4 python python-dev libgoogle-perftools-dev g++ protobuf-compiler protobuf-c-compiler
```
A more detailed guide is available [here](http://gem5.org/Dependencies)

3. Build the system:
`scons build/ALPHA/gem5.opt -j4`

4. Run a simulation test to verify that everything is ok:
`build/ALPHA/gem5.opt configs/example/se.py -c tests/test-progs/hello/bin/arm/linux/hello`

