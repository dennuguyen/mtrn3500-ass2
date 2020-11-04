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
    * Read/write access to laser data []
    * Read/write access to GPS data []
    * Read/write access to camera data []
    * Read/write access to vehicle control data []
    * Read/write access to process management data []
* Start all other processes in logical sequence suitable for teleop of UGV []
* Monitor process heartbeats []
  * Shutdown event if failure of critical process []
  * Attempt to restart failed non-critical processes []
  * Indicate main process is alive. If dead, shutdown []
* Routine shutdown of all processes in response to keypress event []

### Laser Module

* Write client to connect to UGV server (ip: 192.168.1.200, port: 23000) []
* Collect laser rangefinder data []
* Store laser rangefinder data in array of (X,Y) in shared memory []
* Detect laser process heartbeat and respond to shutdown commands from PM module []

### GNSS Module

### Camera Module

### Vehicle Control Module

### Display Module

## Assessment
1. Complete above
2. Process check in week 8 to demonstrate architecture
3. Complete assessment in week 10
4. Submit work in zip file on 20 November.
