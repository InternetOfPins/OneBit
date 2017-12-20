#include <iostream>
#include <bitset>
#include "src/OneBit.h"

using namespace std;
using namespace OneBit;

uint8_t data[]={0b11111111,0b11101011};

int main() {
  cout<<"Testing OneBit"<<endl;
  cout<<"test bpu - bits per unit ------------"<<endl;
  cout<<"byte:"<<(int)Bits<uint8_t>::bpu()<<endl;
  cout<<"word:"<<(int)Bits<uint16_t>::bpu()<<endl;
  cout<<"int:"<<(int)Bits<int>::bpu()<<endl;

  cout<<"address test bpu - bits per unit ------------"<<endl;
  cout<<"byte:"<<(int)Bits<uint8_t>::abpu()<<endl;
  cout<<"word:"<<(int)Bits<uint16_t>::abpu()<<endl;
  cout<<"int:"<<(int)Bits<int>::abpu()<<endl;

  cout<<"byte ones ------------"<<endl;
  cout<<"1:"<<bitset<8>(Bits<uint8_t>::ones(1))<<endl;
  cout<<"2:"<<bitset<8>(Bits<uint8_t>::ones(2))<<endl;
  cout<<"3:"<<bitset<8>(Bits<uint8_t>::ones(3))<<endl;
  cout<<"8:"<<bitset<8>(Bits<uint8_t>::ones(8))<<endl;

  cout<<"word ones ------------"<<endl;
  cout<<"1:"<<bitset<16>(Bits<uint16_t>::ones(1))<<endl;
  cout<<"2:"<<bitset<16>(Bits<uint16_t>::ones(2))<<endl;
  cout<<"3:"<<bitset<16>(Bits<uint16_t>::ones(3))<<endl;
  cout<<"16:"<<bitset<16>(Bits<uint16_t>::ones(16))<<endl;

  cout<<"mask ------------"<<endl;
  cout<<"0,0:"<<bitset<8>(Bits<uint8_t>::mask(0,0))<<endl;
  cout<<"0,1:"<<bitset<8>(Bits<uint8_t>::mask(0,1))<<endl;
  cout<<"0,2:"<<bitset<8>(Bits<uint8_t>::mask(0,2))<<endl;
  cout<<"2,3:"<<bitset<8>(Bits<uint8_t>::mask(2,3))<<endl;

  cout<<"idx ------------"<<endl;
  cout<<"0:"<<(int)Bits<uint8_t>::idx(0)<<endl;
  cout<<"7:"<<(int)Bits<uint8_t>::idx(7)<<endl;
  cout<<"8:"<<(int)Bits<uint8_t>::idx(8)<<endl;
  cout<<"12:"<<(int)Bits<uint8_t>::idx(12)<<endl;

  cout<<"pos ------------"<<endl;
  cout<<"0:"<<(int)Bits<uint8_t>::pos(0)<<endl;
  cout<<"7:"<<(int)Bits<uint8_t>::pos(7)<<endl;
  cout<<"8:"<<(int)Bits<uint8_t>::pos(8)<<endl;
  cout<<"12:"<<(int)Bits<uint8_t>::pos(12)<<endl;

  cout<<"get ------------"<<endl;
  cout<<"0,3:"<<bitset<8>(Bits<uint8_t>::get(data,0,3))<<endl;
  cout<<"2,4:"<<bitset<8>(Bits<uint8_t>::get(data,2,4))<<endl;
  cout<<"8,4:"<<bitset<8>(Bits<uint8_t>::get(data,8,4))<<endl;
  cout<<"9,4:"<<bitset<8>(Bits<uint8_t>::get(data,9,4))<<endl;
  cout<<"10,4:"<<bitset<8>(Bits<uint8_t>::get(data,10,4))<<endl;
  cout<<"11,4:"<<bitset<8>(Bits<uint8_t>::get(data,11,4))<<endl;

  cout<<"set ------------"<<endl;
  Bits<uint8_t>::set(data,0,3,0);
  cout<<"0,3=0:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  Bits<uint8_t>::set(data,0,3,5);
  cout<<"0,3=5:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  Bits<uint8_t>::set(data,4,3,5);
  cout<<"4,3=5:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  Bits<uint8_t>::set(data,0,8,0);
  cout<<"0,8=0:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  Bits<uint8_t>::set(data,8,8,0xF0);
  cout<<"8,8=0xF0:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;

  cout<<"on ------------"<<endl;
  Bits<uint8_t>::on(data,1,6);
  cout<<"1,6:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  Bits<uint8_t>::on(data,9,6);
  cout<<"9,6:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;

  cout<<"off ------------"<<endl;
  Bits<uint8_t>::off(data,2,4);
  cout<<"2,4:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  Bits<uint8_t>::off(data,10,4);
  cout<<"10,4:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;

  // cout<<"on ------------"<<endl;
  // Bits<uint8_t>::on(data,0,6);
  // cout<<"0,6:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;

  cout<<"cross-bouds get ------------"<<endl;
  cout<<"6,4:"<<bitset<8>(Bits<uint8_t>::get(data,6,4))<<endl;

  cout<<"cross-bouds set ------------"<<endl;
  Bits<uint8_t>::set(data,6,4,0xf);
  cout<<"6,4=0xf:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;

  cout<<"BitPart ------------"<<endl;
  typedef BitPart<uint8_t,0,16,uint16_t> M;
  M::set(data,0b1111110000111111);
  cout<<"set 0b1111110000111111:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  cout<<"get:"<<bitset<16>(M::get(data))<<endl;

  cout<<"BitField ------------"<<endl;
  typedef BitField<uint8_t,data,0,16,uint16_t> Md;
  cout<<"get:"<<bitset<16>(Md::get())<<endl;

}
