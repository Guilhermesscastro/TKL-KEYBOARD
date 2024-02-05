#include "bsp/board.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_hid_keys.h"
#include "usb_descriptors.h"

#include "keyboard.h"

typedef uint16_t keycode_t;

keycode_t keybuffer[MAX_COINCIDENT_KEYS] = {0};
keycode_t modbuffer = 0;
keycode_t consumer_key = 0;

uint8_t position = 0;
uint8_t default_position = 0;

int config_row_map[N_ROWS] = {10, 11, 8, 9, 6, 7, 4, 5, 14, 15, 12, 13};
int config_column_map[N_COLS] = {25, 24, 23, 22, 21, 20, 19, 18};


static keycode_t keymap[N_ROWS][N_COLS] = {
    {KEY_ESC,         KEY_F2,           KEY_F4,     KEY_F6,      KEY_F8,        KEY_F10,          KEY_F12,          KEY_SCROLLLOCK},
    {KEY_F1,          KEY_F3,           KEY_F5,     KEY_F7,      KEY_F9,        KEY_F11,          KEY_PRINT_SCREEN, KEY_PAUSE},
    {KEY_GRAVE,       KEY_2,            KEY_4,      KEY_6,       KEY_8,         KEY_0,            KEY_EQUAL,        KEY_INSERT},
    {KEY_1,           KEY_3,            KEY_5,      KEY_7,       KEY_9,         KEY_MINUS,        KEY_BACKSPACE,    KEY_HOME},
    {KEY_TAB,         KEY_W,            KEY_R,      KEY_Y,       KEY_I,         KEY_P,            KEY_RIGHTBRACE,   KEY_DELETE},
    {KEY_Q,           KEY_E,            KEY_T,      KEY_U,       KEY_O,         KEY_LEFTBRACE,    KEY_ENTER,        KEY_END},
    {KEY_CAPSLOCK,    KEY_S,            KEY_F,      KEY_H,       KEY_K,         KEY_SEMICOLON,    KEY_HASHTILDE,    KEY_PAGEDOWN},
    {KEY_A,           KEY_D,            KEY_G,      KEY_J,       KEY_L,         KEY_APOSTROPHE,   KEY_NONE,         KEY_PAGEUP},
    {KEY_LEFTSHIFT,   KEY_X,            KEY_V,      KEY_N,       KEY_COMMA,     KEY_SLASH,        KEY_NONE,         KEY_UP},
    {KEY_Z,           KEY_C,            KEY_B,      KEY_M,       KEY_DOT,       KEY_RIGHTSHIFT,   KEY_NONE,         KEY_MUTE},
    {KEY_LEFTCTRL,    KEY_LEFTALT,      KEY_NONE,   KEY_NONE,    KEY_RIGHTALT,  KEY_PROPS,        KEY_LEFT,         KEY_RIGHT},
    {KEY_LEFTGUI,     KEY_NONE,         KEY_NONE,   KEY_SPACE,   KEY_NONE,      KEY_NONE,         KEY_RIGHTCTRL,    KEY_DOWN,}
};

static keycode_t coord2keycode(int column, int row) { return keymap[row][column]; }

static keycode_t code2mod(keycode_t code) {
    keycode_t mod = 0;
    switch (code) {
        case KEY_LEFTCTRL:  // HID_KEY_CONTROL_LEFT
            mod = KEYBOARD_MODIFIER_LEFTCTRL;
            break;
        case KEY_LEFTSHIFT: // HID_KEY_SHIFT_LEFT
            mod = KEYBOARD_MODIFIER_LEFTSHIFT;
            break;
        case KEY_LEFTALT:   // HID_KEY_ALT_LEFT
            mod = KEYBOARD_MODIFIER_LEFTALT;
            break;
        case KEY_RIGHTCTRL:  // HID_KEY_CONTROL_RIGHT
            mod = KEYBOARD_MODIFIER_RIGHTCTRL;
            break;
        case KEY_RIGHTSHIFT: // HID_KEY_SHIFT_RIGHT
            mod = KEYBOARD_MODIFIER_RIGHTSHIFT;
            break;
        case KEY_RIGHTALT:   // HID_KEY_ALT_RIGHT
            mod = KEYBOARD_MODIFIER_RIGHTALT;
            break;
    }
    return mod;
}


