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

* Set up shared memory, shared memory module must provide [x]
    * Read/write access to laser data [x]
    * Read/write access to GPS data [x]
    * Read/write access to camera data [x]
    * Read/write access to vehicle control data [x]
    * Read/write access to process management data [x]
* Start all other processes in logical sequence suitable for teleop of UGV [x]
* Monitor process heartbeats [x]
  * Shutdown event if failure of critical process [x]
  * Attempt to restart failed non-critical processes [x]
  * Indicate main process is alive. If dead, shutdown [x]
* Routine shutdown of all processes in response to keypress event [x]

### Laser Module

* Write client to connect to UGV server (192.168.1.200:23000) [x]
* Collect laser rangefinder data [x]
* Process laser rangefinder data to get (X,Y) [x]
* Store laser rangefinder data in array of (X,Y) in shared memory [x]
* Detect laser process heartbeat and respond to shutdown commands from PM module [x]

### GNSS Module

* Write client to connect to UGV server (192.168.1.200:24000) [x]
* Get global position data in binary [x]
* Process binary data to get (X,Y) in UTM coordinates and heigh in metres [x]
* Store processed data in shared memory [x]
* Print Northing, Easting, Height in metres and CRC value [x]
* Stored data needs to be include old data [x]

### Camera Module

* Write client to connect to UGV server (192.168.1.200:26000) [x]
* Get most recent camera image frame [x]
* Display camera image on screen [x]

### Vehicle Control Module

* Write client to connect to UGV server (192.168.1.200:25000) [x]
* Send ASCII messages to teleop the UGV [x]
* Get key commands from display module and send as message for teleop [x]

### Display Module

* Plot laser scan data [x]
* Plot GNSS data as a line [x]

## Assessment
1. Complete above
2. Process check in week 8 to demonstrate architecture
3. Complete assessment in week 10
4. Submit work in zip file on 20 November.
