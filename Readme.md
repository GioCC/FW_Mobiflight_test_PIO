# FW_Mobiflight_test_PIO
Test repository forked from MobiFlight project (Firmware source only): PlatformIO conv + mods

Originally forked from Mobiflight base version:
BETA 9.0.0.4    2021-09-12

### Branch:
*encoder_mod* - Optimization of Encoder-related classes

### Purpose:
Memory optimization (major)

### Description: 
Each encoder currently uses a huge amount of memory, particularly through the inheritance chain MFEncoder <- RotaryEncoder <- TicksPerSecond; this latter class in particular has lots of long member variables. Currently, each encoder takes about 80 bytes; 10 Encoders already take 10% of MCU memory!

### Solution:
Turn a few attributes in the involved classes to static.
The max number of encoders has also been limited to a smaller value (from original 20 to max 10), although this is no longer strictly necessary and it can be increased again if required.

### Implementation notes:
Use a single, shared, static TicksPerSecond object common to all RotaryEncoders (one TicksPerSecond takes 35 bytes alone).
Class RotaryEncoder is replaced by the slightly modified RotaryEncoderShd, which uses the shared TicksPerSecond and also has some other members shared.

### Limitations:
Encoders can be now operated only one at a time (which reasonably is what happens anyway); otherwise, they should still work correctly (to be tested), however the timing for fast increment at higher rotation speeds will not work.

### Changes required in Mobiflight Connector or system:
none