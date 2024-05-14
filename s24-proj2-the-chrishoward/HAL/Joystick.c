/*
 * Joystick.c
 *
 *  Created on: Mar 26, 2024
 *      Author: Chris Howard
 */

#include <HAL/Joystick.h>

/* DEFINES */
#define LEFT_THRESHOLD 3000     //x
#define RIGHT_THRESHOLD 13000   //x
#define TOP_THRESHOLD 13000     //y
#define BOTTOM_THRESHOLD 3000   //y

enum _JoystickDebounceStateLeft { LEFT, NOT_LEFT };
typedef enum _JoystickDebounceStateLeft JoystickDebounceStateLeft;

enum _JoystickDebounceStateRight { RIGHT, NOT_RIGHT };
typedef enum _JoystickDebounceStateRight JoystickDebounceStateRight;

enum _JoystickDebounceStateTop { TOP, NOT_TOP };
typedef enum _JoystickDebounceStateTop JoystickDebounceStateTop;

enum _JoystickDebounceStateBottom { BOTTOM, NOT_BOTTOM };
typedef enum _JoystickDebounceStateBottom JoystickDebounceStateBottom;


// Initializing the ADC which resides on SoC
void initADC() {
    ADC14_enableModule();

    ADC14_initModule(ADC_CLOCKSOURCE_SYSOSC,
                     ADC_PREDIVIDER_1,
                     ADC_DIVIDER_1,
                      0
                     );

    // This configures the ADC to store output results
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);

    // This configures the ADC in manual conversion mode
    // Software will start each conversion.
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
}

void startADC() {
   // Starts the ADC with the first conversion
   // in repeat-mode, subsequent conversions run automatically
   ADC14_enableConversion();
   ADC14_toggleConversionTrigger();
}

// Interfacing the Joystick with ADC (making the proper connections in software)
void initJoyStick() {

    // This configures ADC_MEM0 to store the result from
    // input channel A15 (Joystick X), in non-differential input mode
    // (non-differential means: only a single input pin)
    // The reference for Vref- and Vref+ are VSS and VCC respectively
    ADC14_configureConversionMemory(ADC_MEM0,
                                  ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                  ADC_INPUT_A15,                 // joystick X
                                  ADC_NONDIFFERENTIAL_INPUTS);

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                                               GPIO_PIN0,
                                               GPIO_TERTIARY_MODULE_FUNCTION);

   //Added joystick Y
    ADC14_configureConversionMemory(ADC_MEM1,
                                      ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                      ADC_INPUT_A9,                 // joystick Y
                                      ADC_NONDIFFERENTIAL_INPUTS);

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                                                   GPIO_PIN4,
                                                   GPIO_TERTIARY_MODULE_FUNCTION);

}

// Constructs Joystick
// Initializes FSM
// Returns constructed joystick
Joystick Joystick_construct()
{
    // The Joystick object which will be returned at the end of construction
    Joystick Joystick;

    initADC();
    initJoyStick();
    startADC();

    // Initialize all buffered outputs of the Joystick
//    Joystick.pushState = RELEASED;
//    Joystick.isTapped = false;

    // Return the constructed Joystick object to the user
    return Joystick;
}

/*
 * Refreshes the input of the provided Joystick by polling its x and y for new input
 * @param joystick_p:   The Joystick object to refresh
 */
void Joystick_refresh(Joystick* joystick_p) {
    joystick_p->x = ADC14_getResult(ADC_MEM0);
    joystick_p->y = ADC14_getResult(ADC_MEM1);

    static JoystickDebounceStateLeft leftstate =    NOT_LEFT;
    static JoystickDebounceStateRight rightstate =  NOT_RIGHT;
    static JoystickDebounceStateTop topstate =      NOT_TOP;
    static JoystickDebounceStateBottom bottomstate =NOT_BOTTOM;
    joystick_p->isTappedtoLeft = false;
    joystick_p->isTappedtoRight = false;
    joystick_p->isTappedtoTop = false;
    joystick_p->isTappedtoBottom = false;

        switch (leftstate) {
            case NOT_LEFT:
                if (joystick_p->x < LEFT_THRESHOLD) {
                    leftstate = LEFT;
                    joystick_p->isTappedtoLeft = true;
                }
                break;
            case LEFT:
                if (joystick_p->x > LEFT_THRESHOLD) {
                    leftstate = NOT_LEFT;
                    joystick_p->isTappedtoLeft = false;
                }
        }
        switch (rightstate) {
                    case NOT_RIGHT:
                        if (joystick_p->x < RIGHT_THRESHOLD) {
                            rightstate = RIGHT;
                            joystick_p->isTappedtoRight = true;
                        }
                        break;
                    case RIGHT:
                        if (joystick_p->x > RIGHT_THRESHOLD) {
                            rightstate = NOT_RIGHT;
                            joystick_p->isTappedtoRight = false;
                        }
                }
        switch (topstate) {
                    case NOT_TOP:
                        if (joystick_p->y > TOP_THRESHOLD) {
                            topstate = TOP;
                            joystick_p->isTappedtoTop = true;
                        }
                        break;
                    case TOP:
                        if (joystick_p->y < TOP_THRESHOLD) {
                            topstate = NOT_TOP;
                            joystick_p->isTappedtoTop = false;
                        }
                }
        switch (bottomstate) {
                    case NOT_BOTTOM:
                        if (joystick_p->y < BOTTOM_THRESHOLD) {
                            bottomstate = BOTTOM;
                            joystick_p->isTappedtoBottom = true;
                        }
                        break;
                    case BOTTOM:
                        if (joystick_p->y > BOTTOM_THRESHOLD) {
                            bottomstate = NOT_BOTTOM;
                            joystick_p->isTappedtoBottom = false;
                        }
                }
}

// X AXIS FUNCTIONS
// check if joystick is pressed to the left
bool Joystick_isPressedtoLeft(Joystick* joystick_p) {

    // checks location of the x axis in regard to JS
    return (joystick_p->x < LEFT_THRESHOLD);
}

// check is joystick is tapped to the left
bool Joystick_isTappedToLeft(Joystick* joystick_p) {
    return (joystick_p->isTappedtoLeft);
}

// to the right
bool Joystick_isPressedtoRight(Joystick* joystick_p) {
    // checks location of the x axis in regard to JS
    return (joystick_p->x > RIGHT_THRESHOLD);
}

// check is joystick is tapped to the right
bool Joystick_isTappedToRight(Joystick* joystick_p) {
    return (joystick_p->isTappedtoRight);
}

// Y AXIS FUNCTIONS
// check if joystick is pressed upwards
bool Joystick_isPressedtoTop(Joystick* joystick_p) {
    // checks location of the y axis in regard to JS
    return (joystick_p->y > TOP_THRESHOLD);
}

// check is joystick is tapped to the top
bool Joystick_isTappedtoTop(Joystick* joystick_p) {
    return (joystick_p->isTappedtoTop);
}

// to the bottom
bool Joystick_isPressedtoBottom(Joystick* joystick_p) {
    // checks location of the y axis in regard to JS
    return (joystick_p->y < BOTTOM_THRESHOLD);
}

// check is joystick is tapped to the bottom
bool Joystick_isTappedtoBottom(Joystick* joystick_p) {
    return (joystick_p->isTappedtoBottom);
}



