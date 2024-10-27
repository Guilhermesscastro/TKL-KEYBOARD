#ifndef LED_H
#define LED_H

// GPIO pin number for the 1Hz blinking LED
#define LED_1HZ_PIN 17

/**
 * @brief Blink patterns in milliseconds for different device states.
 * 
 * - BLINK_NOT_MOUNTED : 250 ms interval when the device is not mounted
 * - BLINK_MOUNTED     : 500 ms interval when the device is mounted
 * - BLINK_SUSPENDED  : 2500 ms interval when the device is suspended
 */
#define BLINK_NOT_MOUNTED  250
#define BLINK_MOUNTED      500
#define BLINK_SUSPENDED   2500

// External variable representing the current LED blink interval
extern unsigned int blink_interval_ms;

/**
 * @brief Initializes the LED system by setting up GPIO pins.
 */
void led_init(void);

/**
 * @brief Controls the state of the board LED.
 * 
 * @param state Boolean value to turn the LED on (true) or off (false).
 */
void board_led_write(bool state);

/**
 * @brief Periodic task to handle LED blinking based on the current blink interval.
 */
void led_blinking_task(void);

#endif // LED_H
