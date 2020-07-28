
# lifx-configurator

`lifx-configurator` generates a packet that can be sent to any Lifx bulbs on the local network to control their functions. 

Currently `lifx-configurator` is able to generate two packet types with the following options:

1. Set Power (`--power`)
	* `-p, --power` option of `0` (turn off bulb) or `1` (turn on bulb)
	* `-ip, --ip-address` the IP port of the bulb
	* `-d, --duration` option with duration in milliseconds
2. Set Brightness and Color (`--brightness`)
	* `-b, --brightness` value from 0 to 100
 	* `-ip, --ip-address` the IP port of the bulb
	* `-d, --duration` option with duration in milliseconds
	* `-h, --hue` value from 1 to 360
	* `-s, --saturation` value from 0 to 100
	* `-k, --kelvin` value within normal kelvin ranges
3. Packet Only (`--packet-only`)
	* Combine with any of the above packets. Instead of sending the packet, it'll print it instead

If you wish to send both a power and brightness/color packet, you need to call the application twice for **each packet**. Future todo is to combine the packets together.

Warning: I haven't tested the kelvin function though it should work theoretically. I only have a basic bulb to work with.

# Build Instructions

```bash
git clone https://github.com/JohnCiubuc/lifx-configurator
cd lifx-configurator
mkdir build && cd build
cmake .. && make
```
# Usage Instructions
![instructions](https://raw.githubusercontent.com/JohnCiubuc/lifx-configurator/master/Images/usage.png)

# Using the application 

Run `lifx-configurator` with the desired bulb's address and settings. A packet will be generated and sent to that bulb

# Examples

1. Turn on bulb at IP 192.168.1.230
	```bash
	$ lifx-configurator -ip 192.168.1.230 -p 1
	```
2. Set brightness to 20%
	```bash
	$ lifx-configurator -ip 192.168.1.230 -b 20
	```
3. Set brightness to 50%, set saturation to 50%, and set color to blue
	```bash
	$ lifx-configurator -ip 192.168.1.230 -b 50 -s 50 -h 240
	```
4. Set brightness to 72% and Kelvin to 3000K, and take 30 seconds to do so
	```bash
	$ lifx-configurator -ip 192.168.1.230 -b 72 --kelvin 3000K --duration 30000
	```

