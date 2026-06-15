/**
 * @file test.cpp
 * @brief OneBit unit tests — current oneBit:: API
 *
 * Tests:
 *   MaskVal, Pins — mask descriptor compile-time values
 *   Bits          — raw register get/set/rawClear/rawSet
 *   Mask          — bit-selection overlay
 *   BitField      — contiguous field, LSB-aligned get/set
 *   Inverted<>    — full-word inversion
 *   Inverted<M>   — partial inversion via mask descriptor
 *   MaskedInvert  — atomic select+invert
 */

#include <iostream>
#include <cstdint>
#include <cassert>
#include "../src/OneBit.h"

#ifdef ARDUINO
  #define cout Serial
  #define assert(x) do { if(!(x)) { Serial.print("FAIL: "); Serial.println(#x); } } while(0)
#else
  using namespace std;
#endif

using namespace oneBit;

inline uint8_t reg8   = 0;
inline uint16_t reg16 = 0;

void test_mask_val() {
  static_assert(MaskVal<0x0F>::value<uint8_t>()  == 0x0F);
  static_assert(MaskVal<0xFF>::value<uint16_t>() == 0xFF);
  cout << "MaskVal: ok" << endl;
}

void test_pins() {
  static_assert(Pins<0>::value<uint8_t>()     == 0x01);
  static_assert(Pins<0,1,2>::value<uint8_t>() == 0x07);
  static_assert(Pins<4,7>::value<uint8_t>()   == 0x90);
  cout << "Pins: ok" << endl;
}

void test_bits_raw() {
  using R = BitsDef<Bits<uint8_t, reg8>>;
  R r;
  reg8 = 0;
  r.set(0xAB);
  assert(r.get() == 0xAB);
  assert(reg8    == 0xAB);
  r.rawClear(0x0F);
  assert(reg8 == 0xA0);
  r.rawSet(0x05);
  assert(reg8 == 0xA5);
  cout << "Bits raw get/set/rawClear/rawSet: ok" << endl;
}

void test_mask() {
  using R = BitsDef<Mask<MaskVal<0x0F>>, Bits<uint8_t, reg8>>;
  R r;
  reg8 = 0xFF;
  assert(r.get() == 0x0F);       // masked read: low nibble
  r.set(0x05);
  assert(reg8 == 0xF5);           // high nibble preserved
  r.set(0x0A);
  assert(reg8 == 0xFA);
  cout << "Mask<MaskVal>: ok" << endl;
}

void test_mask_pins() {
  using R = BitsDef<Mask<Pins<0,1,3>>, Bits<uint8_t, reg8>>;
  R r;
  reg8 = 0xFF;
  assert(r.get() == 0b00001011);  // bits 0,1,3 selected
  r.set(0b00001011);
  assert(reg8 == 0xFF);           // unchanged (already set)
  r.set(0x00);
  assert(reg8 == 0b11110100);     // only selected bits cleared
  cout << "Mask<Pins>: ok" << endl;
}

void test_bitfield() {
  // BitField<2, 3>: bits 2..4 (shift=2, size=3), fieldMask = 0b00011100
  using R = BitsDef<BitField<2, 3>, Bits<uint8_t, reg8>>;
  R r;
  reg8 = 0;
  r.set(5);                       // 5 << 2 = 0x14
  assert(r.get() == 5);
  assert(reg8    == 0x14);
  reg8 = 0xFF;
  r.set(0);                       // clear field, preserve surrounding bits
  assert(r.get() == 0);
  assert(reg8    == 0b11100011);  // 0xFF & ~0x1C = 0xE3
  cout << "BitField<2,3>: ok" << endl;
}

void test_inverted_all() {
  using R = BitsDef<Inverted<>, Bits<uint8_t, reg8>>;
  R r;
  reg8 = 0x00;
  assert(r.get() == 0xFF);        // ~0 = 0xFF
  r.set(0x0F);                    // writes ~0x0F = 0xF0
  assert(reg8 == 0xF0);
  cout << "Inverted<> (all bits): ok" << endl;
}

void test_inverted_partial() {
  // Inverted<Pins<4,5>>: invert only bits 4 and 5; others pass through
  using R = BitsDef<Inverted<Pins<4,5>>, Bits<uint8_t, reg8>>;
  R r;
  reg8 = 0b00001111;
  // get: (reg & ~invMask) | (~reg & invMask)
  //    = (0x0F & 0xCF)   | (0xF0 & 0x30)
  //    = 0x0F            | 0x30   = 0x3F
  assert(r.get() == 0x3F);
  cout << "Inverted<Pins> (partial): ok" << endl;
}

void test_masked_invert() {
  using R = BitsDef<MaskedInvert<Pins<1,3>>, Bits<uint8_t, reg8>>;
  R r;
  reg8 = 0b00000000;
  // get: (0 & ~0x0A) | (~0 & 0x0A) = 0 | 0x0A = 0x0A
  assert(r.get() == 0x0A);
  reg8 = 0b11111111;
  // get: (0xFF & ~0x0A) | (~0xFF & 0x0A) = 0xF5 | 0 = 0xF5
  assert(r.get() == 0xF5);
  cout << "MaskedInvert<Pins>: ok" << endl;
}

void doTests() {
  test_mask_val();
  test_pins();
  test_bits_raw();
  test_mask();
  test_mask_pins();
  test_bitfield();
  test_inverted_all();
  test_inverted_partial();
  test_masked_invert();
  cout << "all OneBit tests passed" << endl;
}

#ifdef ARDUINO
  void setup() { Serial.begin(115200); while(!Serial); doTests(); }
  void loop() {}
#else
  int main() { doTests(); return 0; }
#endif
