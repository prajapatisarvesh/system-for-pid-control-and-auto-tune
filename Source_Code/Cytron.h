/*
 * Cytron.h
 *
 *  Created on: Jun 22, 2021
 *      Author: alpharomeo911
 *      This is the header file for cytron, this library is intended to use
 *      for operating cytron in serial packetized mode only.
 *
 *  To-Do:
 *      1. Make this library compatible with Serial Simplified
 *      2. Make this library compatible with Microcontroller Analog/PWM mode
 *
 *  ** Remember to configure the DIP switches! **
 */

#ifndef INC_CYTRON_H_
#define INC_CYTRON_H_

/*
 * The includes related to this class are below,
 *
 * In case you find any difficulties, you can contact me :
 * https://github.com/alpharomeo911
 *
 * Also I've tried to explain the code thoroughly in the .cpp file.
 */

#include "Arduino.h"

#define DUMMY_BIT 128
#define HEADER 85


class Cytron {
public:
    // Constructor for the cytron class
    Cytron(Stream* stream, uint8_t address, uint8_t channel);
    // Function to send the dummy bit
    void send_dummy_bit(void);
    void clockwise(uint8_t pwm);
    void anti_clockwise(uint8_t pwm);
    void rotate(int pwm);
    void brake();

private:
    Stream* stream;
    uint8_t address;
    uint8_t channel;
    uint8_t packet[4] = {HEADER, 0, 0, 0};
    uint8_t dummy_bit = DUMMY_BIT;
    void send_data(uint8_t speed);
};





#endif /* INC_CYTRON_H_ */
