#ifndef LED_H

#define LED_H

#define LED_1HZ_PIN 17

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
#define  BLINK_NOT_MOUNTED  250
#define  BLINK_MOUNTED 500
#define  BLINK_SUSPENDED 500


extern unsigned int blink_interval_ms;

void led_init(void);
void board_led_write(bool state);
void led_blinking_task(void);

#endif