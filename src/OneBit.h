/* -*- C++ -*- */
#include <stdint.h>

/*
Bit manipulation functions
*/
#ifndef ONE_BIT_H
#define ONE_BIT_H

  // typedef unsigned char uint8_t;
  constexpr uint8_t log2(uint8_t n) {return n>1?1+log2(n>>1):1;}

  namespace OneBit {

    //@Unit - type to be transacted internally
    //@Type - stored type
    //@data - the storage
    //@at - starting point in bits
    //@sz - size in bits
    //@Value - type to be transacted externally

    //bit operations within a bit field
    //operations get/set, on/off can cross bounds between units
    //bit fields size can even exceed unit size
    #define CROSS_BOUNDS_FIELDS

    template<typename Unit,typename Type,Type data,uint8_t at, uint8_t sz=1,typename Value=Unit>
    struct Bits {
      //some usefull functions for bits map math
      //bits per unit
      constexpr static inline uint8_t bpu() {return sizeof(Unit)<<3;}
      //address bits per unit
      constexpr static inline uint8_t abpu() {return log2(bpu());}
      //max representable value
      constexpr static inline Value ones(uint8_t n=sz) {return (1<<n)-1;}
      //mask
      constexpr static inline Value mask() {return ones(sz)<<pos();}
      //unit index
      constexpr static inline uint8_t idx() {return at>>abpu();}
      //bit address inside unit
      constexpr static inline uint8_t pos() {return at&(ones(abpu()));}
      //domain size-1
      constexpr static inline Value len() {return (1<<sz)-1;}

      // bit field operations get/set, on/off ---------------------------
      enum {
        psz=pos()+sz,
        delta=bpu()-pos(),
        crossing=(pos()+sz)>bpu()
      };

      static inline Value get() {
        return ((pos()+sz)>bpu()) ?
          (Bits<Unit,Type,data,at,delta,Value>::get()|((Bits<Unit,Type,data,(uint8_t)(at+delta),(uint8_t)(sz-delta),Value>::get())<<delta)) :
          (reinterpret_cast<Unit*>(data)[idx()]>>pos())&ones();
      }

      static inline void set(Value value) {
        if (crossing) {
          Bits<Unit,Type,data,at,delta,Value>::set(value);
          Bits<Unit,Type,data,(uint8_t)(at+delta),(uint8_t)(sz-delta),Value>::set(value>>delta);
        } else
          reinterpret_cast<Unit*>(data)[idx()]=(reinterpret_cast<Unit*>(data)[idx()]&~(mask()))|((value<<pos())&(mask()));
      }
      //turn bit-field on (all ones)
      static inline void on() {set(ones(sz));}
      //turn bit-field off (all zeros)
      static inline void off() {set(0);}
      static inline void tog() {set(~get());}

    };

  };
#endif
