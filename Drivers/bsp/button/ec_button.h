#ifndef EC_BUTTON_H
#define EC_BUTTON_H
#include "embedded_button.h"
#include "gpio.h"
#include "tim.h"

uint8_t read_button_pin(uint8_t button_id);
// Add your macro definitions and declarations here
#define  BUTTON_1 0x01
#endif // EC_BUTTON_H