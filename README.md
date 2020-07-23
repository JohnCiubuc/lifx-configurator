# lifx-configurator

lifx-configurator generates a packet that can be sent to any Lifx bulbs on the local network to control their functions. 

Currently, only changing the brightness of the bulbs is allowed (I have only a plain bulb to use). If you have a colored bulb, it will change the color to green. Changes pending in the future.

# Build Instructions

1. run `cmake . && make`

# Usage Instructions
![instructions](https://raw.githubusercontent.com/JohnCiubuc/lifx-configurator/master/Images/usage.png)

# Using the application 

Run `lifx-configurator` with the desired bulb's address and settings. A packet will be generated and sent to that bulb
