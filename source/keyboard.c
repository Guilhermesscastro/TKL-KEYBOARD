#include "bsp/board.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_hid_keys.h"
#include "usb_descriptors.h"
#include "bongocat.h"
#include "keyboard.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "encoder.pio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include "pico/bootrom.h"

typedef uint16_t keycode_t;

// Struct to track the state of each key for debouncing
typedef struct {
    bool is_pressed;
    absolute_time_t last_change;
    uint8_t rapid_press_count;  // Counts rapid presses for potential features
} key_state_t;

// Global variables for key states and buffers
static key_state_t key_states[N_ROWS][N_COLS] = {0};             // Tracks state of each key in the matrix
keycode_t keybuffer[MAX_COINCIDENT_KEYS] = {0};                  // Buffer to store currently pressed keys
keycode_t modbuffer = 0;                                         // Buffer to store active modifier keys
keycode_t consumer_key = 0;                                     // Stores current consumer control key
static uint8_t active_timer = 0;                                // Active timer identifier

// PIO (Programmable I/O) for handling the encoder
PIO pio;
uint sm;

// Flag to indicate if the HID task is ready to send reports
volatile bool is_hid_task_ready = false;

// Pin configurations for rows and columns of the keyboard matrix
int config_row_map[N_ROWS] = {10, 11, 8, 9, 6, 7, 4, 5, 14, 15, 12, 13};    // GPIO pins for rows
int config_column_map[N_COLS] = {25, 24, 23, 22, 21, 20, 19, 18};           // GPIO pins for columns

// Keymap defining the keycodes for each key in the matrix
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

// Struct to track the state of the spacebar for special handling
static struct {
    bool is_pressed;
    absolute_time_t last_press;
    absolute_time_t last_release;
} spacebar_state = {false, {0}, {0}};

// Converts matrix coordinates to a keycode, with bounds checking
static keycode_t coord2keycode(int column, int row) { 
    if (row < 0 || row >= N_ROWS || column < 0 || column >= N_COLS) {
        return KEY_NONE;
    }
    return keymap[row][column]; 
}

// Converts a keycode to its corresponding modifier code
static keycode_t code2mod(keycode_t code) {
    keycode_t mod = 0;
    switch (code) {
        case KEY_LEFTCTRL:
            mod = KEYBOARD_MODIFIER_LEFTCTRL;
            break;
        case KEY_LEFTSHIFT:
            mod = KEYBOARD_MODIFIER_LEFTSHIFT;
            break;
        case KEY_LEFTALT:
            mod = KEYBOARD_MODIFIER_LEFTALT;
            break;
        case KEY_RIGHTCTRL:
            mod = KEYBOARD_MODIFIER_RIGHTCTRL;
            break;
        case KEY_RIGHTSHIFT:
            mod = KEYBOARD_MODIFIER_RIGHTSHIFT;
            break;
        case KEY_RIGHTALT:
            mod = KEYBOARD_MODIFIER_RIGHTALT;
            break;
        case KEY_LEFTGUI:
            mod = KEYBOARD_MODIFIER_LEFTGUI;
            break;
    }
    return mod;
}

// Detects ghosting in the key matrix to prevent multiple keypress issues
static bool detect_ghosting(int row1, int col1, int row2, int col2) {
    return (key_states[row1][col2].is_pressed && key_states[row2][col1].is_pressed);
}

// Finds the matrix position (row and column) for a given keycode
void keycode_to_matrix_pos(keycode_t code, int *row, int *col) {
    for (int r = 0; r < N_ROWS; r++) {
        for (int c = 0; c < N_COLS; c++) {
            if (keymap[r][c] == code) {
                *row = r;
                *col = c;
                return;
            }
        }
    }
    *row = -1;
    *col = -1;
}

