// MFEncoder.h
//
/// \mainpage MF Encoder module for MobiFlight Framework
/// \par Revision History
/// \version 1.0 Initial release
/// \author  Sebastian Moebius (mobiflight@moebiuz.de) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2013-2014 Sebastian Moebius

#ifndef MFEncoder_h
#define MFEncoder_h

#include <stdlib.h>
#include <Arduino.h>

///#include "../Button/Button.h"
///#include "../TicksPerSecond/TicksPerSecond.h"
///#include "../RotaryEncoderAcelleration/RotaryEncoderAcelleration.h"
//#include <RotaryEncoder.h>

#include <RotaryEncoderShd.h>

extern "C"
{
  typedef void (*encoderEvent) (uint8_t, uint8_t, const char *);
};

#define MF_ENC_MIN -32768
#define MF_ENC_MAX 32767
#define MF_ENC_FAST_LIMIT 512

enum
{
  encLeft,
  encLeftFast,
  encRight,
  encRightFast
};

/////////////////////////////////////////////////////////////////////
/// \class MFEncoder MFEncoder.h <MFEncoder.h>
class MFEncoder
{
public:
    MFEncoder(void);
    void attach(uint8_t pin1, uint8_t pin2, uint8_t encoderType, const char * name = "Encoder");
    void update(void);
    //static void attachHandler(uint8_t eventId, encoderEvent newHandler);
    static void attachHandler(encoderEvent newHandler);

protected:
    uint8_t    pins(uint8_t n) /*override*/ { return _encoder.pin(n+1); }  // implements MFPeripheral.pins(n)

private:
    bool                      _initialized;
    static encoderEvent       _handler; //[4];
    //uint8_t                   _pin1;  // contained in _encoder
    //uint8_t                   _pin2;  // contained in _encoder
    RotaryEncoderShd          _encoder;
    const char *              _name;
    long                      _pos;
    uint8_t                   _encoderType;
    uint16_t                  _fastLimit = MF_ENC_FAST_LIMIT;
};
#endif
