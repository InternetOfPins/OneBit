#include <iostream>
#include <bitset>
#include "../src/OneBit.h"

using namespace std;
using namespace OneBit;

uint8_t data[]={0b11101101,0b11101011};

int main() {
  cout<<"Testing OneBit"<<endl;
  cout<<"test bpu - bits per unit ------------"<<endl;
  cout<<"byte:"<<(int)Bits<uint8_t,data,0,8>::bpu()<<endl;
  cout<<"word:"<<(int)Bits<uint16_t,data,0,16>::bpu()<<endl;
  cout<<"int:"<<(int)Bits<int,data,0,8>::bpu()<<endl;

  cout<<"address test bpu - bits per unit ------------"<<endl;
  cout<<"byte:"<<(int)Bits<uint8_t,data,0,8>::abpu()<<endl;
  cout<<"word:"<<(int)Bits<uint16_t,data,0,16>::abpu()<<endl;
  cout<<"int:"<<(int)Bits<int,data,0>::abpu()<<endl;

  cout<<"byte ones ------------"<<endl;
  cout<<"1:"<<bitset<8>(Bits<uint8_t,data,0,1>::ones())<<endl;
  cout<<"2:"<<bitset<8>(Bits<uint8_t,data,0,2>::ones())<<endl;
  cout<<"3:"<<bitset<8>(Bits<uint8_t,data,5,3>::ones())<<endl;
  cout<<"8:"<<bitset<8>(Bits<uint8_t,data,0,8>::ones())<<endl;

  cout<<"word ones ------------"<<endl;
  cout<<"1:"<<bitset<16>(Bits<uint16_t,data,0,1>::ones())<<endl;
  cout<<"2:"<<bitset<16>(Bits<uint16_t,data,0,2>::ones())<<endl;
  cout<<"3:"<<bitset<16>(Bits<uint16_t,data,0,3>::ones())<<endl;
  cout<<"16:"<<bitset<16>(Bits<uint16_t,data,0,16>::ones())<<endl;

  cout<<"mask ------------"<<endl;
  cout<<"0,0:"<<bitset<8>(Bits<uint8_t,data,0,0>::mask())<<endl;
  cout<<"0,1:"<<bitset<8>(Bits<uint8_t,data,0,1>::mask())<<endl;
  cout<<"0,2:"<<bitset<8>(Bits<uint8_t,data,0,2>::mask())<<endl;
  cout<<"2,3:"<<bitset<8>(Bits<uint8_t,data,2,3>::mask())<<endl;

  cout<<"idx ------------"<<endl;
  cout<<"0:"<<(int)Bits<uint8_t,data,0,1>::idx()<<endl;
  cout<<"7:"<<(int)Bits<uint8_t,data,7,1>::idx()<<endl;
  cout<<"8:"<<(int)Bits<uint8_t,data,8,1>::idx()<<endl;
  cout<<"12:"<<(int)Bits<uint8_t,data,12,1>::idx()<<endl;

  cout<<"pos ------------"<<endl;
  cout<<"0:"<<(int)Bits<uint8_t,data,1,1>::pos()<<endl;
  cout<<"7:"<<(int)Bits<uint8_t,data,7,1>::pos()<<endl;
  cout<<"8:"<<(int)Bits<uint8_t,data,8,1>::pos()<<endl;
  cout<<"12:"<<(int)Bits<uint8_t,data,12,1>::pos()<<endl;

  cout<<"get ------------"<<endl;
  cout<<"0,3:"<<bitset<8>(Bits<uint8_t,data,0,3>::get())<<endl;
  cout<<"2,4:"<<bitset<8>(Bits<uint8_t,data,2,4>::get())<<endl;
  cout<<"8,4:"<<bitset<8>(Bits<uint8_t,data,8,4>::get())<<endl;
  cout<<"9,4:"<<bitset<8>(Bits<uint8_t,data,9,4>::get())<<endl;
  cout<<"10,4:"<<bitset<8>(Bits<uint8_t,data,10,4>::get())<<endl;
  cout<<"11,4:"<<bitset<8>(Bits<uint8_t,data,11,4>::get())<<endl;

  cout<<"set ------------"<<endl;
  Bits<uint8_t,data,0,3>::set(0);
  cout<<"0,3=0:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  Bits<uint8_t,data,0,3>::set(5);
  cout<<"0,3=5:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  Bits<uint8_t,data,4,3>::set(5);
  cout<<"4,3=5:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  Bits<uint8_t,data,0,8>::set(0);
  cout<<"0,8=0:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  Bits<uint8_t,data,8,8>::set(0xF0);
  cout<<"8,8=0xF0:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;

  // cout<<"on ------------"<<endl;
  // Bits<uint8_t>::on(data,1,6);
  // cout<<"1,6:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  // Bits<uint8_t>::on(data,9,6);
  // cout<<"9,6:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  //
  // cout<<"off ------------"<<endl;
  // Bits<uint8_t>::off(data,2,4);
  // cout<<"2,4:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  // Bits<uint8_t>::off(data,10,4);
  // cout<<"10,4:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  //
  // // cout<<"on ------------"<<endl;
  // // Bits<uint8_t>::on(data,0,6);
  // // cout<<"0,6:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  //
  // cout<<"cross-bouds get ------------"<<endl;
  // cout<<"6,4:"<<bitset<8>(Bits<uint8_t>::get(data,6,4))<<endl;
  //
  // cout<<"cross-bouds set ------------"<<endl;
  // Bits<uint8_t>::set(data,6,4,0xf);
  // cout<<"6,4=0xf:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  //
  // cout<<"BitPart ------------"<<endl;
  // typedef BitPart<uint8_t,0,16,uint16_t> M;
  // M::set(data,0b1111110000111111);
  // cout<<"set 0b1111110000111111:"<<bitset<8>(data[1])<<" "<<bitset<8>(data[0])<<endl;
  // cout<<"get:"<<bitset<16>(M::get(data))<<endl;
  //
  // cout<<"BitField ------------"<<endl;
  // typedef BitField<uint8_t,data,0,16,uint16_t> Md;
  // cout<<"get:"<<bitset<16>(Md::get())<<endl;

}
