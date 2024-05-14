/*
 * Joystick.h
 *
 *  Created on: Mar 26, 2024
 *      Author: Chris Howard
 */

#ifndef HAL_Joystick_H_
#define HAL_Joystick_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <HAL/Timer.h>

struct _Joystick
{
    uint_fast16_t x;
    uint_fast16_t y;

    bool isTappedtoLeft;
    bool isTappedtoRight;
    bool isTappedtoTop;
    bool isTappedtoBottom;


};
typedef struct _Joystick Joystick;

Joystick Joystick_construct();

/** Given a Joystick, determines if JS is pressed to the left */
bool Joystick_isPressedToLeft(Joystick* joystick_p);

/** Given a Joystick, determines if JS was "tapped" to left. Went from middle to left*/
bool Joystick_isTappedToLeft(Joystick* joystick_p);

/** Given a Joystick, determines if JS is pressed to the left */
bool Joystick_isPressedToRight(Joystick* joystick_p);

/** Given a Joystick, determines if JS is pressed to the left */
bool Joystick_isPressedToRight(Joystick* joystick_p);

/** Given a Joystick, determines if JS is pressed to the left */
bool Joystick_isPressedToTop(Joystick* joystick_p);

/** Given a Joystick, determines if JS is pressed to the left */
bool Joystick_isPressedToTop(Joystick* joystick_p);

/** Given a Joystick, determines if JS is pressed to the left */
bool Joystick_isPressedToBottom(Joystick* joystick_p);

/** Given a Joystick, determines if JS is pressed to the left */
bool Joystick_isPressedToBottom(Joystick* joystick_p);

/** Refreshes this Joystick so the Joystick FSM now has new outputs to interpret */
void Joystick_refresh(Joystick* Joystick);



#endif /* HAL_Joystick_H_ */
