#ifndef UTIL_H__INCLUDED
#define UTIL_H__INCLUDED

#include <type_traits>
#include <array>

namespace detail {

template <typename T>
struct dependent_false : std::false_type {
};

}  // namespace detail

template <class DistributionT, class GeneratorT, std::size_t NDim>
struct RandGenerator {
  static_assert(
      detail::dependent_false<DistributionT>::value,
      "Only up to 3 dimensions are supported");
};

template <class DistributionT, class GeneratorT>
struct RandGenerator<DistributionT, GeneratorT, 2> {
  std::array<typename DistributionT::result_type, 2> operator()(
      DistributionT& dist, GeneratorT& gen)
  {
    return {dist(gen), dist(gen)};
  }
};

template <class DistributionT, class GeneratorT>
struct RandGenerator<DistributionT, GeneratorT, 3> {
  std::array<typename DistributionT::result_type, 3> operator()(
      DistributionT& dist, GeneratorT& gen)
  {
    return {dist(gen), dist(gen), dist(gen)};
  }
};

#endif
