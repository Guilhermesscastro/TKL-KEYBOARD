#ifndef BONGOCAT_H
#define BONGOCAT_H

#include "ssd1306.h" // SSD1306 OLED display driver

// External variables controlling the animation state
extern bool is_animating; // Flag indicating if an animation sequence is active
extern bool is_tapping;    // Flag indicating if a tap action is in progress

/**
 * @brief Initializes the Bongocat display by setting up the initial state.
 * 
 * @param disp Pointer to the SSD1306 display instance.
 */
void bongocat_init(ssd1306_t *disp);

/**
 * @brief Updates the Bongocat animation based on user interactions and time.
 * 
 * @param disp Pointer to the SSD1306 display instance.
 */
void update_bongocat_animation(ssd1306_t *disp);

/**
 * @brief Triggers a tapping action for Bongocat, alternating between paws.
 *        Implements debounce to prevent rapid multiple taps.
 */
void trigger_bongocat_tap();

#endif // BONGOCAT_H
