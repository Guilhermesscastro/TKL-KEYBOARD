#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>
#include "usb_hid_keys.h"

// Configuration
#define MAX_COINCIDENT_KEYS 6
#define N_ROWS 12
#define N_COLS 8

#define BOOTLOADER_KEY KEY_B
// Timing configurations - reduce for lower latency
#define DEBOUNCE_INTERVAL_MS 5   // Was 10ms
#define DEBOUNCE_PRESS_TIME_MS 2 // Was 5ms

// Add new configuration
#define ADAPTIVE_DEBOUNCE_MIN_MS 2
#define ADAPTIVE_DEBOUNCE_MAX_MS 10
#define RAPID_PRESS_THRESHOLD_MS 50

// Encoder pins
#define REA_PIN 3
#define REB_PIN 2

// Encoder directions
#define CLOCKWISE 1
#define COUNTERCLOCKWISE 2

// Status codes
typedef enum {
    KEYBOARD_OK = 0,
    KEYBOARD_ERROR_TOO_MANY_KEYS = -1,
    KEYBOARD_ERROR_INIT_FAILED = -2,
    KEYBOARD_ERROR_ENCODER_FAILED = -3,
    KEYBOARD_ERROR_MATRIX_GHOSTING = -4,  // New
    KEYBOARD_ERROR_USB_DISCONNECT = -5    // New
} keyboard_status_t;

// Core initialization functions
keyboard_status_t keypins_init(void);
keyboard_status_t init_hid_task(void);
void encoder_init(void);

// Main task functions
void hid_task(void);
uint8_t encoder_task(void);

// Timer management
void setup_timer(void);
void timer_callback(uint alarm_num);

// Optional debugging
/* #ifdef DEBUG_MODE
void keyboard_diagnostic(void);
#endif */

// Internal helper functions (exposed for testing)
#ifdef TEST_MODE
int32_t read_encoder(void);
uint8_t poll_columns(int *columns, int *rows, int n_cols, int n_rows);
void send_hid_report(uint8_t report_id);
#endif

// External state indicators
extern volatile bool is_hid_task_ready;

// Configuration arrays
extern int config_row_map[N_ROWS];
extern int config_column_map[N_COLS];

// Add keycode type definition
typedef uint16_t keycode_t;

// Add this helper function declaration
void keycode_to_matrix_pos(keycode_t code, int *row, int *col);

#endif // KEYBOARD_H
