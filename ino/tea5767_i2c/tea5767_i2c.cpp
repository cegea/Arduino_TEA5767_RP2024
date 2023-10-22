/**
 ********************************************************************************
 * @file    tea5737_i2c.c
 * @author  Carlos Egea
 * @date    06/06/2023
 * @brief   I2C Driver for NXP FM Radio module.
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "tea5767_i2c.h"
#include <Wire.h>

/************************************
 * GLOBAL FUNCTIONS
 ************************************/

tea5767_i2c::tea5767_i2c(uint8_t band_mode){ // Constructor

    _address = 0x60;
    // Default (See datasheet).
    //buf[0] = 0x40;buf[1] = 0x00;buf[2] = 0x90;buf[3] = 0x1E;buf[4] = 0x00;

    _mute_mode = false;
    _searchMode = false;
    _frequency = 92.4;
    _searchUpDown = 1;
    _searchLevel = ADC_LOW;
    _stereoMode = true;
    _muteLmode = false;
    _muteRmode = true;
    _standby = false;
    _band_mode = band_mode; // EU_BAND;
    _softMuteMode = false;
    _hpfMode = true;
    _stereoNoiseCancelling = true;
}

void tea5767_i2c::tea5767_read_raw(uint8_t *buffer) {
    Wire.requestFrom(_address, TEA5767_REGISTERS);
    while (Wire.available()) {
        for (size_t i = 0; i < TEA5767_REGISTERS; i++){
            buffer[i] = Wire.read(); // OJOOO
            //Serial.println(buf[i]);
        }        
    }
    //i2c_read_blocking(i2c_default, _address, buffer, TEA5767_REGISTERS, false);
    #ifdef DEEBUG_SERIAL0
    Serial.println("New response");
    Serial.print("[");
    for (size_t i = 0; i < TEA5767_REGISTERS; i++)
    {
        Serial.print(buffer[i],HEX); Serial.print(" ");
    }
    Serial.println("]");
    #endif
    
}

void tea5767_i2c::tea5767_write_registers() {
    uint8_t registers[TEA5767_REGISTERS];
    #ifdef DEEBUG_SERIAL1
    printStatus();
    #endif
    // Calculate the frequency value to be written to the TEA5767 register based on the current radio frequency in MHz. 
    // The calculation takes into account the fixed offset of 225kHz and the 4:1 prescaler used by the TEA5767 module.
    float freq = 4*(_frequency * 1000000 + 225000) / 32768;
    int integer_freq = (int)round(freq);

    registers[0] = integer_freq >> 8 | _mute_mode << 7 | _searchMode << 6;
    registers[1] = integer_freq & 0xff;
    registers[2] = _searchUpDown << 7 | _searchLevel << 5 | 1 << 4 | _stereoMode << 3
            | _muteRmode << 2 | _muteLmode << 1;
    registers[3] = _standby << 6 | _band_mode << 5 | 1 << 4 | _softMuteMode << 3 | _hpfMode << 2;
    registers[3] = registers[3] | _stereoNoiseCancelling << 1;
    registers[4] = 0x00;
    
    Wire.beginTransmission(_address); 

    for (size_t i = 0; i < TEA5767_REGISTERS; i++)
    {
        Wire.write(registers[i]);
    }
    //Wire.write(registers, TEA5767_REGISTERS);
    uint8_t ack = Wire.endTransmission();

    #ifdef DEEBUG_SERIAL0
    printStatus();
    Serial.println("New command");
    Serial.print("[");
    for (size_t i = 0; i < TEA5767_REGISTERS; i++)
    {
        Serial.print(registers[i],HEX); Serial.print(" ");
    }
    Serial.println("]");
    
    if (ack != 0) { //We have a problem!
        Serial.print("Write Fail:"); //No ACK!
        Serial.println(ack, DEC); //I2C error: 0 = success, 1 = data too long, 2 = rx NACK on address, 3 = rx NACK on data, 4 = other error
    }

    #endif
    
}

void tea5767_i2c::begin(){ // tea5767_init  

    // TODO: Allow other pins than I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    Wire.setSDA(PICO_DEFAULT_I2C_SDA_PIN);
    Wire.setSCL(PICO_DEFAULT_I2C_SCL_PIN);
    Wire.setClock(400);
    Wire.begin();
    tea5767_write_registers();
}

