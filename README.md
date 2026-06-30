# OneBit

**HAPI Compatibility:** Works with new Check/Apply/ApplyPack API (2026-Q2)

HAPI-compatible bit manipulation components. Compose over any `Unit`-typed storage (hardware registers or RAM variables).

## Mask descriptors

```cpp
MaskVal<0x0F>       // raw hex mask
Pins<1, 3, 5>       // from bit positions → (1<<1)|(1<<3)|(1<<5)
```

## Components

### `Bits<Unit, data>` — raw storage
Innermost component. Owns a reference to a register or variable.

```cpp
static Unit get()                           // read data
static void set(Unit v)                     // write data
// internal — pass through chain, not filtered by Mask:
static void rawSet(Unit bits)               // data |= bits
static void rawClear(Unit bits)             // data &= ~bits
static void rawWrite(Unit bits, Unit mask)  // data = (data & ~mask) | (bits & mask)
```

### `Mask<MaskDesc>` — bit selection
Narrows `get()`/`set()` to selected bits. `raw*` calls pass through unchanged.

```cpp
static constexpr Type maskBits()  // compiled mask value
static Type get()                 // Base::get() & maskBits()
static void set(Type v)           // rawWrite(v & mask, mask)
```

### `BitField<shift, size>` — contiguous field, LSB-aligned

```cpp
static constexpr Type fieldMask()  // ((1<<size)-1) << shift
static Type get()                  // (Base::get() & fieldMask()) >> shift
static void set(Type v)            // places v at bits [shift .. shift+size-1]
```

### `Inverted<MaskDesc = void>` — logical inversion

```cpp
// Inverted<>           — invert all bits
// Inverted<Pins<1,3>>  — invert only selected bits
static Type get()
static void set(Type v)
```

### `MaskedInvert<MaskDesc>` — select + invert, position-independent

```cpp
static Type get()        // selected bits inverted; unselected pass through
static void set(Type v)
```

## Chain closer

`BitsDef<Comp1, Comp2, ...>` assembles a concrete type with no separate terminal:

```cpp
using LedBit = BitsDef<Mask<Pins<5>>, Bits<uint8_t, portReg>>;
LedBit led;
led.set(led.maskBits());  // set bit 5
led.set(0);               // clear bit 5
```

## Layer order (outermost → innermost)

```
BitsDef  [optional]
Inverted / MaskedInvert
Mask<MaskDesc>  or  BitField<shift,size>
Bits<Unit, data>   ← always innermost
```
