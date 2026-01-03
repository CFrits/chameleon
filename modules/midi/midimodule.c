#include "py/runtime.h"
#include "py/mphal.h"
#include "py/objstr.h"

#include "midi.h"

static mp_obj_t py_midi_init() {
    midi_init();
    return mp_const_none;
}

static mp_obj_t py_midi_write(const mp_obj_t value) {
    uint32_t val;

    // Convert the object to an C integer.
    val = mp_obj_get_int(value);
    midi_transmit(val);

    return mp_const_none;
}

static mp_obj_t py_midi_read() {
    uint32_t val;

    val = midi_read();

    return mp_obj_new_int(val);
}

MP_DEFINE_CONST_FUN_OBJ_0(midi_init_obj, py_midi_init);
MP_DEFINE_CONST_FUN_OBJ_1(midi_write_obj, py_midi_write);
MP_DEFINE_CONST_FUN_OBJ_0(midi_read_obj, py_midi_read);

/*--------------------------------------------------------------------------------*/
/* Module.                                                                        */
/*--------------------------------------------------------------------------------*/

static const mp_rom_map_elem_t midi_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_midi) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&midi_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&midi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&midi_read_obj) },
};

static MP_DEFINE_CONST_DICT(midi_module_globals, midi_module_globals_table);

const mp_obj_module_t midi_user_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&midi_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_midi, midi_user_module);