// Polls the keyboard matrix to detect pressed keys with debouncing
uint8_t poll_columns(int *columns, int *rows, int n_cols, int n_rows) 
{
    int current_key_index = 0;
    absolute_time_t current_time = get_absolute_time();

    memset(keybuffer, 0x0, sizeof(keycode_t) * MAX_COINCIDENT_KEYS); // Clear key buffer
    
    // Iterate through each column
    for (int col = 0; col < n_cols; ++col) {
        gpio_put(columns[col], 1); // Activate current column
        sleep_us(1);               // Short delay for signal stabilization
        
        // Check each row in the current column
        for (int row = 0; row < n_rows; ++row) {
            int switch_status = gpio_get(rows[row]);  // Read row signal
            keycode_t current_scancode = coord2keycode(col, row); // Get keycode
            
            if (switch_status) {
                // Special handling for spacebar key
                if (current_scancode == KEY_SPACE) {
                    uint32_t time_since_last_release = absolute_time_diff_us(spacebar_state.last_release, current_time);
                    uint32_t time_since_last_press = absolute_time_diff_us(spacebar_state.last_press, current_time);
                    
                    // Prevent rapid re-triggering
                    if (time_since_last_release < 20000) { // 20ms minimum between releases
                        continue;
                    }
                    
                    // Adjust debounce time based on recent press duration
                    uint32_t debounce_time = (time_since_last_press > 150000) ? 5000 : 10000; // 5ms or 10ms
                    
                    if (!spacebar_state.is_pressed && time_since_last_release > debounce_time) {
                        spacebar_state.is_pressed = true;
                        spacebar_state.last_press = current_time;
                        keybuffer[current_key_index++] = current_scancode;
                        trigger_bongocat_tap(); // Trigger animation tap frame
                    }
                    continue;
                }

                // Handle modifier keys
                keycode_t mod = code2mod(current_scancode);
                if(mod != 0) {
                    if(modbuffer != (modbuffer | mod)) {
                        modbuffer |= mod;
                    }
                    continue;
                }

                // Handle consumer control keys like mute
                if(current_scancode == KEY_MUTE && consumer_key != KEY_MUTE) {
                    consumer_key = current_scancode;
                    gpio_put(columns[col], 0); // Deactivate column
                    return REPORT_ID_CONSUMER_CONTROL;
                }

                // Handle regular key presses
                if (current_key_index >= MAX_COINCIDENT_KEYS) {
                    gpio_put(columns[col], 0); // Deactivate column
                    return KEYBOARD_ERROR_TOO_MANY_KEYS;
                }

                keybuffer[current_key_index++] = current_scancode;
                trigger_bongocat_tap(); // Trigger animation or action
            } else {
                // Special handling for spacebar release
                if (current_scancode == KEY_SPACE) {
                    if (spacebar_state.is_pressed) {
                        spacebar_state.is_pressed = false;
                        spacebar_state.last_release = current_time;
                    }
                    continue;
                }

                // Handle key release for modifiers
                keycode_t mod = code2mod(current_scancode);
                if(mod != 0) {
                    if(modbuffer != (modbuffer & (~mod))) {
                        modbuffer &= (~mod);
                    }
                }

                // Handle release of consumer control keys
                if(current_scancode == KEY_MUTE && consumer_key != 0) {
                    consumer_key = 0;
                    gpio_put(columns[col], 0); // Deactivate column
                    return REPORT_ID_CONSUMER_CONTROL;
                }
            }
        }
        gpio_put(columns[col], 0); // Deactivate column after scanning
    }

    return REPORT_ID_KEYBOARD; // Return standard keyboard report
}

// Initializes the keyboard matrix pins and sets up GPIO directions
keyboard_status_t keypins_init() {
    // Initialize row GPIO pins as inputs with pull-down resistors
    for (int i = 0; i < N_ROWS; ++i) {
        gpio_init(config_row_map[i]);
        gpio_set_dir(config_row_map[i], GPIO_IN);
        gpio_pull_down(config_row_map[i]);
    }

    // Initialize column GPIO pins as outputs and set them low
    for (int i = 0; i < N_COLS; ++i) {
        gpio_init(config_column_map[i]);
        gpio_set_dir(config_column_map[i], GPIO_OUT);
        gpio_put(config_column_map[i], 0);
    }

    // Initialize key states to not pressed and set last change time
    for (int row = 0; row < N_ROWS; row++) {
        for (int col = 0; col < N_COLS; col++) {
            key_states[row][col].is_pressed = false;
            key_states[row][col].last_change = get_absolute_time();
        }
    }

    return KEYBOARD_OK;
}

