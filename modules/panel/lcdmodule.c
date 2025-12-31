//#include <string.h>

// Include MicroPython API.
#include "py/runtime.h"

// For function mp_hal_stdout_tx_str()
#include "py/mphal.h"
// https://micropython-usermod.readthedocs.io/en/latest/usermods_08.html
#include "py/objstr.h"

#include "uart.h"

static mp_obj_t py_lcd_clear() {

    uart_transmit(0x02);
    uart_transmit(0x03);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_0(lcd_clear_obj, py_lcd_clear);

static mp_obj_t py_lcd_print(const mp_obj_t row_in, const mp_obj_t col_in, const mp_obj_t string_in) {
    int row;
    int col;
    byte pos;

    if (mp_obj_get_type(string_in) != &mp_type_str) {
        mp_raise_TypeError(MP_ERROR_TEXT("Argument must be a string"));
        return MP_OBJ_NEW_SMALL_INT(0);
    }

    row = mp_obj_get_int(row_in);
    col = mp_obj_get_int(col_in);

    pos = (row << 5) | col;

    GET_STR_DATA_LEN(string_in, str, str_len);

    // LEN 0x04 POS ··· TEXT ···
    uart_transmit(str_len + 3);
    uart_transmit(0x04);
    uart_transmit(pos);

    int len = str_len;
    unsigned char *ptr = (unsigned char *) str;

    for (; len > 0; --len) {
        uart_transmit(*ptr++);
    }

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_3(lcd_print_obj, py_lcd_print);

/*--------------------------------------------------------------------------------*/
/* Module.                                                                        */
/*--------------------------------------------------------------------------------*/

static const mp_rom_map_elem_t lcd_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_lcd) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&lcd_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_print), MP_ROM_PTR(&lcd_print_obj) },
};

static MP_DEFINE_CONST_DICT(lcd_module_globals, lcd_module_globals_table);

const mp_obj_module_t lcd_user_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&lcd_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_lcd, lcd_user_module);
