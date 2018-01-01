# OneBit

**Bit operations and bitfields at type-level**

Providing bit fields that can cross boundaries of the base units and that can even span across multiple units.

The grouping of bits is important because it will allow us to use the functionality of direct port access on an MCU.

This way a write operation is atomic and well identified resulting on a single read/write operation over multiple bits.

Bit-fields are discrete and values set/get do not have to take into account its position inside the unit.


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
typedef Bits<uint8_t,size_t,0x23,5,1> Led;

Led::begin();
Led::on();
```
