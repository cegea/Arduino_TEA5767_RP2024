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
#include <hardware/gpio.h>
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "tea5767_i2c.h"

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/

/************************************
 * PRIVATE TYPEDEFS
 ************************************/

/************************************
 * STATIC VARIABLES
 ************************************/

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/

/************************************
 * STATIC FUNCTIONS
 ************************************/

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void tea5767_read_raw(TEA5757_t radio,uint8_t *buffer) {
    i2c_read_blocking(i2c_default, radio.address, buffer, TEA5767_REGISTERS, false);
}

void tea5767_write_registers(TEA5757_t radio) {
    uint8_t registers[TEA5767_REGISTERS];
    // Calculate the frequency value to be written to the TEA5767 register based on the current radio frequency in MHz. 
    // The calculation takes into account the fixed offset of 225kHz and the 4:1 prescaler used by the TEA5767 module.
    float freq = 4*(radio.frequency * 1000000 + 225000) / 32768; 
    int integer_freq = freq;
    registers[0] = integer_freq >> 8 | radio.mute_mode << 7 | radio.searchMode << 6;
    registers[1] = integer_freq & 0xff;
    registers[2] = radio.searchUpDown << 7 | radio.searchLevel << 5 | 1 << 4 | radio.stereoMode << 3
            | radio.muteRmode << 2 | radio.muteLmode << 1;
    registers[3] = radio.standby << 6 | radio.band_mode << 5 | 1 << 4 | radio.softMuteMode << 3 | radio.hpfMode << 2;
    registers[3] = registers[3] | radio.stereoNoiseCancelling << 1;
    registers[4] = 0x00;
    i2c_write_blocking(i2c_default, radio.address, registers, TEA5767_REGISTERS, false);
    // TODO: Use a timer instead.
    sleep_ms(100);
}

TEA5757_t tea5767_init(){
    TEA5757_t radio;
    radio.address = 0x60;
    // Default (See datasheet).
    //buf[0] = 0x40;buf[1] = 0x00;buf[2] = 0x90;buf[3] = 0x1E;buf[4] = 0x00;

    radio.mute_mode = false;
    radio.searchMode = false;
    radio.frequency = 102.7;
    radio.searchUpDown = 1;
    radio.searchLevel = ADC_HIGH;
    radio.stereoMode = true;
    radio.muteLmode = false;
    radio.muteRmode = false;
    radio.standby = false;
    radio.band_mode = EU_BAND;
    radio.softMuteMode = false;
    radio.hpfMode = true;
    radio.stereoNoiseCancelling = true;

    // TODO: Allow other pins than I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    return radio;
}

float tea5767_getStation(TEA5757_t *radio) {
    uint8_t buf[TEA5767_REGISTERS];

    // Read current settings from the TEA5767 module
    tea5767_read_raw(*radio,buf);

    // Calculate the current frequency based on the TEA5767's register values
    float integer_freq = (buf[0] & 0x3f) << 8 | buf[1];
    radio->frequency = (integer_freq*32768/4 - 225000) / 1000000;

    return radio->frequency;
}

int tea5767_getReady(TEA5757_t *radio) {
    uint8_t buf[TEA5767_REGISTERS];
    tea5767_read_raw(*radio,buf);
    radio->isReady = buf[0] >> 7;
    return radio->isReady;
}

void tea5767_setSearch(TEA5757_t *radio, uint8_t searchMode, uint8_t searchUpDown) {
    radio->searchUpDown = searchUpDown;
    radio->searchMode = searchMode;
    tea5767_write_registers(*radio);
}

float tea5767_checkFreqLimits(TEA5757_t radio, float freq) {
    float min_freq = MIN_FREQ_EU;
    float max_freq = MAX_FREQ_EU;

    switch (radio.band_mode) {
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

void tea5767_setStation(TEA5757_t *radio, float freq) {
    radio->frequency = tea5767_checkFreqLimits(*radio,freq);
    tea5767_write_registers(*radio);
}

void tea5767_setStationInc(TEA5757_t *radio, float freq) {
    // Set search mode based on sign of freq
    if (freq < 0) {
        tea5767_setSearch(radio, 1, 0);
    } else {
        tea5767_setSearch(radio, 1, 1);
    }

    // Calculate new frequency and verify it's within limits
    float new_freq = radio->frequency + freq;
    radio->frequency = tea5767_checkFreqLimits(*radio, new_freq);

    // Update registers with new frequency
    tea5767_write_registers(*radio);
}

void tea5767_setMute(TEA5757_t *radio, bool mute) {
    radio->mute_mode = mute;
    tea5767_write_registers(*radio);
}

void tea5767_setMuteLeft(TEA5757_t *radio, bool mute) {
    radio->muteLmode = mute;
    tea5767_write_registers(*radio);
}

void tea5767_setMuteRight(TEA5757_t *radio, bool mute) {
    radio->muteRmode = mute;
    tea5767_write_registers(*radio);
}

void tea5767_setStandby(TEA5757_t *radio, bool standby) {
    radio->standby = standby;
    tea5767_write_registers(*radio);
}

void tea5767_setStereo(TEA5757_t *radio, bool stereo) {
    radio->stereoMode = !stereo;
    tea5767_write_registers(*radio);
}


