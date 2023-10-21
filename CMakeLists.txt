add_library(tea5767_i2c
        tea5767_i2c.h
        tea5767_i2c.c)

target_link_libraries(tea5767_i2c pico_stdlib hardware_i2c hardware_gpio)
#add_executable(tea5767_i2c
 #       tea5767_i2c.c
  #      )

# pull in common dependencies
# target_link_libraries(tea5767_i2c pico_stdlib hardware_i2c)

# enable usb output, disable uart output
# pico_enable_stdio_usb(tea5767_i2c 1)
# pico_enable_stdio_uart(tea5767_i2c 0)

# create map/bin/hex file etc.
# pico_add_extra_outputs(tea5767_i2c)

# add url via pico_set_program_url
# example_auto_set_url(tea5767_i2c)
