#Wind Speed and Direction Measurement
##Overview
This project is being created for demonstration of concepts relating to measuring methods for wind direction and speed alongwith communications / data aggregation in remote centres for analysis.

Project is targeted for general learning on concepts at high level of electronics and computers, with focus on the logic building that is required to measure, refine and collate data over networks.

##Architecture
It uses low cost electronic components around atmega8 controller. It has to provide a local display as well as communication with remote computers over wire (USB). Mechanical structure needs to be provide stationary circular base with sensors fitting. There will be two such bases

* One with 8 sensors to detect eight directions of wind arrow
* One with 1 sensor to detect rotations of wind wane

In both the mechanical structures, this stationary base will have a freely moving circular disc with 45deg sector cut out to allow for sensor to detect it. This moving circular disc will be attached to the wind arrow or wind wane, causing the movement of this sector-cut-disc over the sensors bases.

This data shall be continuously emitted by the project towards computer, which will read, store and analyse data.

Local display shall also display the same information over LCD.

Refer to detailed information to:

* [Differences of atmega8 with respect to atmega328p](docs/diff_m8_m328p.md)
* [Design of mechanical structure](docs/mechanical_structure.md)
* [Hardware - Design](docs/hw_design.md)
* [Firmware Code - Design](docs/fw_design.md)
* [Computer Code - Design](docs/mc_design.md)