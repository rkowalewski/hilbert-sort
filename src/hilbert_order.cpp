#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>

#include <morton.h>

template <typename T>
static inline std::string toBinaryString(const T& x)
{
  return std::bitset<sizeof(T) * 8>(x).to_string();
}

namespace detail {
template <typename T>
struct dependent_false : std::false_type {
};

template <class DistributionT, class GeneratorT, size_t NDim>
struct RandGenerator {
  static_assert(
      dependent_false<DistributionT>::value,
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

template <class CoordT, std::size_t NDim>
struct MortonEncoder {
  static_assert(
      dependent_false<CoordT>::value,
      "Only up to 3 dimensions are supported");
};

template <class CoordT>
struct MortonEncoder<CoordT, 2> {
  uint64_t operator()(std::array<CoordT, 2> point)
  {
    return libmorton::morton2D_64_encode(
        std::get<1>(point), std::get<0>(point));
  }
};

template <class CoordT>
struct MortonEncoder<CoordT, 3> {
  uint64_t operator()(std::array<CoordT, 3> point)
  {
    return libmorton::morton2D_64_encode(
        std::get<1>(point), std::get<0>(point));
  }
};

}  // namespace detail

template <class coord_t, size_t NDim>
static uint64_t transpose_to_hilbert_integer(std::array<coord_t, NDim> array)
{
  return detail::MortonEncoder<coord_t, NDim>{}(array);
}

template <class coord_t, std::size_t HilbertOrder, std::size_t NDim>
static uint64_t hilbert_distance_by_coords(std::array<coord_t, NDim> point)
{
  static_assert(NDim < 4, "we support up to 3 dimensions only");
  static_assert(
      NDim * HilbertOrder < 64, "we cannot compute this hilbert value");
  static_assert(
      std::is_integral<coord_t>::value, "we support only integral types");

  std::for_each(std::begin(point), std::end(point), [](const coord_t val) {
    assert(!(val > ((1 << HilbertOrder) - 1)));
  });

  constexpr const std::size_t M = 1 << (HilbertOrder - 1);

  // Inverse Undo Excess work
  for (auto Q = M; Q > 1; Q >>= 1) {
    auto P = Q - 1;
    for (std::size_t i = 0; i < NDim; ++i) {
      if (point[i] & Q) {
        point[0] ^= P;
      }
      else {
        auto t = (point[0] ^ point[i]) & P;
        point[0] ^= t;
        point[i] ^= t;
      }
    }
  }

  // Gray encode
  for (std::size_t i = 1; i < NDim; ++i) {
    point[i] ^= point[i - 1];
  }

  coord_t t{0};

  for (auto Q = M; Q > 1; Q >>= 1) {
    if (point[NDim - 1] & Q) {
      t ^= Q - 1;
    }
  }

  for (std::size_t i = 0; i < NDim; ++i) {
    point[i] ^= t;
  }

  // the hilbert integer can be computed through the Morton Value
  return transpose_to_hilbert_integer<coord_t, NDim>(point);
}

template <size_t HilbertOrder, size_t NDim, class Iter>
void rand_points(Iter begin, Iter end)
{
  using point_t = typename std::iterator_traits<Iter>::value_type;
  using coord_t = typename point_t::value_type;

  using distribution_t = std::uniform_int_distribution<coord_t>;
  using generator_t    = std::mt19937;

  static distribution_t distribution(0, (1 << HilbertOrder) - 1);
  static random_device  rd;
  static generator_t    generator(rd());

  std::generate(begin, end, []() -> point_t {
    return detail::RandGenerator<distribution_t, std::mt19937, NDim>{}(
        distribution, generator);
  });
}

int main()
{
  constexpr size_t const NDim = 3;
  // We have 3 dimensions, so we can combine max 3*21 Bits into a single 64
  // Bit integer
  constexpr size_t const HilbertOrder = 2;  // max = 21

  using coord_t = uint32_t;
  using point   = std::array<coord_t, NDim>;

  constexpr const size_t npoints = 20;

  static_assert(
      npoints <= (1 << (NDim * HilbertOrder)),
      "npoints exceeds the available capacity");

  std::vector<point> points(npoints, {0, 0});

  rand_points<HilbertOrder, NDim>(std::begin(points), std::end(points));

  std::sort(
      std::begin(points),
      std::end(points),
      [](const point& a, const point& b) {
        auto const h1 =
            hilbert_distance_by_coords<coord_t, HilbertOrder, NDim>(a);
        auto const h2 =
            hilbert_distance_by_coords<coord_t, HilbertOrder, NDim>(b);
        return h1 < h2;
      });

  for (auto const& p : points) {
    std::copy(
        std::begin(p),
        std::prev(std::end(p)),
        std::ostream_iterator<coord_t>(std::cout, " "));
    std::cout << *std::prev(std::end(p));
    std::cout << "\n";
  }

  return 0;
}
