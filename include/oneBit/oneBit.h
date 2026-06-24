/**
 * @file oneBit.h
 * @author Rui Azevedo (neu-rah) (ruihfazevedo@gmail.com)
 * @brief HAPI compatible bit manipulation components
 */

#pragma once

#include <hapi/hapi.h>

namespace oneBit {

  namespace detail {
    template<typename T, typename = void>
    struct has_pin_fn : std::false_type {};
    template<typename T>
    struct has_pin_fn<T, std::void_t<decltype(T::pin())>> : std::true_type {};

    template<typename T, typename = void>
    struct has_port_fn : std::false_type {};
    template<typename T>
    struct has_port_fn<T, std::void_t<decltype(T::port())>> : std::true_type {};

    template<typename T, typename = void>
    struct has_ddr_fn : std::false_type {};
    template<typename T>
    struct has_ddr_fn<T, std::void_t<decltype(T::ddr())>> : std::true_type {};

    // dir_out(Unit) / dir_in(Unit) — masked direction change (AVR: DDR |= mask; STM32: MODER)
    template<typename T, typename = void>
    struct has_dir_out_fn : std::false_type {};
    template<typename T>
    struct has_dir_out_fn<T, std::void_t<decltype(T::dir_out(std::declval<typename T::Type>()))>>
      : std::true_type {};

    template<typename T, typename = void>
    struct has_dir_in_fn : std::false_type {};
    template<typename T>
    struct has_dir_in_fn<T, std::void_t<decltype(T::dir_in(std::declval<typename T::Type>()))>>
      : std::true_type {};

    // bsrr_set(Unit) / bsrr_clr(Unit) — atomic STM32 BSRR operations
    template<typename T, typename = void>
    struct has_bsrr_set_fn : std::false_type {};
    template<typename T>
    struct has_bsrr_set_fn<T, std::void_t<decltype(T::bsrr_set(std::declval<typename T::Type>()))>>
      : std::true_type {};

    template<typename T, typename = void>
    struct has_bsrr_clr_fn : std::false_type {};
    template<typename T>
    struct has_bsrr_clr_fn<T, std::void_t<decltype(T::bsrr_clr(std::declval<typename T::Type>()))>>
      : std::true_type {};
  }

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
      if constexpr (sizeof...(rest) == 0) return Unit(1) << b;
      else return (Unit(1) << b) | _fold<Unit, rest...>();
    }
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
    };
  };

  // Mask — bit selection -------------------------------------------------------
  /// @brief select bits via mask descriptor
  /// Mask<MaskVal<0x0f>> or Mask<Pins<1,2,3>>
  /// get() returns selected bits — not LSB aligned
  /// set() writes only selected bits, preserving the rest

  template<typename MaskDesc>
  struct Mask {
    using Desc = MaskDesc;  // exposed for compile-time chain inspection (PortAlloc)

    template<typename O>
    struct Part : O {
      using Base = O;
      using Base::Base;
      using Type = typename O::Type;

      static constexpr Type maskBits() noexcept {
        return MaskDesc::template value<Type>();
      }

      static Type get() noexcept {
        if constexpr (detail::has_pin_fn<Base>::value)
          return Base::pin() & maskBits();
        else
          return Base::get() & maskBits();
      }
      static void set(Type v) noexcept {
        if constexpr (detail::has_port_fn<Base>::value)
          Base::port((Base::port() & ~maskBits()) | (v & maskBits()));
        else
          Base::set((Base::get() & ~maskBits()) | (v & maskBits()));
      }
      static void set(bool b) noexcept { b ? on() : off(); }
      static void on()  noexcept {
        if constexpr (detail::has_bsrr_set_fn<Base>::value)
          Base::bsrr_set(maskBits());              // STM32: atomic BSRR set
        else if constexpr (detail::has_port_fn<Base>::value)
          Base::port(Base::port() |  maskBits());  // AVR: ODR/PORT latch RMW
        else
          Base::set(Base::get() |  maskBits());    // generic
      }
      static void off() noexcept {
        if constexpr (detail::has_bsrr_clr_fn<Base>::value)
          Base::bsrr_clr(maskBits());              // STM32: atomic BSRR reset
        else if constexpr (detail::has_port_fn<Base>::value)
          Base::port(Base::port() & ~maskBits());
        else
          Base::set(Base::get() & ~maskBits());
      }
      static void dir_out() noexcept {
        if constexpr (detail::has_dir_out_fn<Base>::value)
          Base::dir_out(maskBits());               // masked: AVR DDR|=mask, STM32 MODER
        else if constexpr (detail::has_ddr_fn<Base>::value)
          Base::dir(Base::ddr() | maskBits());     // fallback: whole-port DDR write
      }
      static void dir_in() noexcept {
        if constexpr (detail::has_dir_in_fn<Base>::value)
          Base::dir_in(maskBits());
        else if constexpr (detail::has_ddr_fn<Base>::value)
          Base::dir(Base::ddr() & ~maskBits());
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

      static Type get()       noexcept { return ~Base::get(); }
      static void set(Type v) noexcept { Base::set(~v); }
      static void on()        noexcept { Base::off(); }
      static void off()       noexcept { Base::on(); }
      static void set(bool b) noexcept { b ? on() : off(); }
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

  // extractMask — compile-time chain walk, returns mask value from first Mask<> found ----

  template<typename Unit>
  constexpr Unit extractMask(hapi::Chain<>) { return Unit(0); }

  template<typename Unit, typename MaskDesc, typename... Rest>
  constexpr Unit extractMask(hapi::Chain<Mask<MaskDesc>, Rest...>) {
    return MaskDesc::template value<Unit>();
  }

  template<typename Unit, typename O, typename... Rest>
  constexpr Unit extractMask(hapi::Chain<O, Rest...>) {
    return extractMask<Unit>(hapi::Chain<Rest...>{});
  }

  // BitsDef — closes the chain -------------------------------------------------

  /// @brief closed bit-component chain; use as a complete pin/port definition
  template<typename... OO>
  struct BitsDef
    : hapi::Chain<OO...>::template Part<hapi::Nil> {
    using Base =
      typename hapi::Chain<OO...>::template Part<hapi::Nil>;
    using Base::Base;
    // API head = Nil (no separate API class); components = OO...
    // Required by hapi::find<Q> which splits Types::Head as API terminal.
    using Types = hapi::Chain<hapi::Nil, OO...>;
  };

} // namespace oneBit
