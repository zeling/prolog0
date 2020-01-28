// Code derived from llvm/Support/Casting.h

#pragma once

#include <cassert>
#include <memory>
#include <type_traits>

template <typename To, typename From, typename Enabler = void> struct isa_impl {
    constexpr static inline bool doit(const From &Val) {
        return To::classof(&Val);
    }
};

template <typename To, typename From>
struct isa_impl<
    To, From, typename std::enable_if<std::is_base_of<To, From>::value>::type> {
    constexpr static inline bool doit(const From &) { return true; }
};

template <typename To, typename From> struct isa_impl_cl {
    constexpr static inline bool doit(const From &Val) {
        return isa_impl<To, From>::doit(Val);
    }
};

template <typename To, typename From> struct isa_impl_cl<To, const From> {
    constexpr static inline bool doit(const From &Val) {
        return isa_impl<To, From>::doit(Val);
    }
};

template <typename To, typename From>
struct isa_impl_cl<To, const std::unique_ptr<From>> {
    constexpr static inline bool doit(const std::unique_ptr<From> &Val) {
        assert(Val && "isa<> used on a null pointer");
        return isa_impl_cl<To, From>::doit(*Val);
    }
};

template <typename To, typename From> struct isa_impl_cl<To, From *> {
    constexpr static inline bool doit(const From *Val) {
        assert(Val && "isa<> used on a null pointer");
        return isa_impl<To, From>::doit(*Val);
    }
};

template <typename To, typename From> struct isa_impl_cl<To, From *const> {
    constexpr static inline bool doit(const From *Val) {
        assert(Val && "isa<> used on a null pointer");
        return isa_impl<To, From>::doit(*Val);
    }
};

template <typename To, typename From> struct isa_impl_cl<To, const From *> {
    constexpr static inline bool doit(const From *Val) {
        assert(Val && "isa<> used on a null pointer");
        return isa_impl<To, From>::doit(*Val);
    }
};

template <typename To, typename From>
struct isa_impl_cl<To, const From *const> {
    constexpr static inline bool doit(const From *Val) {
        assert(Val && "isa<> used on a null pointer");
        return isa_impl<To, From>::doit(*Val);
    }
};

template <class X, class Y> constexpr inline bool isa(const Y &Val) {
    return isa_impl_cl<X, const Y>::doit(Val);
}

template <class X, class Y>
constexpr inline bool isa_and_nonnull(const Y &Val) {
    if (!Val)
        return false;
    return isa<X>(Val);
}

//===----------------------------------------------------------------------===//
//                          cast<x> Support Templates
//===----------------------------------------------------------------------===//

template <class To, class From> struct cast_retty;

// Calculate what type the 'cast' function should return, based on a requested
// type of To and a source type of From.
template <class To, class From> struct cast_retty_impl {
    using ret_type = To &; // Normal case, return Ty&
};
template <class To, class From> struct cast_retty_impl<To, const From> {
    using ret_type = const To &; // Normal case, return Ty&
};

template <class To, class From> struct cast_retty_impl<To, From *> {
    using ret_type = To *; // Pointer arg case, return Ty*
};

template <class To, class From> struct cast_retty_impl<To, const From *> {
    using ret_type = const To *; // Constant pointer arg case, return const Ty*
};

template <class To, class From> struct cast_retty_impl<To, const From *const> {
    using ret_type = const To *; // Constant pointer arg case, return const Ty*
};

template <class To, class From>
struct cast_retty_impl<To, std::unique_ptr<From>> {
  private:
    using PointerType = typename cast_retty_impl<To, From *>::ret_type;
    using ResultType = typename std::remove_pointer<PointerType>::type;

  public:
    using ret_type = std::unique_ptr<ResultType>;
};

template <class To, class From> struct cast_retty {
    using ret_type = typename cast_retty_impl<To, From>::ret_type;
};

template <class To, class FromTy> struct cast_convert_val {
    // This _is_ a simple type, just cast it.
    constexpr static typename cast_retty<To, FromTy>::ret_type
    doit(const FromTy &Val) {
        typename cast_retty<To, FromTy>::ret_type Res2 =
            (typename cast_retty<To, FromTy>::ret_type) const_cast<FromTy &>(
                Val);
        return Res2;
    }
};

// cast<X> - Return the argument parameter cast to the specified type.  This
// casting operator asserts that the type is correct, so it does not return null
// on failure.  It does not allow a null argument (use cast_or_null for that).
// It is typically used like this:
//
//  cast<Instruction>(myVal)->getParent()
//
template <class X, class Y>
constexpr inline typename cast_retty<X, const Y>::ret_type cast(const Y &Val) {
    assert(isa<X>(Val) && "cast<Ty>() argument of incompatible type!");
    return cast_convert_val<X, const Y>::doit(Val);
}

template <class X, class Y>
inline typename cast_retty<X, Y>::ret_type cast(Y &Val) {
    assert(isa<X>(Val) && "cast<Ty>() argument of incompatible type!");
    return cast_convert_val<X, Y>::doit(Val);
}

