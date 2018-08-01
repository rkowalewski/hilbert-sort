#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>

#include <hilbert-sort.h>
#include <util.h>

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
    return RandGenerator<distribution_t, generator_t, NDim>{}(
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
  using point_t = std::array<coord_t, NDim>;

  constexpr const size_t npoints = 64;

  static_assert(
      npoints <= (1 << (NDim * HilbertOrder)),
      "npoints exceeds the available capacity");

  std::vector<point_t> points(npoints, {0, 0});

  rand_points<HilbertOrder, NDim>(std::begin(points), std::end(points));

  std::sort(
      std::begin(points),
      std::end(points),
      [](const point_t& a, const point_t& b) {
        auto const h1 =
            hilbert::hilbert_distance_by_coords<point_t, HilbertOrder, NDim>(
                a);
        auto const h2 =
            hilbert::hilbert_distance_by_coords<point_t, HilbertOrder, NDim>(
                b);
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
