/**
 * @file T81Category.hpp
 * @brief Provides C++ templates for representing concepts from category theory.
 */
#pragma once

#include <functional>
#include <optional>
#include "t81/core/T81Map.hpp"
#include "t81/core/T81Matrix.hpp"
#include "t81/core/T81Polynomial.hpp"
#include "t81/core/T81Set.hpp"
#include "t81/core/T81Symbol.hpp"
#include "t81/core/T81Tree.hpp"
#include "t81/core/T81Vector.hpp"

namespace t81 {

template <typename Obj, typename Mor>
struct T81Morphism {
  T81Symbol name;
  Obj source;
  Obj target;
  Mor data;

  constexpr T81Morphism() = default;
  constexpr T81Morphism(T81Symbol n, Obj s, Obj t, Mor d)
      : name(n), source(s), target(t), data(std::move(d)) {}

  [[nodiscard]] constexpr bool composable_with(const T81Morphism& other) const noexcept {
    return target == other.source;
  }
};

template <typename Obj, typename Mor>
class T81Category {
  T81Set<Obj> objects_;
  T81Map<T81Symbol, T81Morphism<Obj, Mor>> morphisms_;
  T81Map<Obj, T81Symbol> identity_map_;

public:
  using object_type = Obj;
  using morphism_type = Mor;

  constexpr T81Category() = default;

  constexpr T81Category& add_object(Obj obj, T81Symbol identity_name = {}) {
    objects_ = objects_.insert(obj);
    if (identity_name.is_valid()) {
      identity_map_[obj] = identity_name;
      morphisms_[identity_name] = T81Morphism<Obj, Mor>(identity_name, obj, obj, Mor{});
    }
    return *this;
  }

  constexpr T81Category& add_morphism(T81Symbol name, Obj src, Obj dst, Mor data) {
    morphisms_[name] = T81Morphism<Obj, Mor>(name, src, dst, std::move(data));
    return *this;
  }

  [[nodiscard]] std::optional<T81Morphism<Obj, Mor>> compose(const T81Symbol& f_name,
                                                             const T81Symbol& g_name) const {
    if (!morphisms_.contains(f_name) || !morphisms_.contains(g_name)) return std::nullopt;
    const auto& f = morphisms_.at(f_name);
    const auto& g = morphisms_.at(g_name);
    if (!g.composable_with(f)) return std::nullopt;

    T81Symbol composite_name = T81Symbol::intern(f.name.to_string() + " o " + g.name.to_string());
    return T81Morphism<Obj, Mor>(composite_name, g.source, f.target, f.data * g.data);
  }

  [[nodiscard]] T81Symbol identity_of(Obj obj) const {
    if (identity_map_.contains(obj)) return identity_map_.at(obj);
    return symbols::null;
  }
};

template <typename C, typename D>
class T81Functor {
  const C& source_;
  const D& target_;
  T81Map<typename C::object_type, typename D::object_type> obj_map_;
  T81Map<T81Symbol, T81Symbol> mor_map_;

public:
  constexpr T81Functor(const C& src, const D& dst) : source_(src), target_(dst) {}

  constexpr T81Functor& map_object(typename C::object_type src, typename D::object_type dst) {
    obj_map_[src] = dst;
    return *this;
  }

  constexpr T81Functor& map_morphism(T81Symbol src, T81Symbol dst) {
    mor_map_[src] = dst;
    return *this;
  }
};

}  // namespace t81
