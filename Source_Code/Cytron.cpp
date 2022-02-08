/*
 * Cytron.cpp
 *
 *  Created on: Jun 11, 2021
 *      Author: alpharomeo911
 */

#include "Cytron.h"

/*
 * The constructor for this class
 *
 * This accepts 3 arguments,
 *  1. The UART on which you are planning to use the Cytron
 *  2. The address of the motor which is configured via DIP switch
 *     for additional information read the datasheet :
 *     https://www.robotshop.com/media/files/pdf/smartdriveduo-smart-dual-channel-30a-motor-driver-datasheet.pdf
 *  3. And the channel (again read the datasheet, if you are not aware what is this).
 *
 *
 */

Cytron::Cytron(Stream* stream, uint8_t address, uint8_t channel) {
  this->stream = stream;
  this->address = address;
  this->channel = channel;
}


/*
 * The send_dummy_bit is to initialize all the cytrons connected.
 * Call this function only if you have initialized all the cytrons
 * you will be using, and plan to use no further cytron.
 *
 * This function comes with a cost of 500ms delay, so keep in mind that
 * not to use any time-sensitive operations while calling this function.
 */

void Cytron::send_dummy_bit(void) {
  stream->write(dummy_bit);
  delay(500);
}

/*
 * Below given are the clockwise and anti_clockwise functions.
 *
 * Arguments required:
 *  pwm value in a range of 0 to 255
 *
 * Since running in serial packetized mode, we can only pass 0 - 255 to a
 * single motor, so if we want to run the motor in forward direction, we pass 255
 * else 0, and to stop we pass 127. So here we are simply calculating the slope,
 * and scaling the passed pwm. This is something similar to what map() does in an
 * arduino.
 *
 * And finally we call the send_data function, which finally transmits the packet.
 */

void Cytron::clockwise(uint8_t pwm) {
  pwm = map(pwm, 0, 255, 127, 255);
  this->send_data(pwm);
}

void Cytron::anti_clockwise(uint8_t pwm) {
  pwm = map(pwm, 0, 255, 127, 0);
  this->send_data(pwm);
}

void Cytron::brake(void) {
  this->send_data(127);
}

/**
 * 
 * For using it with PID Class
 * 
 * Pass it -255 -> 255 PWM
 * 
 * 
*/
void Cytron::rotate(int pwm) {
  pwm = map(pwm, -255, 255, 0, 255);
  this->send_data(pwm);
}

/*
 * The send_data function
 *
 * Arguments -
 *  1. PWM value
 *
 * We already have an array, where the first block stores the header, i.e. 85
 * so, what we need to do is calculate the last 3 blocks and send the packet to the
 * cytron.
 *
 * We calculate the packet 1 by left shifting the channel 3 times, and simply performing
 * an OR operation with the shifted channel value,
 *
 * Example Channel is 1 and address is 101
 * So if we perform three times right shift we get 1000 (8) and finally, we perform
 * OR operation, we get 1101 (13), which is the required value.
 *
 * The packet 2 is the PWM, and finally the packet 3 is the checksum, which is the
 * addition of first three packets.
 */

void Cytron::send_data(uint8_t speed) {
  this->packet[1] = (this->channel << 3) | this->address;
  this->packet[2] = speed;
  this->packet[3] = this->packet[0] + this->packet[1] + this->packet[2];

  for(uint8_t i = 0; i < 4; i++) {
    this->stream->write(packet[i]);
  }
}
