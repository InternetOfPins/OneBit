/**
 * @file oneBit.h
 * @author Rui Azevedo (neu-rah) (ruihfazevedo@gmail.com)
 * @brief HAPI compatible bit manipulation components
 */

#pragma once

#include <hapi/hapi.h>

namespace oneBit {

  // Mask descriptors -----------------------------------------------------------

  /// @brief mask from a raw value
  template<unsigned long _mask>
  struct MaskVal {
    template<typename Unit>
    static constexpr Unit value() noexcept {
      return static_cast<Unit>(_mask);
    }
  };

  /// @brief mask from bit positions
  template<int... bits>
  struct Pins {
    template<typename Unit>
    static constexpr Unit value() noexcept {
      return _fold<Unit, bits...>();
    }
  private:
    template<typename Unit, int b, int... rest>
    static constexpr Unit _fold() noexcept {
      return (Unit(1) << b) | _fold<Unit, rest...>();
    }
    template<typename Unit, int b>
    static constexpr Unit _fold() noexcept { return (Unit(1) << b); }
  };

  // Bits — raw storage component -----------------------------------------------
  /// @brief owns a reference to a hardware register or variable
  /// provides raw get/set over the full unit
  /// mask/field/inversion components narrow or transform above it

  template<typename Unit, Unit& data>
  struct Bits {
    template<typename O>
    struct Part : O {
      using Base = O;
      using Base::Base;
      using Type = Unit;

      static Unit get() noexcept { return data; }
      static void set(Unit v) noexcept { data = v; }

      // internal chain communication — not public API
      static void rawClear(Unit bits) noexcept { data &= ~bits; }
      static void rawSet(Unit bits) noexcept { data |= bits; }
      static void rawWrite(Unit bits, Unit mask) noexcept {
        data = (data & ~mask) | (bits & mask);
      }
    };
  };

  // Mask — bit selection -------------------------------------------------------
  /// @brief select bits via mask descriptor
  /// Mask<MaskVal<0x0f>> or Mask<Pins<1,2,3>>
  /// get() returns selected bits — not LSB aligned
  /// set() writes only selected bits, preserving the rest

  template<typename MaskDesc>
  struct Mask {
    template<typename O>
    struct Part : O {
      using Base = O;
      using Base::Base;
      using Type = typename O::Type;

      static constexpr Type maskBits() noexcept {
        return MaskDesc::template value<Type>();
      }

      static Type get() noexcept {
        return Base::get() & maskBits();
      }
      static void set(Type v) noexcept {
        Base::rawWrite(v & maskBits(), maskBits());
      }
    };
  };

  // BitField — contiguous field, LSB aligned -----------------------------------
  /// @brief extract a contiguous field at `shift`, `size` bits wide
  /// get() returns LSB aligned value (0..2^size-1)
  /// set() takes LSB aligned value, places at correct position

  template<int shift, int size>
  struct BitField {
    template<typename O>
    struct Part : O {
      using Base = O;
      using Base::Base;
      using Type = typename O::Type;

      static constexpr Type fieldMask() noexcept {
        return ((Type(1) << size) - 1) << shift;
      }

      static Type get() noexcept {
        return (Base::get() & fieldMask()) >> shift;
      }
      static void set(Type v) noexcept {
        Base::rawWrite((v << shift) & fieldMask(), fieldMask());
      }
    };
  };

  // Inverted — logical inversion -----------------------------------------------
  /// @brief invert get/set
  /// Inverted<>                 — invert all bits
  /// Inverted<Pins<1,3,5>>      — invert only selected bits
  /// position in chain determines scope relative to Mask/BitField

  template<typename MaskDesc = void>
  struct Inverted {
    template<typename O>
    struct Part : O {
      using Base = O;
      using Base::Base;
      using Type = typename O::Type;

      static constexpr Type invMask() noexcept {
        return MaskDesc::template value<Type>();
      }

      static Type get() noexcept {
        return (Base::get() & ~invMask()) | (~Base::get() & invMask());
      }
      static void set(Type v) noexcept {
        Base::set((v & ~invMask()) | (~v & invMask()));
      }
    };
  };

  // Inverted<> specialization — invert all
  template<>
  struct Inverted<void> {
    template<typename O>
    struct Part : O {
      using Base = O;
      using Base::Base;
      using Type = typename O::Type;

      static Type get() noexcept { return ~Base::get(); }
      static void set(Type v) noexcept { Base::set(~v); }
    };
  };

  // MaskedInvert — atomic select + invert -------------------------------------
  /// @brief select and invert bits atomically — no placement dependency
  /// MaskedInvert<Pins<1,3,5>> or MaskedInvert<MaskVal<0x0f>>
  /// selected bits are inverted, unselected bits pass through unchanged

  template<typename MaskDesc>
  struct MaskedInvert {
    template<typename O>
    struct Part : O {
      using Base = O;
      using Base::Base;
      using Type = typename O::Type;

      static constexpr Type invMask() noexcept {
        return MaskDesc::template value<Type>();
      }

      static Type get() noexcept {
        Type raw = Base::get();
        return (raw & ~invMask()) | (~raw & invMask());
      }
      static void set(Type v) noexcept {
        Base::set((v & ~invMask()) | (~v & invMask()));
      }
    };
  };

  // BitsDef — closes the chain -------------------------------------------------

  template<typename... OO>
  struct BitsDef
    : hapi::Chain<OO...>::template Part<hapi::Nil> {
    using Base =
      typename hapi::Chain<OO...>::template Part<hapi::Nil>;
    using Base::Base;
  };

} // namespace oneBit
