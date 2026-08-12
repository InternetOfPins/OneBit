// Ported to the current oneBit API (namespace OneBit / Bits<Unit,PtrOrAddr,
// addr,bit,size> no longer exist -- see OneBit/examples/mono_block/src/main.cpp
// for the same BitsDef<BitField<shift,size>, Bits<Unit,data>> composition
// this file now follows for the RAM-array half below). All Bits<>/
// BitField<> methods are static, so no object instance is needed, same
// call style as the original.
#include <oneBit/oneBit.h>
using namespace oneBit;
using namespace hapi;

// A plain named variable, not an array element: Bits<>'s reference NTTP
// needs an id-expression naming an object with linkage -- tmp[0] doesn't
// qualify even though tmp itself does (confirmed by trying it). Only
// index 0 was ever used by the original typedefs anyway.
uint8_t tmp=0;
using Bit0    = BitsDef<Bits<uint8_t,tmp>>;               // whole byte; bit 0 read via get()&1
using Bits1_3 = BitsDef<BitField<1,3>,Bits<uint8_t,tmp>>; // 3 bits @tmp, pos 1

// arduino uno led: port B, bit #5. PINB/DDRB/PORTB expand to
// *(volatile uint8_t*)addr (avr-libc's SFR macros) -- not an id-expression,
// so they can't bind to Bits<>'s reference NTTP (Unit& data) at all
// ("has side-effects" / cv-qualification conflict, confirmed by trying it).
// Same reason OneChip's own real AVR drivers (e.g. chips/avr/avrSpi.h)
// never route hardware ports through oneBit::Bits<> either -- they use the
// raw bit-twiddling idiom below directly.
void setup() {
  DDRB |= (1<<PB5);//set led pin as output
}

void loop() {
  PORTB ^= (1<<PB5);//toggle led
  delay(500);
}
