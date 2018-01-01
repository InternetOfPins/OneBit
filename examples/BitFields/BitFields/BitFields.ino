#include <OneBit.h>
using namespace OneBit;

uint8_t tmp[3]={0,0,0};
typedef Bits<uint8_t,uint8_t*,tmp,0> Bit0;//single bit @tmp
typedef Bits<uint8_t,uint8_t*,tmp,1,3> Bits1_3;// 3 bits @tmp starting at position 1

//arduino uno led: port B, bit #5
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
