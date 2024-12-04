# Bot Behavior SC2 Bot for CMPUT 350

## Group Members
* Jesse Goertzen (Jesse-Goertzen)
* Tony Yang (tony-m-yang)
* James Stephen (scravers)
* Kaia Bourque (kbreex)


## Compile Instructions

```
:: Clone the project
$ git clone --recursive https://github.com/Jesse-Goertzen/bot-behavior-sc2
$ cd BasicSc2Bot

:: Create build directory.
$ mkdir build
$ cd build

:: Set Apple Clang as the default compiler
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

:: Generate a Makefile
:: Use 'cmake -DCMAKE_BUILD_TYPE=Debug ../' if debug info is needed
$ cmake -DCMAKE_BUILD_TYPE=Release ../

:: Build
$ make
```

Above is for mac. If using windows or linux, follow the instuctions here https://github.com/tuero/BasicSc2Bot

## Running Instuctions

Similar to instuctions in the link above, use the following command to play the bot against built in AI against zerg on the cactus valley map.
The excecutable will be under `bin` directory.

`./BasicSc2Bot -c -a zerg -d Hard -m CactusValleyLE.SC2Map`



