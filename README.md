# OneBit

**Bit operations and bitfields from type-level-up**

**This project is in early development and should only be used by contributing developers. Expect substantial changes to the library API as it evolves. Contributions, ideas and PRs are very welcome.**

Providing bit fields that can cross boundaries of the base units and that can even span across multiple units.

The grouping of bits is important because it will allow us to use the functionality of direct port access on an MCU.

This way a write operation is atomic and well identified resulting on a single read/write operation over multiple bits.

Bit-fields are discrete and values set/get do not have to take into account its position inside the unit.

This is type-level only at the index and boundary checking, after that its not only runtime but `inlined`, making your code expert written.

## Operations

**get** return the bits value

**set** set the bits value

**on** set bits on

**off** set bits off

## Examples

**Simple Field**

```c++
//at global scope
uint8_t data[]={0,0};

typedef Bits<uint8_t, uint8_t*, data, 2> MyBit;

MyBit::on();
MyBit::off();
MyBit::set(true);
MyBit::set(0);
MyBit::get();
```

**multi-unit field** this field type, `MyField`, not only crossed the `uint8_t` base unit boundary but also has a size that exceeds the base unit size.

```c++
//at global scope
uint8_t data[]={0,0};

typedef Bits<uint8_t,uint8_t*,data,0,16,uint16_t> MyField;

MyField::set(data,0b1111110000111111);
MyField::get(data);

```

**hardware port address**
```c++
//arduino uno led: port B, bit #5 ------------------------
//portB input register @0x23
typedef Bits<uint8_t,size_t,0x23,5> LedIn;
//portB mode register @0x24
typedef Bits<uint8_t,size_t,0x24,5> LedMode;
//portB output register @0x25
typedef Bits<uint8_t,size_t,0x25,5> LedOut;

void setup() {
  LedMode::on();//set led pin as output
}

void loop() {
  LedOut::set(!LedIn::get());//toggle led
  delay(500);
}
```
