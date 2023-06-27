SparkFun Weather Meter Kit Arduino Library
========================================

<p align="center">
	<a href="https://github.com/sparkfun/SparkFun_Weather_Meter_Kit_Arduino_Library/issues" alt="Issues">
		<img src="https://img.shields.io/github/issues/sparkfun/SparkFun_Weather_Meter_Kit_Arduino_Library.svg" /></a>
	<a href="https://github.com/sparkfun/SparkFun_Weather_Meter_Kit_Arduino_Library/blob/master/LICENSE" alt="License">
		<img src="https://img.shields.io/badge/license-MIT-blue.svg" /></a>
	<a href="https://twitter.com/intent/follow?screen_name=sparkfun">
		<img src="https://img.shields.io/twitter/follow/sparkfun.svg?style=social&logo=twitter" alt="follow on Twitter"></a>
	<a href="https://github.com/sparkfun/SparkFun_Weather_Meter_Kit_Arduino_Library/actions" alt="Actions">
		<img src="https://github.com/sparkfun/SparkFun_Weather_Meter_Kit_Arduino_Library/actions/workflows/mkdocs.yml/badge.svg" /></a>
</p>

![Product Image - Weather Meter Kit](https://cdn.sparkfun.com//assets/parts/1/4/5/3/9/15901-Weather_Meter-02.jpg)

[Weather Meter Kit](https://www.sparkfun.com/products/15901) *(SEN-15901)*


Whether you're an agriculturalist, a professional meteorologist, or a weather hobbyist, building a weather station can be a rewarding project. This Arduino library allows users to easily record wind speed, wind direction, and rainfall data from our [weather meter kit](https://www.sparkfun.com/products/15901). It should be noted that the sensors in the weather meter kit rely on magnetic reed switches and requires a power source to make any measurements.

- The rain gauge is a self-emptying bucket-type rain gauge, which activates a momentary button closure for each 0.011" of rain that is collected.
- The anemometer (wind speed meter) encodes the wind speed by simply closing a switch which each rotation. A wind speed of 1.492 MPH produces a switch closure once per second.
- The wind vane reports wind direction as a voltage, which is produced by the combination of resistors inside the sensor. The vane’s magnet could potentially close two switches at once, allowing up to 16 different positions to be indicated, but we have found that 8 positions are more realistic.


SparkFun labored with love to create this code. Feel like supporting open-source hardware and software? Buy a board from SparkFun!
*This library is intended to be utilized with the [weather meter kit](https://www.sparkfun.com/products/15901) and the following boards:*

<table>
  <tr align="center">
  	<td><a href="https://www.sparkfun.com/products/13956"><img src="https://cdn.sparkfun.com//assets/parts/1/1/6/6/5/13956-01.jpg" alt="Product Image - SparkFun Weather Shield"></a></td>
  	<td><a href="https://www.sparkfun.com/products/16794"><img src="https://cdn.sparkfun.com//assets/parts/1/5/7/0/3/16794-SparkFun_MicroMod_Weather_Carrier_Board-01b.jpg" alt="Product Image - SparkFun MicroMod Weather Carrier Board"></a></td>
  </tr>
  <tr align="center">
  	<td>SparkFun Weather Shield <i>[<a href="https://www.sparkfun.com/products/13956">DEV-13956</a>]</i></td>
  	<td>SparkFun MicroMod Weather Carrier Board <i>[<a href="https://www.sparkfun.com/products/16794">SEN-16794</a>]</i></td>
  </tr>
</table>

## Supported Microcontrollers - Arduino Environment

* Weather Shield
  * [ATMega328](https://www.sparkfun.com/products/18158)
* MicroMod Weather Carrier Board
  * [Artemis](https://www.sparkfun.com/products/16401)
  * [SAMD51](https://www.sparkfun.com/products/16791)
  * [ESP32](https://www.sparkfun.com/products/16781)
  * [STM32](https://www.sparkfun.com/products/21326)
  * [nrf5280](https://www.sparkfun.com/products/16984)
  * [Teensy](https://www.sparkfun.com/products/16402)
  * [RP2040](https://www.sparkfun.com/products/17720)

Repository Contents
-------------------

* [**/documents**](./documents) - Datasheet and User Manual
* [**/examples**](./examples) - Example sketches for the library (.ino). Run these from the Arduino IDE.
* [**/src**](./src) - Source files for the library (.cpp, .h).
* [**keywords.txt**](./keywords.txt) - Keywords from this library that will be highlighted in the Arduino IDE.
* [**library.properties**](./library.properties) - General library properties for the Arduino package manager.
* [**CONTRIBUTING.md**](./CONTRIBUTING.md) - Guidelines on how to contribute to this library.

Documentation
-------------

* **[Installing an Arduino Library Guide](https://learn.sparkfun.com/tutorials/installing-an-arduino-library)** - Basic information on how to install an Arduino library
* **[Assembly Guide](https://learn.sparkfun.com/tutorials/681)** - A tutorial for assembling the weather meter kit


Products that use this Library
------------------------------

* **[SEN-16794](https://www.sparkfun.com/products/16794)** - SparkFun MicroMod Weather Carrier Board
* **[DEV-13956](https://www.sparkfun.com/products/13956)** - SparkFun Weather Shield
* **[SEN-15901](https://www.sparkfun.com/products/15901)** - Weather Meter Kit

Contributing
------------
If you would like to contribute to this library: please do, we truly appreciate it, but please follow [these guidelines](./CONTRIBUTING.md). Thanks!

License Information
-------------------

SparkFun's source files are ***open source***!

Please review the [`LICENSE.md`](LICENSE.md) file for license information.

Distributed as-is; no warranty is given.

\- Your friends at SparkFun.
