# Rover Drive Firmware

Uses Janus, OpenCRDynamixelBridge and Hermes, and receives commands from Ares to make the rover drive. 
The purpose of this project is to glue everything together. 
Something similar will be provided for the arm firmware shortly.

### Todo:
- Make decision to either splice everything into one file for simplicity or keep as is.
  - Make encapsulation clearer if choosing the ladder option.
-  Better labeling of variables, especially in `drive.h`.