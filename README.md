tea5767_i2c
=============
I2C Driver for NXP FM Radio module

Designed to be used with the RP2040 SDK library. Finally made into an Arduino library.

Disclaimer: There are other (better) libraries for this device since 2013. I created this one mainly to learn and have fun.

tea5767_i2c Class
=================

The class ``tea5767_i2c`` exposes some functionalities 


tea5767_i2c(uint8_t band_mode)
------------------------------
Class constructor. Pass the frequency band desired:
``JP_BAND``,``EU_BAND``

void begin()
------------
Initialize the parameters needed for ``setup()``.

float tea5767_getStation()
--------------------------
Gets the current station frequency from the TEA5757 radio and prints it to stdout.
This function reads the raw data from the TEA5757 radio using the ``tea5767_read_raw()`` function,
extracts the frequency values from the read buffer, and calculates the frequency in MHz.
The extracted frequency is then printed to stdout with two decimal places.
@note This function assumes that the TEA5757 radio device has been initialized and is currently powered on.
    
void tea5767_setSearch(uint8_t searchMode, uint8_t searchUpDown);
--------------------
Configures the search mode and direction of the TEA5757 tuner.
This function sets the search mode and direction of the TEA5757 tuner. It updates the values of the TEA5757_t structure
with the given search mode and search direction and then writes them to the tuner using the `tea5767_write_registers()` function.

`searchMode` An 8-bit unsigned integer specifying the search mode to use. Valid values are:

`TEA5767_SEARCH_MUTE_LOW`

`TEA5767_SEARCH_MUTE_HIGH`

`TEA5767_SEARCH_MUTE_BW`

`TEA5767_SEARCH_MUTE_OFF`

`TEA5767_SEARCH_MUTE_SW`

`searchUpDown` An 8-bit unsigned integer specifying the direction of the search. Valid values are:

`TEA5767_SEARCH_UP`

`TEA5767_SEARCH_DOWN`

void tea5767_setStation(float freq)
-----------------------------------
Sets the frequency of the TEA5757 tuner.
This function sets the frequency of the TEA5757 tuner to the given value.
If the frequency is out of range for the current band mode, it will be adjusted to the nearest valid frequency.
After setting the frequency, the new value will be written to the tuner through the `tea5767_write_registers` function.
`freq` The desired frequency to set the tuner to.
