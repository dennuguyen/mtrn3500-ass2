# 3500-ass2

## Specification

Design an architecture which includes:
- Process Management Module
- Laser Module
- GNSS Module
- Vehicle Control Module
- Camera Module
- Display Module

Deliverables:
- GPS data display.
- Steering and propulsion control of UGV.
- Graphical display of laser data, camera data, GPS data.

### Process Management Module

* Set up shared memory, shared memory module must provide []
    * Read/write access to laser data [x]
    * Read/write access to GPS data [x]
    * Read/write access to camera data []
    * Read/write access to vehicle control data []
    * Read/write access to process management data []
* Start all other processes in logical sequence suitable for teleop of UGV []
* Monitor process heartbeats [x]
  * Shutdown event if failure of critical process [x]
  * Attempt to restart failed non-critical processes [x]
  * Indicate main process is alive. If dead, shutdown [x]
* Routine shutdown of all processes in response to keypress event [x]

### Laser Module

* Write client to connect to UGV server (192.168.1.200:23000) [x]
* Collect laser rangefinder data [x]
* Process laser rangefinder data to get (X,Y) [x]
* Store laser rangefinder data in array of (X,Y) in shared memory []
* Detect laser process heartbeat and respond to shutdown commands from PM module [x]

### GNSS Module

* Write client to connect to UGV server (192.168.1.200:24000) [x]
* Get global position data in binary [x]
* Process binary data to get (X,Y) in UTM coordinates and heigh in metres []
* Store processed data in shared memory []
* Print Northing, Easting, Height in metres and CRC value []

### Camera Module

* 

### Vehicle Control Module

### Display Module

## Assessment
1. Complete above
2. Process check in week 8 to demonstrate architecture
3. Complete assessment in week 10
4. Submit work in zip file on 20 November.
