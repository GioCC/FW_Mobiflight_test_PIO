// -----
// RotaryEncoderShd.cpp - Library for using rotary encoders.
// This class is implemented for use with the Arduino environment.
//
// Derived from class:
// RotaryEncoder - Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
//
// This modified class uses a static shared TPS object in order to save memory (lots of it)
// when many encoders are used.
// It has the limitation that it must be assumed that the encoders are only activated one at a time.
// Requires an EXTENDED TickPerSecond class, having an additional update() method that takes a 'UnitNo'
// argument (unitNo), in order to reset counters when the TPS is switched.
// -----
// 18.01.2014 Original class created by Matthias Hertel
// 18.01.2018 New class derived by Giorgio Croci Candiani -
// -----

#include "Arduino.h"
#include "RotaryEncoderShd.h"
#include <TicksPerSecond.h>


// The array holds the values -1 for the entries where a position was decremented,
// a 1 for the entries where the position was incremented
// and 0 in all the other (no change or not valid) cases.

const int8_t KNOBDIR[] = {
  0, -1,  1,  0,
  1,  0,  0, -1,
  -1, 0,  0,  1,
  0,  1, -1,  0  };

// Configuration for different types of rotary encoders.
// For more information, refer to http://svglobe.com/arduino/in_encoder.html
//
// The detents in the encoder type settings below are indexed
// by value, not by the order in the quadrature cycle. For example, a rotary
// encoder with detents at 00 and 11 (positions 0 and 2 in the
// quadrature) but are indexed based on their decimal values (0 and 3).
// This allows simple lookup of detent positions by value.
const encoderType encoderTypes[] ={

	// 1 detents per cycle:  00 ,  10 , [11],  01
	{ { false, false, false, true }, 2 },

	// 1 detents per cycle: [00],  10 ,  11 ,  01
	{ { true, false, false, false }, 2 },

	// 2 detents per cycle: [00],  10 , [11],  01
	{ { true, false, false, true }, 1 },

	// 2 detents per cycle:  00 , [10],  11,  [01]
	{ { false, true, true, false }, 1 },

	// 4 detents per cycle: [00], [10], [11], [01]
	{ { true, true, true, true }, 0 },
};

TicksPerSecond   RotaryEncoderShd::_tps;

// ----- Initialization and Default Values -----

void RotaryEncoderShd::initialize(uint8_t pin1, uint8_t pin2, uint8_t encoder_type) {

    // Setup the input pins
    _pin1 = pin1;
    _pin2 = pin2;
    pinMode(_pin1, INPUT_PULLUP);
    pinMode(_pin2, INPUT_PULLUP);
    // when not started in motion, the current state of the encoder should be 3
    //_state = 3;

    // start with position 0;
    _state = 0;
    _position = 0;
    _positionExt = 0;

    _minValue = 0;
    _maxValue = 1000;

    _encoderType = encoderTypes[encoder_type];

    _tps.initialize();  // Shared TPS is reset whenever a new encoder is initialized
}


void RotaryEncoderShd::setMinMax(long min, long max) {
    _minValue = min;
    _maxValue = max;
}


int  RotaryEncoderShd::getPosition() {
    return _positionExt;
}

void RotaryEncoderShd::setPosition(int newPosition) {
    // only adjust the external part of the position.
    //_position = ((newPosition<<2) | (_position & 0x03));
    _position = ((newPosition >> _encoderType.resolutionShift) | (_position & 0x03));
    _positionExt = newPosition;
}

void RotaryEncoderShd::tick(void)
{
    _tps.update(_id, false);

    //uint8_t curState = _state;
    uint8_t newState;
    newState = (digitalRead(_pin1)==LOW ? 0x01 : 0);
    newState |= (digitalRead(_pin2)==LOW ? 0x02 : 0);

    long now = millis();
    if (newState != _state) {
        if (now - _lastToggleTime >= LINE_DEBOUNCE_MS) {
            // Button state has not changed for #debounce# milliseconds. Consider it is stable.
            int _speed = calcSpeed();
            _position += KNOBDIR[newState | (_state<<2)] * _speed;
            //if (newState == LATCHSTATE) {
            if (_encoderType.detents[newState]) {
                _tps.update(_id, true);
                //_positionExt = (_position >> 2);
                _positionExt = _position >> _encoderType.resolutionShift;
            }
            _state = newState;
        }
        _lastToggleTime = now;
    } else if (now - _lastToggleTime >= LINE_DEBOUNCE_MS) {
        // Forward the last toggle time a bit
        _lastToggleTime = now - LINE_DEBOUNCE_MS - 1;
    }
} // tick()

long RotaryEncoderShd::calcSpeed(void)
{
    int speed = constrain(_tps.getIntTPS_unsafe(), MIN_TPS, MAX_TPS) - MIN_TPS;
    long delta = max(1, (_maxValue - _minValue) / TICKS_AT_MAX_SPEED_FOR_FULL_SPAN);
    // Exponential acceleration - cubic (most comfortable)
    long step = 1 + delta * speed * speed * speed /
        ((MAX_TPS - MIN_TPS) * (MAX_TPS - MIN_TPS) * (MAX_TPS - MIN_TPS));
    return step;
}
// End
