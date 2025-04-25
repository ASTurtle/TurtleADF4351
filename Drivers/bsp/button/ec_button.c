#include "ec_button.h"
#include "main.h"

uint8_t read_button_pin(uint8_t button_id)
{
    uint8_t pin_state = 0;
    switch (button_id)
    {
        case BUTTON_1:
            pin_state = HAL_GPIO_ReadPin(EC_SW_GPIO_Port, EC_SW_Pin); // Replace with actual GPIO pin for BUTTON_1
            break;
        // case BUTTON_2:
        //     pin_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1); // Replace with actual GPIO pin for BUTTON_2
        //     break;
        // Add more cases for additional buttons as needed
        default:
            pin_state = 0; // Invalid button ID
            break;
    }
    return pin_state;
}