// Calculates encoder acceleration based on rotation speed
static int32_t get_encoder_acceleration(int32_t diff, uint32_t time_delta_us) {
    static const struct {
        uint32_t threshold_us;
        float multiplier;
    } accel_levels[] = {
        {20000,  5.0f},  // Ultra fast
        {40000,  4.0f},  // Very fast
        {60000,  3.0f},  // Fast
        {100000, 2.0f},  // Medium
        {200000, 1.5f},  // Slow
    };
    
    for (int i = 0; i < sizeof(accel_levels)/sizeof(accel_levels[0]); i++) {
        if (time_delta_us < accel_levels[i].threshold_us) {
            return diff * accel_levels[i].multiplier;
        }
    }
    return diff;
}

// Initializes the encoder using PIO
void encoder_init() {
    pio = pio0;
    uint offset = pio_add_program(pio, &quadratureA_program);
    if (offset == -1) {
        return; // Handle error if program couldn't be added
    }
    
    sm = pio_claim_unused_sm(pio, true);
    if (sm == -1) {
        return; // Handle error if no state machine is available
    }

    quadratureA_program_init(pio, sm, offset, REA_PIN, REB_PIN); // Initialize encoder program
}

// Reads the current count from the encoder
int32_t read_encoder() {
    // Clear any stale data from the encoder's RX FIFO
    while (!pio_sm_is_rx_fifo_empty(pio, sm)) {
        pio_sm_get(pio, sm);
    }
    
    pio_sm_exec_wait_blocking(pio, sm, pio_encode_in(pio_x, 32)); // Execute IN instruction to read data
    return pio_sm_get_blocking(pio, sm); // Get the encoder count
}

// Handles encoder tasks, applying debouncing and acceleration
uint8_t encoder_task() {
    static int32_t last_count = 0;
    static absolute_time_t last_change_time = {0};
    static uint8_t consecutive_changes = 0;
    static bool encoder_active = false;
    
    absolute_time_t current_time = get_absolute_time();
    uint32_t time_delta = absolute_time_diff_us(last_change_time, current_time);

    int32_t current_count = read_encoder();
    int32_t diff = current_count - last_count;

    // Reset consecutive changes if no activity for a while
    if (time_delta > 200000) { // 200ms timeout
        consecutive_changes = 0;
        encoder_active = false;
    }

    if (diff != 0) {
        // Basic debounce: ignore changes that are too rapid
        if (time_delta > DEBOUNCE_INTERVAL_MS * 1000) {
            int32_t accel_diff = get_encoder_acceleration(diff, time_delta);
            
            // Update consecutive changes counter for acceleration
            if (time_delta < 100000) { // 100ms window
                consecutive_changes = (consecutive_changes < 255) ? consecutive_changes + 1 : 255;
                encoder_active = true;
            }

            // Apply additional acceleration if sustained rotation
            if (consecutive_changes > 5 && encoder_active) {
                accel_diff *= 1.5;
            }

            if (accel_diff > 0) {
                consumer_key = KEY_VOLUMEUP; // Increase volume
                last_count = current_count;
                last_change_time = current_time;
                return REPORT_ID_CONSUMER_CONTROL;
            } else if (accel_diff < 0) {
                consumer_key = KEY_VOLUMEDOWN; // Decrease volume
                last_count = current_count;
                last_change_time = current_time;
                return REPORT_ID_CONSUMER_CONTROL;
            }
        }
    } else {
        // Reset consumer key if no encoder movement
        if (consumer_key == KEY_VOLUMEUP || consumer_key == KEY_VOLUMEDOWN) {
            consumer_key = 0;
            return REPORT_ID_CONSUMER_CONTROL;
        }
    }

    return REPORT_ID_KEYBOARD; // No consumer control action
}

// Sends a consumer HID report based on the active consumer key
static void send_hid_consumer_report() {
    if (!tud_hid_ready()) return; // Ensure HID is ready to send

    uint16_t consumer_keybuffer = 0;

    switch (consumer_key) {
        case KEY_VOLUMEDOWN:
            consumer_keybuffer = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
            break;
        case KEY_VOLUMEUP:
            consumer_keybuffer = HID_USAGE_CONSUMER_VOLUME_INCREMENT;
            break;
        case KEY_MUTE:
            consumer_keybuffer = HID_USAGE_CONSUMER_MUTE;
            break;
        default:
            consumer_keybuffer = 0;
            break;
    }
        
    tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &consumer_keybuffer, 2); // Send consumer report
}

