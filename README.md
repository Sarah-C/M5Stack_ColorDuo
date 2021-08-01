# M5Stack_ColorDuo - for TCS34725 color sensor
Color sensor demo for Core Grey, and Core2. 

This little demo shows how to:
1. Save preferences between power cycles.
2. Compile features unique to Core2 (Such as the LED strips in the M5Go-Bottom-2) from 1 codebase.
    https://shop.m5stack.com/products/m5go-battery-bottom2-for-core2-only
3. Displaying HEX codes in drawString to the screen.

This color demo shows the "Clear" white color of the sensor, as well as the red, green and blue components.
It also translates it in to the CSS hex color triplet.... #RRGGBB.

Lastly, the sensor can be calibrated.
Place your sensor on some white paper, and observe the R G B values. They should all be roughly equal, if not the sensor needs a bit of calibrating.
While the sensor is on the paper, press and hold Button-C (touch sensitive circle logo on Core2 or the silver button on Core Grey) for more than 1 second and then release.
This will calibrate the sensor and store it in EEPROM preferences for the sketch.

What calibration does is take the color that's in the middle of brightnesses of the three colors, and calculate a scaler for the other two to make them match this middle value.
In effect all 3, R G and B become the same value - which will be grey.


