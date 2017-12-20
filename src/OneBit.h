/* -*- C++ -*- */

/*
Bit manipulation functions
we can deal with individual bits with an unit (byte, word,...)
or with groups of bits (bitPart)
*/

namespace OneBit {
  typedef uint8_t Byte;

  //log base 2
  constexpr inline unsigned char _log2(size_t n) {return (n?_log2(n>>1)+1:0);};
  constexpr inline unsigned char log2(size_t n) {return n?_log2(n-1):0;};

  //bit operations within a bit field
  //operations get/set, on/off can cross bounds between units
  //bit fields size can even exceed unit size
  template<typename Unit,typename Value=Unit>
  struct Bits {
    //bits per unit
    constexpr static inline Byte bpu() {return sizeof(Unit)<<3;}
    //address bits per unit
    constexpr static inline Byte abpu() {return log2(bpu());}
    //max representable value
    constexpr static inline Value ones(Byte sz) {return (1<<sz)-1;}
    //mask
    constexpr static inline Value mask(Byte at=0,Byte sz=1) {return ones(sz)<<at;}
    //unit index
    constexpr static inline Byte idx(Byte at) {return at>>abpu();}
    //bit address inside unit
    constexpr static inline Byte pos(Byte at) {return at&(ones(abpu()));}
    //get bit-field value
    constexpr static inline Value get(Unit data[],Byte at,Byte sz) {
      return (pos(at)+sz)>bpu()?//crossing bounds
        get(data,at,bpu()-pos(at))|get(data,at+(bpu()-pos(at)),sz-(bpu()-pos(at)))<<(bpu()-pos(at)):
        (data[idx(at)]>>pos(at))&(ones(sz));
    }

    //set bit-field value
    constexpr static inline void set(Unit data[],Byte at,Byte sz,Value value) {
      Unit m=mask(pos(at),sz);
      if ((pos(at)+sz)>bpu()) {//crossing bounds
        set(data,at,bpu()-pos(at),value);
        set(data,at+(bpu()-pos(at)),sz-(bpu()-pos(at)),value>>(bpu()-pos(at)));
      } else data[idx(at)]=(data[idx(at)]&~m)|((value<<pos(at))&m);
    }
    //turn bit-field on (all ones)
    constexpr static inline void on(Unit data[],Byte at,Byte sz) {
      set(data,at,sz,ones(sz));
    }
    //turn bit-field off (all zeros)
    constexpr static inline void off(Unit data[],Byte at,Byte sz) {
      set(data,at,sz,0);
    }

  };

  // a sequence of bits on arbitrary data
  template<typename Unit,Byte at, Byte sz=1,typename Value=Unit>
  struct BitPart {
    constexpr static inline Value get(Unit data[]) {
      return Bits<Unit,Value>::get(data,at,sz);
    }
    constexpr static inline void on(Unit data[]) {
      Bits<Unit,Value>::on(data,at);
    }
    constexpr static inline void off(Unit data[]) {
      Bits<Unit,Value>::off(data,at);
    }
    constexpr static inline void set(Unit data[],Value value) {
      Bits<Unit,Value>::set(data,at,sz,value);
    }
  };

  // just an array of data with functions to work with bits
  template<typename Unit,Unit data[],typename Value=Unit>
  struct BitData {

    constexpr static inline Value get(Byte at,Byte sz) {
      return Bits<Unit,Value>::get(data,at,sz);
    }

    template<Byte at, Byte sz=1>
    constexpr static inline Value get() {
      return BitPart<Unit,at,sz,Value>::get(data);
    }

    constexpr static inline void set(Byte at,Byte sz,Value value) {
      return Bits<Unit,Value>::set(data,at,sz,value);
    }

    template<Byte at, Byte sz=1>
    constexpr static inline void set(Value value) {
      return BitPart<Unit,at,sz,Value>::set(data,value);
    }
  };

  template<typename Unit, Unit data[],Byte at, Byte sz=1,typename Value=Unit>
  struct BitField:public BitPart<Unit,at,sz> {
    constexpr static inline Value get() {return BitPart<Unit,at,sz,Value>::get(data);}
    constexpr static inline void on() {BitPart<Unit,at,sz,Value>::on(data);}
    constexpr static inline void off() {BitPart<Unit,at,sz,Value>::off(data);}
    constexpr static inline void set(Value value) {BitPart<Unit,at,sz,Value>::set(data,value);}
  };

};
