// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ----------- //
// quadratureB //
// ----------- //

#define quadratureB_wrap_target 0
#define quadratureB_wrap 13

static const uint16_t quadratureB_program_instructions[] = {
            //     .wrap_target
    0x2020, //  0: wait   0 pin, 0                   
    0x00c6, //  1: jmp    pin, 6                     
    0xa049, //  2: mov    y, !x                      
    0x0084, //  3: jmp    y--, 4                     
    0xa02a, //  4: mov    x, !y                      
    0x0007, //  5: jmp    7                          
    0x0047, //  6: jmp    x--, 7                     
    0x20a0, //  7: wait   1 pin, 0                   
    0x00ca, //  8: jmp    pin, 10                    
    0x0040, //  9: jmp    x--, 0                     
    0xa049, // 10: mov    y, !x                      
    0x008c, // 11: jmp    y--, 12                    
    0xa02a, // 12: mov    x, !y                      
    0x0000, // 13: jmp    0                          
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program quadratureB_program = {
    .instructions = quadratureB_program_instructions,
    .length = 14,
    .origin = -1,
};

static inline pio_sm_config quadratureB_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + quadratureB_wrap_target, offset + quadratureB_wrap);
    return c;
}

static inline void quadratureB_program_init(PIO pio, uint sm, uint offset, uint a_pin, uint b_pin) {
    pio_sm_config c = quadratureB_program_get_default_config(offset);
    sm_config_set_in_pins(&c, a_pin);
    sm_config_set_jmp_pin(&c, b_pin);
    sm_config_set_in_shift(&c, false, true, 32);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

#endif

// ----------- //
// quadratureA //
// ----------- //

#define quadratureA_wrap_target 0
#define quadratureA_wrap 13

static const uint16_t quadratureA_program_instructions[] = {
            //     .wrap_target
    0x20a0, //  0: wait   1 pin, 0                   
    0x00c6, //  1: jmp    pin, 6                     
    0xa049, //  2: mov    y, !x                      
    0x0084, //  3: jmp    y--, 4                     
    0xa02a, //  4: mov    x, !y                      
    0x0007, //  5: jmp    7                          
    0x0047, //  6: jmp    x--, 7                     
    0x2020, //  7: wait   0 pin, 0                   
    0x00ca, //  8: jmp    pin, 10                    
    0x0040, //  9: jmp    x--, 0                     
    0xa049, // 10: mov    y, !x                      
    0x008c, // 11: jmp    y--, 12                    
    0xa02a, // 12: mov    x, !y                      
    0x0000, // 13: jmp    0                          
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program quadratureA_program = {
    .instructions = quadratureA_program_instructions,
    .length = 14,
    .origin = -1,
};

static inline pio_sm_config quadratureA_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + quadratureA_wrap_target, offset + quadratureA_wrap);
    return c;
}

static inline void quadratureA_program_init(PIO pio, uint sm, uint offset, uint a_pin, uint b_pin) {
    pio_sm_config c = quadratureA_program_get_default_config(offset);
    sm_config_set_in_pins(&c, b_pin);
    sm_config_set_jmp_pin(&c, a_pin);
    sm_config_set_in_shift(&c, false, true, 32);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

#endif