uint8_t poll_columns(int *columns, int *rows, int n_cols, int n_rows) 
{
    int current_key_index = 0;

    memset(keybuffer, 0x0, MAX_COINCIDENT_KEYS);
    for (int col = 0; col < n_cols; ++col) {
        gpio_put(columns[col], 1);
        sleep_us(10);
        for (int row = 0; row < n_rows; ++row) {
            int switch_status = gpio_get(rows[row]);
            if (switch_status) {
                if (current_key_index > MAX_COINCIDENT_KEYS) {
                    memset(keybuffer, 0x01, MAX_COINCIDENT_KEYS);
                    return -1; // too many keys pressed
                }
                
                keycode_t current_scancode = coord2keycode(col, row);
                
                keycode_t mod = code2mod(current_scancode);
                if(mod != 0){
                    if(modbuffer != (modbuffer | mod)){
                        modbuffer |= mod; 
                    }
                    continue;
                }
                
                if((current_scancode == KEY_MUTE)&&(consumer_key != KEY_MUTE)){
                    consumer_key = current_scancode;
                    
                    gpio_put(columns[col], 0);
                    return REPORT_ID_CONSUMER_CONTROL;
                    //continue;
                }
                
                keybuffer[current_key_index] = current_scancode;
                current_key_index++;

            }else {
                keycode_t current_scancode = coord2keycode(col, row);
                
                keycode_t mod = code2mod(current_scancode);
                if(mod != 0){
                    if(modbuffer != (modbuffer & (~mod))){
                        modbuffer &= (~mod); 
                    }
                }
                
                if((current_scancode == KEY_MUTE)&&(consumer_key != 0)){
                    consumer_key = 0;
                    
                    gpio_put(columns[col], 0);
                    return REPORT_ID_CONSUMER_CONTROL;
                    //continue;
                }
            }
        }
        gpio_put(columns[col], 0);
        sleep_us(10);
    }
    return REPORT_ID_KEYBOARD;
}

void keypins_init()
{
    //puts("Inside keypins_init");
    for (int i = 0; i < N_ROWS; ++i) {
        gpio_init(config_row_map[i]);
        gpio_set_dir(config_row_map[i], GPIO_IN);
        gpio_pull_down(config_row_map[i]);
    }

    //puts("after keypins_init rows_init");

    for (int i = 0; i < N_COLS; ++i) {
        gpio_init(config_column_map[i]);
        gpio_set_dir(config_column_map[i], GPIO_OUT);
        gpio_put(config_column_map[i], 0);
    }
    //puts("End keypins_init");
}

static void send_hid_consumer_report()
{
    if ( !tud_hid_ready() ) return;
    uint16_t consumer_keybuffer = 0;

    switch (consumer_key)
    {
        case KEY_VOLUMEDOWN:
            consumer_keybuffer = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
            break;
        case KEY_VOLUMEUP:
            consumer_keybuffer = HID_USAGE_CONSUMER_VOLUME_INCREMENT;
            break;
        case KEY_MUTE:
            consumer_keybuffer = HID_USAGE_CONSUMER_MUTE; //mute_was_pressed ? 0 : 
            break;
        case 0:
            consumer_keybuffer = 0;
            break;
    }
        
    tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &consumer_keybuffer, 2);
    
    /*if (consumer_keybuffer){}else{
        uint16_t empty_key = 0;
    
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
    }*/
}

static void send_hid_report(uint8_t report_id) 
{
    if ( !tud_hid_ready() ) return;
    
    switch (report_id)
    {
    case REPORT_ID_KEYBOARD:
        tud_hid_keyboard_report(report_id, modbuffer, keybuffer);
        break;
    case REPORT_ID_CONSUMER_CONTROL:
        send_hid_consumer_report();
        break;
    }
    
}

//encoder rotation to be implemented
#if 0
void encoder_callback(){
    
    uint8_t rea = gpio_get(REA_PIN);
    uint8_t reb = gpio_get(REB_PIN);
    uint8_t oldrea = rea;
    uint8_t oldreb = reb;

    while(1) {
       rea = gpio_get(REA_PIN);
       reb = gpio_get(REB_PIN);

        if (rea != oldrea) {
            if(reb!= rea) { 
                position++;
                
            }
            if (reb == rea) {
                position--;
            }
            
            oldrea = rea;
            oldreb = reb;

        }else{
            
            return ;
        }
        sleep_ms(1);
    }
}
uint8_t encoder_task(){
    if(position > default_position){
        consumer_key = KEY_VOLUMEUP;
        position--;

        return REPORT_ID_CONSUMER_CONTROL;

    }else if(position < default_position){
        consumer_key = KEY_VOLUMEDOWN;
        position++;
       
        return REPORT_ID_CONSUMER_CONTROL;

    }else if(consumer_key != (0 && KEY_MUTE)){
        consumer_key = 0;
        
        return REPORT_ID_CONSUMER_CONTROL;

    }else{
        return REPORT_ID_KEYBOARD;
    }
    
}



void encoder_init() {
    
    gpio_init(REA_PIN);
    gpio_set_dir(REA_PIN, GPIO_IN);
    gpio_init(REB_PIN);
    gpio_set_dir(REB_PIN, GPIO_IN)
    gpio_set_irq_enabled_with_callback(REA_PIN, GPIO_IRQ_EDGE_FALL, true, &encoder_callback);
    gpio_set_irq_enabled_with_callback(REB_PIN, GPIO_IRQ_EDGE_FALL, true);
    ;
}
#endif

// Every 10ms, we will send 1 report for each HID profile
// tud_hid_report_complete_cb() is used to send the next 
// report after the previous one is complete
void hid_task(void) 
{
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    uint8_t report_id = poll_columns(config_column_map, config_row_map, N_COLS, N_ROWS);
    /*if(report_id == REPORT_ID_KEYBOARD) {
            report_id = encoder_task();
        }*/
    send_hid_report(report_id);

}

//report selection with encoder
#if 0
// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len){
    (void) instance;
    (void) len;
    uint8_t next_report_id = report[0] + 2u;

  if (next_report_id == REPORT_ID_CONSUMER_CONTROL)
  {
    send_hid_consumer_report();
  }     
}
#endif