#define CFG_TUSB_OS OPT_OS_PICO

#include "bsp/board.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_hid_keys.h"
#include "hardware/i2c.h"
#include "pico-ssd1306/ssd1306.h"
#include "keyboard.h"
#include "bongocat.h"
#include "led.h"
#include "hardware/timer.h"
#include "hardware/irq.h"


/*uint64_tboard_us(void) 
{
    return to_us_since_boot(get_absolute_time());
}*/

ssd1306_t disp;

void display_setup(void) {
    i2c_init(i2c0, 400000);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);

    disp.external_vcc = false;
    ssd1306_init(&disp, 128, 32, 0x3C, i2c0);
    bongocat_init(&disp);
}


int main(void) 
{
    led_init();
    tusb_init();
    display_setup();
    encoder_init();
    init_hid_task();        // This calls keypins_init() internally

    while (1) {
        tud_task();
        hid_task();
        update_bongocat_animation(&disp);
    }
    return 0;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}


/*
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    (void) itf;
    (void) report_id;
    (void) report_type;

    tud_hid_report(0, buffer, bufsize);
}
*/

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    (void) itf;

    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        // Set keyboard LED e.g. CAPSLOCK, NUMLOCK, etc.
        if (bufsize < 1) return;
       //uint8_t const kbd_leds = buffer[0]; // on my machine it looks like the second byte does this
        uint8_t const kbd_leds = buffer[0];
        if (kbd_leds & KEYBOARD_LED_CAPSLOCK) {
            blink_interval_ms = 0;
            board_led_write(1);
        } else {
            blink_interval_ms = BLINK_MOUNTED;
            bongocat_init(&disp);
            board_led_write(0);
        }
    }
}
