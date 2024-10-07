TEA5767 Ardunio driver for RP2040
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

void tea5767_setStationInc(float freq)
--------------------------------------
Increments the current frequency of the TEA5757 radio by a given value.
This function increases the current frequency of the TEA5757 radio by a given value.
If the given value is negative, the search mode is set to down, otherwise the search mode is set to up.
The frequency is then incremented by the given value, but it is first checked to ensure that it
is within the frequency limits specified by the radio's band mode. The resulting frequency is then
written to the radio using the tea5767_write_registers function.

`freq` The frequency increment value.

void tea5767_setMute(bool mute)
-------------------------------
Sets the mute mode of the TEA5757 tuner.
This function sets the mute mode of the TEA5757 tuner to the specified value. When mute mode is enabled,
the audio output is muted.

`mute` The desired mute mode value. true to enable mute mode, false to disable it.
  
void tea5767_setSoftMute(bool mute)
-----------------------------------
Sets the soft mute mode of the TEA5767 radio.
This function sets the soft mute mode of the TEA5767 radio to either on or off.

`mute` A boolean indicating whether the soft mute mode should be on (true) or off (false).

void tea5767_setMuteLeft(bool mute)
-----------------------------------
Sets the left channel mute mode for the TEA5767 radio.

`mute` Boolean value indicating whether the left channel should be muted.
    
void tea5767_setMuteRight(bool mute)
------------------------------------
Sets the right channel mute mode for the TEA5767 radio.

`mute` Boolean value indicating whether the left channel should be muted.
    
void tea5767_setStandby(bool standby)
-------------------------------------
Sets the standby mode of the TEA5757 radio.
This function sets the standby mode of the TEA5757 radio. When the radio is in standby mode, it consumes less power but cannot receive signals.

`standby` Set to true to activate standby mode, false to deactivate it.
    
void tea5767_setStereo(bool stereo)
-----------------------------------
Sets the stereo mode of the TEA5757 radio.
This function sets the stereo mode of the TEA5757 radio. When the radio is in stereo mode, it receives stereo signals if available. When in mono mode, it receives only mono signals.

`stereo` Set to true to activate stereo mode, false to activate mono mode.
    