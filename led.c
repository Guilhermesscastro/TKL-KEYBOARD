#include "bsp/board.h"
#include "pico/stdlib.h"
#include "led.h"
const uint32_t LED_PIN = LED_1HZ_PIN;
//const uint32_t NUMLOCK_LED_PIN = 11;

unsigned int blink_interval_ms = BLINK_NOT_MOUNTED;
/*
bool numlock_on = false;

unsigned int led_pwm_on_us = 1;
unsigned int led_pwm_off_us = 10;
*/

void led_init(void){
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
     /*gpio_init(NUMLOCK_LED_PIN);
    gpio_set_dir(NUMLOCK_LED_PIN, GPIO_OUT);
    */
}

void board_led_write(bool state) 
{
    gpio_put(LED_PIN, state);
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // Blink every interval ms
  if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}

#if 0
void led_pwm_task(void)
{
    static uint32_t start_us = 0;
    if (numlock_on) {
        bool led_state = gpio_get(NUMLOCK_LED_PIN);
        if (led_state) {
            if ( board_us() - start_us < led_pwm_on_us) return; // not enough time
            start_us += led_pwm_on_us;
            gpio_put(NUMLOCK_LED_PIN, 0);
        } else {
            if ( board_us() - start_us < led_pwm_off_us) return; // not enough time
            start_us += led_pwm_off_us;
            gpio_put(NUMLOCK_LED_PIN, 1);
        }
    }
}
#endif

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}