// Sends the appropriate HID report based on the report ID
static void send_hid_report(uint8_t report_id) {
    if (!tud_hid_ready()) return; // Ensure HID is ready to send
    
    switch (report_id) {
        case REPORT_ID_KEYBOARD: {
            // Prepare key buffer excluding modifier keys
            keycode_t cleaned_keybuffer[MAX_COINCIDENT_KEYS] = {0};
            int clean_index = 0;
            
            for (int i = 0; i < MAX_COINCIDENT_KEYS && keybuffer[i] != 0; i++) {
                // Skip adding modifier keys to the cleaned buffer
                if (code2mod(keybuffer[i]) == 0) {
                    cleaned_keybuffer[clean_index++] = keybuffer[i];
                }
            }
            
            tud_hid_keyboard_report(report_id, modbuffer, cleaned_keybuffer); // Send keyboard report
            break;
        }
        case REPORT_ID_CONSUMER_CONTROL:
            send_hid_consumer_report(); // Send consumer control report
            break;
    }
}

// Callback function triggered by the hardware timer
void timer_callback(uint alarm_num) {
    is_hid_task_ready = true; // Flag that the HID task can proceed
}

// Sets up a hardware timer for periodic HID tasks
void setup_timer(void) {
    hardware_alarm_claim(0); // Claim hardware alarm 0
    hardware_alarm_set_callback(0, timer_callback); // Set callback function
    hardware_alarm_set_target(0, make_timeout_time_ms(DEBOUNCE_INTERVAL_MS)); // Set timer target
    active_timer = 0;  // Initialize active timer ID
}

// Checks if the bootloader key combination is pressed to enter bootloader mode
static bool check_bootloader_combo(void) {
    // Check if Left Control + Left Shift are held
    if ((modbuffer & (KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_LEFTSHIFT)) == 
        (KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_LEFTSHIFT)) {
        
        // Verify if the bootloader key is pressed
        for (int i = 0; i < MAX_COINCIDENT_KEYS; i++) {
            if (keybuffer[i] == BOOTLOADER_KEY) {
                return true;
            }
        }
    }
    return false;
}

// Main HID task that polls the keyboard and encoder, then sends HID reports
void hid_task(void) {
    static uint32_t error_count = 0;
    static absolute_time_t last_error_time = {0};

    if (!is_hid_task_ready) {
        return; // Exit if HID task is not ready
    }
    
    is_hid_task_ready = false; // Reset the flag

    // Poll the keyboard matrix for pressed keys
    uint8_t report_id = poll_columns(config_column_map, config_row_map, N_COLS, N_ROWS);
    
    // Check if the bootloader key combination is pressed
    if (check_bootloader_combo()) {
        reset_usb_boot(0, 0);  // Reset device into USB bootloader mode
    }

    // Handle potential polling errors
    if (report_id < 0) {
        error_count++;
        if (error_count > 100) {
            absolute_time_t current_time = get_absolute_time();
            if (absolute_time_diff_us(last_error_time, current_time) > 1000000) {
                // Reset error counter every second
                error_count = 0;
                last_error_time = current_time;
            } else {
                // Reinitialize the keyboard on excessive errors
                keypins_init();
                error_count = 0;
            }
        }
        return;
    }

    // If no keyboard activity, check the encoder
    if (report_id == REPORT_ID_KEYBOARD) {
        report_id = encoder_task();
    }
    
    // Send the appropriate HID report
    send_hid_report(report_id);

    // Reset the timer for the next HID task interval
    hardware_alarm_set_target(active_timer, make_timeout_time_ms(DEBOUNCE_INTERVAL_MS));
}

// Initializes the HID task by setting up keyboard pins, timers, and buffers
keyboard_status_t init_hid_task(void) {
    // Initialize keyboard matrix pins
    keyboard_status_t status = keypins_init();
    if (status != KEYBOARD_OK) {
        return status;
    }

    // Initialize hardware timer
    setup_timer();

    // Clear key and modifier buffers
    memset(keybuffer, 0, sizeof(keycode_t) * MAX_COINCIDENT_KEYS);
    modbuffer = 0;
    consumer_key = 0;

    // Reset HID task state
    is_hid_task_ready = false;

    // Initialize spacebar state
    spacebar_state.is_pressed = false;
    spacebar_state.last_press = get_absolute_time();
    spacebar_state.last_release = get_absolute_time();

    return KEYBOARD_OK;
}
