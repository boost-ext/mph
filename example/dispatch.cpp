//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <array>
#include <iostream>
#include <mph>

class dispatch {
  static constexpr std::array keys{
      mph::pair{"APPL    ", 0},
      mph::pair{"GOOGL   ", 1},
      mph::pair{"MSFT    ", 2},
  };

 public:
  constexpr ~dispatch() {
    for (const auto& e : v) {
      std::clog << e << '\n';
    }
  }

  constexpr auto on(const auto data) {
    ++v[*mph::hash<keys>(data)];
  }  // dereferencing not found symbol -> v[max_value<keys>+1u] which will be 3
     // for keys

 private:
  std::array<std::size_t, std::size(keys) + 1> v{};
};

int main() {
  dispatch d{};

  const auto on = [&](const auto key) {
    d.on(std::span<const char, std::size(key) - 1>(
        std::data(key), std::data(key) + std::size(key) - 1));
  };

  on(std::to_array("APPL    "));
  on(std::to_array("GOOGL   "));
  on(std::to_array("MSFT    "));
  on(std::to_array("UNKNOWN "));
}