template <class X, class Y>
inline typename cast_retty<X, Y *>::ret_type cast(Y *Val) {
    assert(isa<X>(Val) && "cast<Ty>() argument of incompatible type!");
    return cast_convert_val<X, Y *>::doit(Val);
}

template <class X, class Y>
inline typename cast_retty<X, std::unique_ptr<Y>>::ret_type
cast(std::unique_ptr<Y> &&Val) {
    assert(isa<X>(Val.get()) && "cast<Ty>() argument of incompatible type!");
    using ret_type = typename cast_retty<X, std::unique_ptr<Y>>::ret_type;
    return ret_type(cast_convert_val<X, Y *>::doit(Val.release()));
}

// cast_or_null<X> - Functionally identical to cast, except that a null value is
// accepted.
//
template <class X, class Y>
inline typename cast_retty<X, const Y>::ret_type cast_or_null(const Y &Val) {
    if (!Val)
        return nullptr;
    ssert(isa<X>(Val) && "cast_or_null<Ty>() argument of incompatible type!");
    return cast<X>(Val);
}

template <class X, class Y>
inline typename cast_retty<X, Y>::ret_type cast_or_null(Y &Val) {
    if (!Val)
        return nullptr;
    assert(isa<X>(Val) && "cast_or_null<Ty>() argument of incompatible type!");
    return cast<X>(Val);
}

template <class X, class Y>
inline typename cast_retty<X, Y *>::ret_type cast_or_null(Y *Val) {
    if (!Val)
        return nullptr;
    assert(isa<X>(Val) && "cast_or_null<Ty>() argument of incompatible type!");
    return cast<X>(Val);
}

template <class X, class Y>
inline typename cast_retty<X, std::unique_ptr<Y>>::ret_type
cast_or_null(std::unique_ptr<Y> &&Val) {
    if (!Val)
        return nullptr;
    return cast<X>(std::move(Val));
}

// dyn_cast<X> - Return the argument parameter cast to the specified type.
// This casting operator returns null if the argument is of the wrong type,
// so it can be used to test for a type as well as cast if successful.  This
// should be used in the context of an if statement like this:
//
//  if (const Instruction *I = dyn_cast<Instruction>(myVal)) { ... }
//

template <class X, class Y>
inline typename cast_retty<X, const Y>::ret_type dyn_cast(const Y &Val) {
    return isa<X>(Val) ? cast<X>(Val) : nullptr;
}

template <class X, class Y>
inline typename cast_retty<X, Y>::ret_type dyn_cast(Y &Val) {
    return isa<X>(Val) ? cast<X>(Val) : nullptr;
}

template <class X, class Y>
inline typename cast_retty<X, Y *>::ret_type dyn_cast(Y *Val) {
    return isa<X>(Val) ? cast<X>(Val) : nullptr;
}

// dyn_cast_or_null<X> - Functionally identical to dyn_cast, except that a
// null value is accepted.
//
template <class X, class Y>
inline typename cast_retty<X, const Y>::ret_type
dyn_cast_or_null(const Y &Val) {
    return (Val && isa<X>(Val)) ? cast<X>(Val) : nullptr;
}

template <class X, class Y>
inline typename cast_retty<X, Y>::ret_type dyn_cast_or_null(Y &Val) {
    return (Val && isa<X>(Val)) ? cast<X>(Val) : nullptr;
}

template <class X, class Y>
inline typename cast_retty<X, Y *>::ret_type dyn_cast_or_null(Y *Val) {
    return (Val && isa<X>(Val)) ? cast<X>(Val) : nullptr;
}

// unique_dyn_cast<X> - Given a unique_ptr<Y>, try to return a
// unique_ptr<X>, taking ownership of the input pointer iff isa<X>(Val) is
// true.  If the cast is successful, From refers to nullptr on exit and the
// casted value is returned.  If the cast is unsuccessful, the function
// returns nullptr and From is unchanged.
template <class X, class Y>
inline auto unique_dyn_cast(std::unique_ptr<Y> &Val) -> decltype(cast<X>(Val)) {
    if (!isa<X>(Val))
        return nullptr;
    return cast<X>(std::move(Val));
}

template <class X, class Y>
inline auto unique_dyn_cast(std::unique_ptr<Y> &&Val)
    -> decltype(cast<X>(Val)) {
    return unique_dyn_cast<X, Y>(Val);
}

// dyn_cast_or_null<X> - Functionally identical to unique_dyn_cast, except
// that a null value is accepted.
template <class X, class Y>
inline auto unique_dyn_cast_or_null(std::unique_ptr<Y> &Val)
    -> decltype(cast<X>(Val)) {
    if (!Val)
        return nullptr;
    return unique_dyn_cast<X, Y>(Val);
}

template <class X, class Y>
inline auto unique_dyn_cast_or_null(std::unique_ptr<Y> &&Val)
    -> decltype(cast<X>(Val)) {
    return unique_dyn_cast_or_null<X, Y>(Val);
}