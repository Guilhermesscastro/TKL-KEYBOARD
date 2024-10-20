#ifndef KEYBOARD_H

#define KEYBOARD_H

#define MAX_COINCIDENT_KEYS 6

#define N_ROWS 12
#define N_COLS 8

void keypins_init();
void init_hid_task();
void hid_task(void);

//#define ENCODER_SENSITIVITY 2  // Adjust this value to change sensitivity

#define DEBOUNCE_INTERVAL_MS 10

#define REA_PIN 3
#define REB_PIN 2

#define CLOCKWISE 1
#define COUNTERCLOCKWISE 2

void encoder_init();

#endif