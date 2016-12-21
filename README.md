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
sudo apt-get install scons swig gcc m4 python python-dev libgoogle-perftools-dev g++ protobuf-compiler protobuf-c-compiler zlib1g-dev libprotobuf-dev
```
A more detailed guide is available [here](http://gem5.org/Dependencies)

3. Build the system:
`scons build/ALPHA/gem5.opt -j4`

4. Run a simulation test to verify that everything is ok:
`build/ALPHA/gem5.opt configs/example/se.py -c tests/test-progs/hello/bin/arm/linux/hello`

5. Install ALPHA cross-compiler:
```
cd /opt
sudo wget http://www.m5sim.org/dist/current/alphaev67-unknown-linux-gnu.tar.bz2
sudo tar -jxf alphaev67-unknown-linux-gnu.tar.bz2
sudo chown $USER -R alphaev67-unknown-linux-gnu
sudo gedit /etc/environment
```
Add the following path to the PATH variable:
`/opt/alphaev67-unknown-linux-gnu/bin`

Save the file and reboot the machine

6. Install the ARM cross-compiler
`sudo apt-get install gcc-arm-linux-gnueabi`


## Branch Prediction Unit fault injection
Currently only BTB (Branch Target Buffer) injections are possibile.
We build a python script that firstly runs a simulation without any fault injection (golden) and then a runs a simulation for each faults specified in a file.
The script is located in the main folder of gem5 and it's called `fault-injection-simulation.py`
This script has the following parameters:
* **-b --benchmarks** : specify the path of the desired testbench to be executed
* **-i --fault-input** : specify the path of the file containing all the desired fault to inject
* **-o --options** : if needed testbench arguments can be passed as a string (i.e. -o "arg1 arg2 ...")

The fault-input file must be formatted in the following way:

`LABEL: STACK_AT, FIELD, ENTRY_NUMBER, BIT_POSITION, START_TICK, END_TICK`

For sake of clarity an example is reported below:
```
FAULT0: 1 , 0 , 232 , 5 , 0 , -1
```
In this example it's specified a fault labeled "FAULT0" which will stuck at 1 the 5-th bit of the tag field of the 232-th entry of the BTB.
Field value can be the following:
* 0 : tag field of the BTB
* 1 : target field of the BTB
* 2 : validty field ot the BTB


