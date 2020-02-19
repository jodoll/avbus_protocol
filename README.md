# Arduino AVBus

This Project aims to reengineer the AVBus protocol of the Marantz ST560 Stereo Tuner.

Using the AVBus protocol it is also possible to control multiple HiFi devices from that device line, for example the PM451 Amplifier.

## Devices

### RMC-20AV
<img src="img/rmc-20av.jpg" width="200" />

### ST560
![](img/st560.jpg)

##### AV-Bus Jacks
<img src="img/av-bus-jacks.jpg" width="200" />

### PM-451
![](img/pm451.jpg)

## Building
The project is built with [PlatformIO](https://platformio.org/). Their version of Visual Studio code is used to built the project. Please refer to their documentation on how to setup their SDK. 

Afterwards the project can be opened and should work out of the box on a mac, provided you have set up your USB drivers. For other platforms you may need to add a config to the [platformio.ini](platformio.ini). Please refer to PlatformIO for [instructions](https://docs.platformio.org/en/latest/projectconf.html) on how to do so.

## Project Setup

The 32kHz output of a DS3231 is used to generate a clock signal to measure the time between bus signal changes. The signal is connected to one of the interrupt pins of the Arduino Uno (Pin 2).

The other interrupt pin (Pin 3) is connected to the Remote Control line of the AVBus. Every time the bus changes state the time is stored. 

Notes:
* There seems to be no difference between the IN and OUT jack
* Writing to the bus while reading seems possible, but the input from the RMC-20AV seems to be blocked (See [Todo](#Todo))

### Circuit
Breadboard and PCB design were created with [Fritzing](https://fritzing.org/).  
The Fritzing source file can be found in [circuit](circuit/).

#### ESP32
**PCB**  
![ESP32 PCB](img/pcb_esp32.png)

**Breadboard**  
![ESP32 Breadboard](img/breadboard_esp32.png)

[AvBus_Esp32.fzz](circuit/AvBus_Esp32.fzz)

## Protocol

The default state of the Bus signal is `HIGH`.

The commands seem to have the following structure:
* The bus is pulled to `LOW` for around 10.125 ms (5.626 ms seem to be enough) (`INIT`)
* Then the command follows
  * Structure
    * 6 bit device address
    * 9 bit command
  * The values are determined by a delay before the next change of the bus value
    * 0 -> 0.375 ms 
    * 1 -> 0.750 ms
* The bus is pulled to `LOW` again for some time between 100 ms and 200 ms (5.626 ms seem to be enough) (`HOLD`)
  * When the key on the remote control is held down the bus stays `LOW` until the key is released and the command is executed again by the device

Notes: 
* The timings may be off a good deal due to the sampling rate of 8kHz. This also leads to some alias.

## Todo
### Protocol
- [x] Record and protocol all signals
- [ ] Narrow down timings for INIT, 0, 1 and HOLD
- [ ] What's the threshold where HOLD makes a device repeat the command 

### Circuit
- [x] Check if the DS3231 signal is required (at least not for ESP32)
- [ ] Do not block the bus to allow for side by side operation with RMC-20AV

### Documentation
- [x] Add diagram of wiring
- [ ] Add diagram of the bus signals
- [ ] Add REST api documentation

### Features
- [x] Allow sending of arbitrary commands
- [x] ESP32 port to support a webserver/bt
- [x] Add a Webserver on ESP32
- [ ] Android app to act as a remote control (probably separate project, will be linked here)