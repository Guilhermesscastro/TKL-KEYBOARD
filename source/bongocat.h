#ifndef BONGOCAT_H
#define BONGOCAT_H

#include "ssd1306.h"

// Declare the is_animating variable as external
extern bool is_animating;

extern bool is_tapping;

// Initialize Bongocat animation
void bongocat_init(ssd1306_t *disp);

// Update Bongocat animation
void update_bongocat_animation(ssd1306_t *disp);

// Trigger Bongocat animation
//void trigger_bongocat_animation();

//Trigger Bongocat tapping
void trigger_bongocat_tap();

#endif // BONGOCAT_H