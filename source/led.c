#include "bsp/board.h" // Board support package 
#include "pico/stdlib.h"  // Standard library for Raspberry Pi Pico
#include "led.h"          // LED module header


// Current LED blink interval in milliseconds, initialized to indicate device not mounted
unsigned int blink_interval_ms = BLINK_NOT_MOUNTED;

#if 0
Num Lock LED pin (currently unused)
const uint32_t NUMLOCK_LED_PIN = 11;
bool numlock_on = false; // Flag to indicate if Num Lock is active
#endif


/**
 * @brief Initializes the LED GPIO pins and sets their direction.
 */
void led_init(void){
    gpio_init(LED_PIN);                     // Initialize LED pin
    gpio_set_dir(LED_PIN, GPIO_OUT);        // Set LED pin as output
    
    /* Initialize Num Lock LED pin (currently unused)
    gpio_init(NUMLOCK_LED_PIN);
    gpio_set_dir(NUMLOCK_LED_PIN, GPIO_OUT);
    */
}

/**
 * @brief Sets the state of the board LED.
 * 
 * @param state Boolean value to turn the LED on (true) or off (false).
 */
void board_led_write(bool state) 
{
    gpio_put(LED_PIN, state); // Set LED pin high or low based on state
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

/**
 * @brief Callback invoked when the USB device is mounted.
 * 
 * Sets the LED blink interval to indicate the device is mounted.
 */
void tud_mount_cb(void)
{
    blink_interval_ms = BLINK_MOUNTED;
}

/**
 * @brief Callback invoked when the USB device is unmounted.
 * 
 * Sets the LED blink interval to indicate the device is not mounted.
 */
void tud_umount_cb(void)
{
    blink_interval_ms = BLINK_NOT_MOUNTED;
}

/**
 * @brief Callback invoked when the USB bus is suspended.
 * 
 * @param remote_wakeup_en Indicates if remote wakeup is enabled by the host.
 * 
 * Updates the LED blink interval to indicate suspension.
 */
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en; // Suppress unused parameter warning
    blink_interval_ms = BLINK_SUSPENDED;
}

/**
 * @brief Callback invoked when the USB bus is resumed.
 * 
 * Sets the LED blink interval to indicate the device is active again.
 */
void tud_resume_cb(void)
{
    blink_interval_ms = BLINK_MOUNTED;
}
