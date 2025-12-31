//#include <string.h>

// Include MicroPython API.
#include "py/runtime.h"

// For function mp_hal_stdout_tx_str()
#include "py/mphal.h"
// https://micropython-usermod.readthedocs.io/en/latest/usermods_08.html
#include "py/objstr.h"

#include "uart.h"

//
// rtems_boolean panel_out_led(int ref, rtems_unsigned32 led_bits);
// #define PANEL01_LED_CTRL3    0x01000000
// #define PANEL01_LED_CTRL2    0x02000000
// #define PANEL01_LED_CTRL1    0x04000000
// #define PANEL01_LED_SHIFT    0x08000000
// #define PANEL01_LED_EDIT     0x10000000
//
static int currentleds = 0;

static mp_obj_t py_out_led(const mp_obj_t led_bits) {
    int allleds;
    byte leds;

    // Convert the object to an C integer.
    allleds = mp_obj_get_int(led_bits);

    leds = (byte) allleds & 0xFF;

    // LEN 0x02 LED1 [LED2 [LED3 [LED4]]]
    uart_transmit(0x03);
    uart_transmit(0x02);
    uart_transmit(leds);

    currentleds = leds;

    return mp_const_none;
}


static mp_obj_t py_ledon(const mp_obj_t lednr) {
    byte mask;
    byte leds;
    byte led;

    // Convert the object to an C integer.
    led = mp_obj_get_int(lednr);

    mask = 1 << led;
    
    leds = (byte) currentleds | mask;

    // LEN 0x02 LED1 [LED2 [LED3 [LED4]]]
    uart_transmit(0x03);
    uart_transmit(0x02);
    uart_transmit(leds);

    currentleds = leds;

    return mp_const_none;
}

static mp_obj_t py_ledoff(const mp_obj_t lednr) {
    byte mask;
    byte leds;
    byte led;

    // Convert the object to an C integer.
    led = mp_obj_get_int(lednr);

    mask = 1 << led;

    leds = (byte) currentleds & ~mask;

    // LEN 0x02 LED1 [LED2 [LED3 [LED4]]]
    uart_transmit(0x03);
    uart_transmit(0x02);
    uart_transmit(leds);

    currentleds = leds;

    return mp_const_none;
}



MP_DEFINE_CONST_FUN_OBJ_1(out_led_obj, py_out_led);
MP_DEFINE_CONST_FUN_OBJ_1(ledon_obj, py_ledon);
MP_DEFINE_CONST_FUN_OBJ_1(ledoff_obj, py_ledoff);

/*--------------------------------------------------------------------------------*/
/* Module.                                                                        */
/*--------------------------------------------------------------------------------*/

static const mp_rom_map_elem_t led_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_led) },
    { MP_ROM_QSTR(MP_QSTR_out), MP_ROM_PTR(&out_led_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&ledon_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&ledoff_obj) },
};

static MP_DEFINE_CONST_DICT(led_module_globals, led_module_globals_table);

const mp_obj_module_t led_user_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&led_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_led, led_user_module);
