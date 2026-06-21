// OneBit mono_block tests
//
// Verifies OneBit components under the mono_block HAPI chain:
//   1. Bits<Unit,data>           — raw register get/set
//   2. Mask<MaskDesc>            — bit selection, preserves rest on set
//   3. BitField<shift,size>      — contiguous field, LSB aligned
//   4. Inverted<void>            — full inversion
//   5. Inverted<MaskDesc>        — selective inversion
//   6. MaskedInvert<MaskDesc>    — atomic select + invert
//   7. find<TagIs<Q>> on BitsDef — locate tagged component

#include <cassert>
#include <cstdint>
#include <iostream>
using namespace std;

#include <hapi/hapi.h>
#include <oneBit/oneBit.h>
using namespace hapi;
using namespace oneBit;

// ─── Hardware register simulators ────────────────────────────────────────────

static uint8_t  reg8  = 0;
static uint16_t reg16 = 0;

// ─── Test 1: Bits — raw get/set ──────────────────────────────────────────────

void test_bits() {
  using B = BitsDef<Bits<uint8_t, reg8>>;
  reg8 = 0;
  B b;
  b.set(0xAB);
  assert(reg8 == 0xAB);
  assert(b.get() == 0xAB);
  cout << "PASS test_bits\n";
}

// ─── Test 2: Mask — bit selection ────────────────────────────────────────────

void test_mask() {
  using B = BitsDef<Mask<MaskVal<0x0F>>, Bits<uint8_t, reg8>>;
  reg8 = 0b11110000;
  B b;
  // get: only low nibble
  assert(b.get() == 0x00);
  reg8 = 0b10101111;
  assert(b.get() == 0x0F);
  // set: preserves high nibble
  reg8 = 0b11110000;
  b.set(0x05);
  assert(reg8 == 0b11110101);
  cout << "PASS test_mask\n";
}

// ─── Test 3: BitField — contiguous field, LSB aligned ────────────────────────

void test_bitfield() {
  using B = BitsDef<BitField<2, 4>, Bits<uint8_t, reg8>>;
  reg8 = 0;
  B b;
  b.set(0b1010);                        // puts 1010 at bits [5:2]
  assert(reg8 == (0b1010 << 2));
  assert(b.get() == 0b1010);
  // preserves bits outside field
  reg8 = 0b11000011;
  b.set(0b0110);
  assert((reg8 & 0b11000011) == 0b11000011);
  assert(b.get() == 0b0110);
  cout << "PASS test_bitfield\n";
}

// ─── Test 4: Inverted<void> — full inversion ─────────────────────────────────

void test_inverted_full() {
  using B = BitsDef<Inverted<>, Bits<uint8_t, reg8>>;
  reg8 = 0;
  B b;
  assert(b.get() == 0xFF);              // 0 inverted
  b.set(0xFF);
  assert(reg8 == 0x00);                 // FF inverted → 0 stored
  b.set(0x0F);
  assert(reg8 == 0xF0);
  cout << "PASS test_inverted_full\n";
}

// ─── Test 5: Inverted<MaskDesc> — selective inversion ────────────────────────

void test_inverted_mask() {
  // Invert only low nibble (mask 0x0F)
  using B = BitsDef<Inverted<MaskVal<0x0F>>, Bits<uint8_t, reg8>>;
  reg8 = 0b10100101;
  B b;
  // get: high nibble unchanged, low nibble inverted
  uint8_t got = b.get();
  assert((got & 0xF0) == 0b10100000);   // high nibble unchanged
  assert((got & 0x0F) == 0b00001010);   // low nibble inverted
  cout << "PASS test_inverted_mask\n";
}

// ─── Test 6: MaskedInvert — atomic select + invert ───────────────────────────

void test_masked_invert() {
  using B = BitsDef<MaskedInvert<Pins<0,2,4>>, Bits<uint8_t, reg8>>;
  reg8 = 0b00010101;
  B b;
  // bits 0,2,4 are set; get inverts those → 0b00000000
  assert(b.get() == 0b00000000);
  reg8 = 0b00000000;
  assert(b.get() == 0b00010101);        // bits 0,2,4 inverted from 0
  cout << "PASS test_masked_invert\n";
}

// ─── Test 7: find<TagIs<Q>> on BitsDef ───────────────────────────────────────

struct HWTag {};

template<typename Unit, Unit& data>
struct TaggedBits : HWTag {
  template<typename O>
  struct Part : O {
    using Base = O; using Base::Base;
    using Type = Unit;
    static Unit get() noexcept { return data; }
    static void set(Unit v) noexcept { data = v; }
    static void rawClear(Unit bits) noexcept { data &= ~bits; }
    static void rawSet(Unit bits) noexcept { data |= bits; }
    static void rawWrite(Unit bits, Unit mask) noexcept {
      data = (data & ~mask) | (bits & mask);
    }
  };
};

void test_find_tagged() {
  using B = BitsDef<Mask<MaskVal<0x0F>>, TaggedBits<uint8_t, reg8>>;
  reg8 = 0;
  B b;
  // find the tagged bits component through the chain
  static_assert(query<TagIs<HWTag>, B::Types>, "HWTag should be in BitsDef types");
  auto& tb = hapi::find<TagIs<HWTag>>(b);
  tb.set(0xAB);
  assert(reg8 == 0xAB);
  cout << "PASS test_find_tagged\n";
}

// ─────────────────────────────────────────────────────────────────────────────

#ifdef ARDUINO
  void setup() {
    Serial.begin(115200);
    while (!Serial);
    test_bits();
    test_mask();
    test_bitfield();
    test_inverted_full();
    test_inverted_mask();
    test_masked_invert();
    test_find_tagged();
  }
  void loop() {}
#else
  int main() {
    test_bits();
    test_mask();
    test_bitfield();
    test_inverted_full();
    test_inverted_mask();
    test_masked_invert();
    test_find_tagged();
    cout << "\nAll tests passed.\n";
    return 0;
  }
#endif
