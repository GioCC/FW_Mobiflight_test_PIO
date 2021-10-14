// MFEncoder.cpp
//
// Copyright (C) 2013-2014

#include "MFEncoder.h"

encoderEvent    MFEncoder::_handler;

MFEncoder::MFEncoder() :_encoder() {
  _initialized = false;
}

void MFEncoder::attach(uint8_t pin1, uint8_t pin2, uint8_t encoderType, const char * name)
{
//   char ns[4];
  _pos   = 0;
  _name = name;
  //_pin1  = pin1;
  //_pin2  = pin2;
  _encoderType = encoderType;

  _encoder.initialize(pin1, pin2, encoderType);
  _encoder.setMinMax(MF_ENC_MIN,MF_ENC_MAX);
  _encoder.setPosition(_pos);

  _initialized = true;
}

void MFEncoder::update()
{
  if (!_initialized) return;

  //_encoder.update();
  _encoder.tick();
  long pos = _encoder.getPosition();

  if (pos == _pos) {
    // nothing happened
    return;
  }

  long delta = pos - _pos;
  long dir = 1;
  if (delta<0) dir = -1;

  long absDelta = abs(delta);
  if (absDelta < _fastLimit) {
    // slow turn detected
    if (_handler) { (*_handler)((dir==1 ? encLeft : encRight), (dir==1 ? _encoder.pin(1) : _encoder.pin(2)), _name); }
  } else {
    // fast turn detected
    if (_handler) { (*_handler)((dir==1 ? encLeftFast : encRightFast), (dir==1 ? _encoder.pin(1) : _encoder.pin(2)), _name); }
  }

  // clamp values
  if ( (dir > 0 && (pos + delta*2) > MF_ENC_MAX) || (dir < 0 && (pos - delta*2) < MF_ENC_MIN))
  {
    _encoder.setPosition(0);
    pos = 0;
  }
  _pos = pos;
}

void MFEncoder::attachHandler(encoderEvent newHandler)
{
  MFEncoder::_handler = newHandler;
}
