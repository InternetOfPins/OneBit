/* -*- C++ -*- */

/*
Bit manipulation functions
we can deal with individual bits with an unit (byte, word,...)
or with groups of bits (bitPart)
*/

namespace OneBit {
  typedef uint8_t Byte;

  //log base 2
  template<Byte n>
  constexpr inline unsigned char _log2() {return n?_log2<(n>>1)>()+1:0;}
  template<Byte n>
  constexpr inline unsigned char log2() {return n?_log2<(n>>1)>():0;}

  //bit operations within a bit field
  //operations get/set, on/off can cross bounds between units
  //bit fields size can even exceed unit size
  #define CROSS_BOUNDS_FIELDS
  #define DATA_TYPE uint8_t //Unit*

  template<typename Unit,DATA_TYPE data,uint8_t at, int8_t sz=1,typename Value=Unit>
  struct Bits {
    //some usefull functions for bits map math
    //bits per unit
    constexpr static inline uint8_t bpu() {return sizeof(Unit)<<3;}
    //address bits per unit
    constexpr static inline uint8_t abpu() {return log2<bpu()>();}
    //max representable value
    constexpr static inline Value ones(int8_t n=sz) {return (1<<n)-1;}
    //mask
    constexpr static inline Value mask() {return ones(sz)<<pos();}
    //unit index
    constexpr static inline uint8_t idx() {return at>>abpu();}
    //bit address inside unit
    constexpr static inline uint8_t pos() {return at&(ones(abpu()));}

    // bit field operations get/set, on/off ---------------------------
    template<typename T,T v>
    class display;
    enum {
      psz=pos()+sz,
      delta=bpu()-pos(),
      crossing=(pos()+sz)>bpu()
    };
    static inline Value get() {
      #pragma GCC diagnostic ignored "-Woverflow"
        return ((pos()+sz)>bpu()) ?
          (Bits<Unit,data,at,delta,Value>::get()|((Bits<Unit,data,at+delta,sz-delta,Value>::get())<<delta)) :
          (reinterpret_cast<Unit*>(data)[idx()]>>pos())&ones();
      #pragma GCC diagnostic pop
    }
    static inline void set(Value value) {
      if (crossing) {
        Bits<Unit,data,at,delta,Value>::set(value);
        #pragma GCC diagnostic ignored "-Woverflow"
        Bits<Unit,data,at+delta,sz-delta,Value>::set(value>>delta);
        #pragma GCC diagnostic pop
      } else
        reinterpret_cast<Unit*>(data)[idx()]=(reinterpret_cast<Unit*>(data)[idx()]&~(mask()))|((value<<pos())&(mask()));
    }
    //turn bit-field on (all ones)
    static inline void on() {set(ones(sz));}
    //turn bit-field off (all zeros)
    static inline void off() {set(0);}
  };

};
