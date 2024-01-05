//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include <algorithm>
#include <array>
#include <random>

#include <mph>
#include <unordered_map>

#include "data.hpp"

int main() {
  static constexpr auto iterations = 1'000'000;
  static constexpr auto size = 100;

  std::random_device rd{};
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> distribution(0, size - 1);
  std::vector<int> ids{};
  ids.reserve(iterations);
  for (auto i = 0; i < iterations; ++i) {
    ids.push_back(distribution(gen));
  }

  using namespace ankerl::nanobench;

  // const auto first = [](const auto &symbols) { return symbols[0]; };

  auto next = [&, i = 0](const auto &symbols) mutable {
    return symbols[ids[i++ % iterations]];
  };

  const auto bench_unordered_map = [](const auto name, const auto &symbols,
                                      auto fn) {
    std::unordered_map<std::string_view, int> hash_map{};
    for (auto index = 0; const auto &symbol : symbols) {
      hash_map[symbol] = index++;
    }
    Bench()
        .minEpochIterations(iterations)
        .run(std::string(name) + ".unordered_map",
             [&] { doNotOptimizeAway(hash_map[fn(symbols)]); });
  };

  const auto bench_frozen = [](const auto name, const auto &symbols,
                               const auto &frozen, auto fn) {
    Bench()
        .minEpochIterations(iterations)
        .run(std::string(name) + ".frozen",
             [&] { doNotOptimizeAway(frozen.at(fn(symbols))); });
  };

  const auto bench_bsearch = [](const auto name, const auto &symbols, auto fn) {
    std::vector<std::string_view> strings{};
    for (const auto &symbol : symbols) {
      strings.push_back(symbol);
    }
    std::sort(std::begin(strings), std::end(strings));
    constexpr auto bsearch = [](const auto &symbols, const auto data) -> int {
      if (const bool found = std::binary_search(std::cbegin(symbols),
                                                std::cend(symbols), data)) {
        return std::distance(
            std::cbegin(symbols),
            std::lower_bound(std::cbegin(symbols), std::cend(symbols), data));
      }
      return -1;
    };
    Bench()
        .minEpochIterations(iterations)
        .run(std::string(name) + ".bsearch",
             [&] { doNotOptimizeAway(bsearch(strings, fn(symbols))); });
  };

  const auto bench_gperf = [](const auto name, const auto &symbols, auto fn) {
    const auto size = symbols[0].size();
    /* Command-line: gperf -e ' \\015' -L C++ -7 -C -E -k '*,1,$' */
    const auto hash = [](const char *str, std::size_t len) {
      static constexpr const unsigned short asso_values[] = {
          393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
          393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
          393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
          393, 393, 393, 393, 393, 393, 5,   10,  393, 393, 393, 393, 393,
          393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
          20,  15,  10,  80,  0,   20,  55,  70,  5,   2,   2,   115, 50,
          15,  105, 50,  55,  0,   5,   0,   2,   115, 0,   90,  2,   60,
          393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
          393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
          393, 393, 393, 393, 393, 393, 393, 393, 393, 393, 393};
      unsigned int hval = len;

      switch (hval) {
      default:
        hval += asso_values[static_cast<unsigned char>(str[5])];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[static_cast<unsigned char>(str[4])];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[static_cast<unsigned char>(str[3])];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[static_cast<unsigned char>(str[2])];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[static_cast<unsigned char>(str[1])];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[static_cast<unsigned char>(str[0])];
        break;
      }
      return hval;
    };

    Bench()
        .minEpochIterations(iterations)
        .run(std::string(name) + ".gperf",
             [&] { doNotOptimizeAway(hash(fn(symbols).data(), size)); });
  };

  const auto bench_mph =
      [](auto Symbols, const auto name, const auto &symbols, auto fn) {
      constexpr auto hash = mph::hash{std::move(Symbols)};
    Bench().minEpochIterations(iterations).run(std::string(name) + ".mph", [&] {
      doNotOptimizeAway(hash(fn(symbols).data()));
    });
  };

  bench_unordered_map("all", data::all, next);
  bench_bsearch("all", data::all, next);
#if __has_include(<frozen/unordered_map.h>) and __has_include(<frozen/string.h>)
  bench_frozen("all", data::all, data::frozen_all, next);
#endif
  bench_gperf("all", data::all, next);
  bench_mph([] { return data::all; }, "all", data::all, next);

  bench_unordered_map("random", data::random, next);
  bench_bsearch("random", data::random, next);
#if __has_include(<frozen/unordered_map.h>) and __has_include(<frozen/string.h>)
  bench_frozen("all", data::random, data::frozen_random, next);
#endif
  bench_gperf("random", data::random, next);
  bench_mph([] { return data::random; }, "random", data::random, next);

  // bench_unordered_map("single", data::single, first);
  // bench_bsearch("single", data::single, first);
  // #if __has_include(<frozen/unordered_map.h>) and
  // __has_include(<frozen/string.h>) bench_frozen("single", data::single,
  // data::frozen_single, first);
  // #endif
  // bench_gperf("single", data::single, first);
  // bench_mph([] { return data::single; }, "single", data::single,
  // first);
}