void tea5767_i2c::printStatus(){
    Serial.print("_address ");Serial.println(_address);
    Serial.print("_mute_mode ");Serial.println(_mute_mode);
    Serial.print("_searchMode ");Serial.println(_searchMode);
    Serial.print("_frequency ");Serial.println(_frequency);
    Serial.print("_searchUpDown ");Serial.println(_searchUpDown);
    Serial.print("_searchLevel ");Serial.println(_searchLevel);
    Serial.print("_stereoMode ");Serial.println(_stereoMode);
    Serial.print("_muteLmode ");Serial.println(_muteLmode);
    Serial.print("_muteRmode ");Serial.println(_muteRmode);
    Serial.print("_standby ");Serial.println(_standby);
    Serial.print("_band_mode ");Serial.println(_band_mode);
    Serial.print("_softMuteMode ");Serial.println(_softMuteMode);
    Serial.print("_hpfMode ");Serial.println(_hpfMode);
    Serial.print("_stereoNoiseCancelling ");Serial.println(_stereoNoiseCancelling);
}

float tea5767_i2c::tea5767_getStation() {
    uint8_t buf[TEA5767_REGISTERS];

    // Read current settings from the TEA5767 module
    tea5767_read_raw(buf);

    // Calculate the current frequency based on the TEA5767's register values
    float freq = (buf[0] & 0x3f) << 8 | buf[1];
    freq = (freq*32768/4 - 225000) / 1000000;

    return freq;
}

int tea5767_i2c::tea5767_getReady() {
    uint8_t buf[TEA5767_REGISTERS];
    tea5767_read_raw(buf);
    _isReady = buf[0] >> 7;
    return _isReady;
}

void tea5767_i2c::tea5767_setSearch(uint8_t searchMode, uint8_t searchUpDown) {
    _searchUpDown = searchUpDown;
    _searchMode = searchMode;
    tea5767_write_registers();
}

float tea5767_i2c::tea5767_checkFreqLimits(float freq) {
    float min_freq = MIN_FREQ_EU;
    float max_freq = MAX_FREQ_EU;

    switch (_band_mode) {
        case JP_BAND:
            min_freq = MIN_FREQ_JP;
            max_freq = MAX_FREQ_JP;
            break;

        case EU_BAND:
            min_freq = MIN_FREQ_EU;
            max_freq = MAX_FREQ_EU;
            break;

        default:
            return freq;
            break;
    }

    if (freq > max_freq) {
        freq = max_freq;
        //printf("Warning: Frequency out of range. Setting to maximum value.\n");
    } else if (freq < min_freq) {
        freq = min_freq;
        //printf("Warning: Frequency out of range. Setting to minimum value.\n");
    }

    return freq;
}

void tea5767_i2c::tea5767_setStation(float freq) {
    _frequency = tea5767_checkFreqLimits(freq);
    tea5767_write_registers();
}

void tea5767_i2c::tea5767_setStationInc(float freq) {
    // Set search mode based on sign of freq
    if (freq < 0) {
        tea5767_setSearch(1, 0);
    } else {
        tea5767_setSearch(1, 1);
    }

    // Calculate new frequency and verify it's within limits
    float new_freq = _frequency + freq;
    _frequency = tea5767_checkFreqLimits(new_freq);

    // Update registers with new frequency
    tea5767_write_registers();
}

void tea5767_i2c::tea5767_setMute(bool mute) {
    _mute_mode = mute;
    tea5767_write_registers();
}

void tea5767_i2c::tea5767_setMuteLeft(bool mute) {
    _muteLmode = mute;
    tea5767_write_registers();
}

void tea5767_i2c::tea5767_setMuteRight(bool mute) {
    _muteRmode = mute;
    tea5767_write_registers();
}

void tea5767_i2c::tea5767_setStandby(bool standby) {
    _standby = standby;
    tea5767_write_registers();
}

void tea5767_i2c::tea5767_setStereo(bool stereo) {
    _stereoMode = !stereo;
    tea5767_write_registers();
}


