//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <mph>

#include "ut.hpp"

int main() {
  using namespace boost::ut;
  using std::literals::operator""sv;

  constexpr auto verify = [](const auto &keys, const auto &hash) {
    for (auto expected = 1; const auto &[symbol, _] : keys) {
      expect(_i(expected++) == hash(symbol));
    }
  };

  "[hash] enum"_test = [] {
    enum class color {
      red,
      green,
      blue,
    };

    constexpr auto colors = std::array{
        std::pair{mph::fixed_string{"red"}, color::red},
        std::pair{mph::fixed_string{"green"}, color::green},
        std::pair{mph::fixed_string{"blue"}, color::blue},
    };

    constexpr auto hash = mph::hash<color{-1}, colors>;

    expect(color::red == hash("red"));
    expect(color::green == hash("green"));
    expect(color::blue == hash("blue"));
    expect(color{-1} == hash(""));
    expect(color{-1} == hash("D"));
    expect(color{-1} == hash("a"));
    expect(color{-1} == hash("b"));
  };

  "[hash] integral"_test = [verify] {
    constexpr auto keys = std::array{
        std::pair{23423ul, 1},
        std::pair{432432ul, 2},
        std::pair{31232ul, 3},
    };

    constexpr auto hash = mph::hash<0, keys>;

    verify(keys, hash);

    expect(0 == hash(0ul));
    expect(0 == hash(1234ul));
    expect(0 == hash(42ul));
  };

  "[hash] policies"_test = [verify] {
    constexpr std::array keys{
        std::pair{mph::fixed_string{"A"}, 1},
        std::pair{mph::fixed_string{"B"}, 2},
        std::pair{mph::fixed_string{"C"}, 3},
    };

    constexpr auto hash = mph::hash<0, keys, mph::policies>;

    verify(keys, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash("D"sv));
    expect(0_i == hash("a"sv));
    expect(0_i == hash("b"sv));
  };

  "[hash] integral - custom policies - pext"_test = [verify] {
    constexpr auto keys = std::array{
        std::pair{23423ul, 1},
        std::pair{432432ul, 2},
        std::pair{31232ul, 3},
    };

    constexpr auto hash = mph::hash < 0, keys,
                   []<const auto... ts>(auto &&...args) {
      return mph::pext<5>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(keys, hash);

    expect(0 == hash(0ul));
    expect(0 == hash(1234ul));
    expect(0 == hash(42ul));
  };

  "[hash] integral - custom policies - swar<std::uint32_t>"_test = [verify] {
    constexpr auto keys = std::array{
        std::pair{3, 1},
        std::pair{4, 2},
        std::pair{7, 3},
    };

    constexpr auto hash = mph::hash < 0, keys,
                   []<const auto... ts>(auto &&...args) {
      return mph::swar<std::uint32_t>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(keys, hash);

    expect(0 == hash(0));
    expect(0 == hash(2));
    expect(0 == hash(5));
    expect(0 == hash(6));
  };

  "[hash] integral - custom policies - swar32"_test = [verify] {
    constexpr auto keys = std::array{
        std::pair{3, 1},
        std::pair{4, 2},
        std::pair{7, 3},
    };

    constexpr auto hash = mph::hash < 0, keys,
                   []<const auto... ts>(auto &&...args) {
      return mph::swar<std::uint32_t>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(keys, hash);

    expect(0 == hash(0));
    expect(0 == hash(2));
    expect(0 == hash(5));
    expect(0 == hash(6));
  };

  "[hash] integral - custom policies - swar64"_test = [verify] {
    constexpr auto keys = std::array{
        std::pair{std::uint64_t(11111), 1},
        std::pair{std::uint64_t(22222), 2},
        std::pair{std::uint64_t(33333), 3},
    };

    constexpr auto hash = mph::hash < 0, keys,
                   []<const auto... ts>(auto &&...args) {
      return mph::swar<std::uint64_t>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(keys, hash);

    expect(0 == hash(std::uint64_t(0)));
    expect(0 == hash(std::uint64_t(2)));
    expect(0 == hash(std::uint64_t(5)));
    expect(0 == hash(std::uint64_t(6)));
  };

  "[hash] strings - custom policies - pext"_test = [verify] {
    constexpr std::array keys{
        std::pair{mph::fixed_string{"A"}, 1},
        std::pair{mph::fixed_string{"B"}, 2},
        std::pair{mph::fixed_string{"C"}, 3},
    };

    constexpr auto hash = mph::hash < 0, keys,
                   []<const auto... ts>(auto &&...args) {
      return mph::pext<5>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(keys, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash("D"sv));
    expect(0_i == hash("a"sv));
    expect(0_i == hash("b"sv));
  };

  "[hash] strings - custom policies - pext_split"_test = [verify] {
    constexpr std::array keys{
        std::pair{mph::fixed_string{"A"}, 1},
        std::pair{mph::fixed_string{"B"}, 2},
        std::pair{mph::fixed_string{"C"}, 3},
    };

    constexpr auto hash = mph::hash < 0, keys,
                   []<const auto... ts>(auto &&...args) {
      return mph::pext_split<5, 0u>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(keys, hash);

    expect(0_i == hash("D"sv));
    expect(0_i == hash("a"sv));
    expect(0_i == hash("b"sv));
  };

  "[hash] strings - custom policies - pext_split<N>"_test = [verify] {
    constexpr std::array keys{
        std::pair{mph::fixed_string{"AAPL    "}, 1},
        std::pair{mph::fixed_string{"AMZN    "}, 2},
        std::pair{mph::fixed_string{"GOOGL   "}, 3},
        std::pair{mph::fixed_string{"MSFT    "}, 4},
        std::pair{mph::fixed_string{"NVDA    "}, 5},
    };

    constexpr auto hash = mph::hash < 0, keys,
                   []<const auto... ts>(auto &&...args) {
      return mph::pext_split<7, 0u>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(keys, hash);
  };

  "[hash] strings - custom policies - pext_split<len(N)-1>"_test = [verify] {
    constexpr std::array keys{
        std::pair{mph::fixed_string{"    AAPL"}, 1},
        std::pair{mph::fixed_string{"    AMZN"}, 2},
        std::pair{mph::fixed_string{"   GOOGL"}, 3},
        std::pair{mph::fixed_string{"    MSFT"}, 4},
        std::pair{mph::fixed_string{"    NVDA"}, 5},
    };

    constexpr auto hash = mph::hash < 0, keys,
                   []<const auto... ts>(auto &&...args) {
      return mph::pext_split<7, 7u>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(keys, hash);
  };

  "[hash] strings - custom policies - swar32"_test = [verify] {
    constexpr std::array keys{
        std::pair{mph::fixed_string{"A"}, 1},
        std::pair{mph::fixed_string{"B"}, 2},
        std::pair{mph::fixed_string{"C"}, 3},
    };

    constexpr auto hash = mph::hash < 0, keys,
                   []<const auto... ts>(auto &&...args) {
      return mph::swar<std::uint32_t>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(keys, hash);

    expect(0_i == hash("D"sv));
    expect(0_i == hash("a"sv));
    expect(0_i == hash("b"sv));
  };

  "[hash] strings - custom policies - swar64"_test = [verify] {
    constexpr std::array keys{std::pair{mph::fixed_string{"foobar"}, 1},
                              std::pair{mph::fixed_string{"bar"}, 2},
                              std::pair{mph::fixed_string{"foo"}, 3}};

    constexpr auto hash = mph::hash < 0, keys,
                   []<const auto... ts>(auto &&...args) {
      return mph::swar<std::uint64_t>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(keys, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash("xxx"sv));
    expect(0_i == hash("baz"sv));
  };

  "[hash] strings - custom policies - swar64 / std::span"_test = [verify] {
    constexpr std::array keys{
        std::pair{mph::fixed_string{"A       "}, 1},
        std::pair{mph::fixed_string{"B       "}, 2},
        std::pair{mph::fixed_string{"C       "}, 3},
    };

    constexpr auto size = std::size(keys[0].first);
    constexpr auto hash = mph::hash < 0, keys,
                   []<const auto... ts>(auto &&...args) {
      return mph::swar<std::uint64_t>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(keys, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash("D "sv));
    expect(0_i == hash(" D"sv));
    expect(0_i == hash(" D "sv));
    expect(0_i == hash("E"sv));
    expect(0_i == hash("F"sv));
    expect(0_i == hash(std::span<const char>("        ", size)));
    expect(0_i == hash(std::span<const char>("D       ", size)));
    expect(0_i == hash(std::span<const char>("E       ", size)));
    expect(0_i == hash(std::span<const char>("F       ", size)));
  };

  "[hash] std::span data"_test = [verify] {
    constexpr std::array keys{
        std::pair{mph::fixed_string{"A       "}, 1},
        std::pair{mph::fixed_string{"B       "}, 2},
        std::pair{mph::fixed_string{"C       "}, 3},
    };

    constexpr auto size = std::size(keys[0].first);

    auto hash = mph::hash<0, keys>;

    verify(keys, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash("D "sv));
    expect(0_i == hash(" D"sv));
    expect(0_i == hash(" D "sv));
    expect(0_i == hash("E"sv));
    expect(0_i == hash("F"sv));
    expect(0_i == hash(std::span<const char>("        ", size)));
    expect(0_i == hash(std::span<const char>("D       ", size)));
    expect(0_i == hash(std::span<const char>("E       ", size)));
    expect(0_i == hash(std::span<const char>("F       ", size)));
  };

  "[hash] std::span variable length"_test = [verify] {
    constexpr std::array keys{
        std::pair{mph::fixed_string("enter  "), 1},
        std::pair{mph::fixed_string("delete "), 2},
        std::pair{mph::fixed_string("esc    "), 3},
    };

    const auto hash = mph::hash<0, keys>;

    verify(keys, hash);

    expect(0_i == hash(std::span("")));
    expect(0_i == hash(std::span("  ")));
    expect(0_i == hash(std::span("    ")));
    expect(0_i == hash(std::span("stop")));
    expect(0_i == hash(std::span("start")));
    expect(0_i == hash(std::span("foobar")));
    expect(0_i == hash(std::span("1234567")));
  };

  "[hash] std::array"_test = [verify] {
    constexpr std::array keys{
        std::pair{std::array{'A', 'a'}, 1},
        std::pair{std::array{'B', 'b'}, 2},
        std::pair{std::array{'C', 'c'}, 3},
    };

    const auto hash = mph::hash<0, keys>;

    verify(keys, hash);

    expect(0_i == hash(std::array{'A'}));
    expect(0_i == hash(std::array{'X'}));
    expect(0_i == hash(std::array{'A', 'A'}));
    expect(0_i == hash(std::array{'B', 'c'}));
    expect(0_i == hash(std::array{'C', 'b'}));
    expect(0_i == hash(std::array{'C', 'b', 'a'}));
  };

  "[hash] std::string_view"_test = [verify] {
    constexpr std::array keys{
        std::pair{mph::fixed_string{"AA "}, 1},
        std::pair{mph::fixed_string{"BB "}, 2},
        std::pair{mph::fixed_string{"CC "}, 3},
    };

    auto hash = mph::hash<0, keys>;

    verify(keys, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash("   "sv));
    expect(0_i == hash("aa "sv));
    expect(0_i == hash("aaa"sv));
    expect(0_i == hash("bb"sv));
    expect(0_i == hash("bb "sv));
    expect(0_i == hash(" cc"sv));
  };

  "[hash] fail case - variable length"_test = [verify] {
    constexpr std::array keys{
        std::pair{mph::fixed_string{" AA "}, 1},
        std::pair{mph::fixed_string{" AB "}, 2},
        std::pair{mph::fixed_string{" AC "}, 3},
    };

    auto hash = mph::hash<0, keys>;

    verify(keys, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash(" aa "sv));
    expect(0_i == hash("aaaa"sv));
    expect(0_i == hash(" AA"sv));
    expect(0_i == hash("AA "sv));
    expect(0_i == hash(" AA_"sv));
    expect(0_i == hash("_AA_"sv));
  };

  "[hash] variable length"_test = [verify] {
    constexpr std::array<std::pair<mph::fixed_string, int>, 6> keys{
        {{"ftp", 1},
         {"file", 2},
         {"http", 3},
         {"https", 4},
         {"ws", 5},
         {"wss", 6}}};
    constexpr auto hash = mph::hash<0, keys>;

    verify(keys, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash("udp"sv));
    expect(0_i == hash("HTTP"sv));
    expect(0_i == hash("http2"sv));
    expect(0_i == hash("https!"sv));
  };

  "[hash] multiple policies trigger"_test = [verify] {
    constexpr std::array<std::pair<mph::fixed_string, std::uint8_t>, 100> keys{
        std::pair{mph::fixed_string{"III     "}, 1},
        std::pair{mph::fixed_string{"AGM-C   "}, 2},
        std::pair{mph::fixed_string{"LOPE    "}, 3},
        std::pair{mph::fixed_string{"FEMS    "}, 4},
        std::pair{mph::fixed_string{"IEA     "}, 5},
        std::pair{mph::fixed_string{"VYMI    "}, 6},
        std::pair{mph::fixed_string{"BHK     "}, 7},
        std::pair{mph::fixed_string{"SIEB    "}, 8},
        std::pair{mph::fixed_string{"DGBP    "}, 9},
        std::pair{mph::fixed_string{"INFN    "}, 10},
        std::pair{mph::fixed_string{"USRT    "}, 11},
        std::pair{mph::fixed_string{"BCOR    "}, 12},
        std::pair{mph::fixed_string{"TWM     "}, 13},
        std::pair{mph::fixed_string{"BVSN    "}, 14},
        std::pair{mph::fixed_string{"STBA    "}, 15},
        std::pair{mph::fixed_string{"GPK     "}, 16},
        std::pair{mph::fixed_string{"LVHD    "}, 17},
        std::pair{mph::fixed_string{"FTEK    "}, 18},
        std::pair{mph::fixed_string{"GLBS    "}, 19},
        std::pair{mph::fixed_string{"CUBB    "}, 20},
        std::pair{mph::fixed_string{"LRCX    "}, 21},
        std::pair{mph::fixed_string{"HTGM    "}, 22},
        std::pair{mph::fixed_string{"RYN     "}, 23},
        std::pair{mph::fixed_string{"IPG     "}, 24},
        std::pair{mph::fixed_string{"PNNTG   "}, 25},
        std::pair{mph::fixed_string{"ZIG     "}, 26},
        std::pair{mph::fixed_string{"IVR-A   "}, 27},
        std::pair{mph::fixed_string{"INVA    "}, 28},
        std::pair{mph::fixed_string{"MNE     "}, 29},
        std::pair{mph::fixed_string{"KRA     "}, 30},
        std::pair{mph::fixed_string{"BRMK    "}, 31},
        std::pair{mph::fixed_string{"ARKG    "}, 32},
        std::pair{mph::fixed_string{"FFR     "}, 33},
        std::pair{mph::fixed_string{"QTRX    "}, 34},
        std::pair{mph::fixed_string{"XTN     "}, 35},
        std::pair{mph::fixed_string{"BAC-A   "}, 36},
        std::pair{mph::fixed_string{"CYBE    "}, 37},
        std::pair{mph::fixed_string{"ETJ     "}, 38},
        std::pair{mph::fixed_string{"JHCS    "}, 39},
        std::pair{mph::fixed_string{"RBCAA   "}, 40},
        std::pair{mph::fixed_string{"GDS     "}, 41},
        std::pair{mph::fixed_string{"WTID    "}, 42},
        std::pair{mph::fixed_string{"TCO     "}, 43},
        std::pair{mph::fixed_string{"BWA     "}, 44},
        std::pair{mph::fixed_string{"MIE     "}, 45},
        std::pair{mph::fixed_string{"GENY    "}, 46},
        std::pair{mph::fixed_string{"TDOC    "}, 47},
        std::pair{mph::fixed_string{"MCRO    "}, 48},
        std::pair{mph::fixed_string{"QFIN    "}, 49},
        std::pair{mph::fixed_string{"NBTB    "}, 50},
        std::pair{mph::fixed_string{"PWC     "}, 51},
        std::pair{mph::fixed_string{"FQAL    "}, 52},
        std::pair{mph::fixed_string{"NJAN    "}, 53},
        std::pair{mph::fixed_string{"IWB     "}, 54},
        std::pair{mph::fixed_string{"GXGXW   "}, 55},
        std::pair{mph::fixed_string{"EDUC    "}, 56},
        std::pair{mph::fixed_string{"RETL    "}, 57},
        std::pair{mph::fixed_string{"VIACA   "}, 58},
        std::pair{mph::fixed_string{"KLDO    "}, 59},
        std::pair{mph::fixed_string{"NEE-I   "}, 60},
        std::pair{mph::fixed_string{"FBC     "}, 61},
        std::pair{mph::fixed_string{"JW.A    "}, 62},
        std::pair{mph::fixed_string{"BSMX    "}, 63},
        std::pair{mph::fixed_string{"FMNB    "}, 64},
        std::pair{mph::fixed_string{"EXR     "}, 65},
        std::pair{mph::fixed_string{"TAC     "}, 66},
        std::pair{mph::fixed_string{"FDL     "}, 67},
        std::pair{mph::fixed_string{"SWIR    "}, 68},
        std::pair{mph::fixed_string{"CLWT    "}, 69},
        std::pair{mph::fixed_string{"LMHB    "}, 70},
        std::pair{mph::fixed_string{"IRTC    "}, 71},
        std::pair{mph::fixed_string{"CDMO    "}, 72},
        std::pair{mph::fixed_string{"HMLP-A  "}, 73},
        std::pair{mph::fixed_string{"LVUS    "}, 74},
        std::pair{mph::fixed_string{"UMRX    "}, 75},
        std::pair{mph::fixed_string{"GJH     "}, 76},
        std::pair{mph::fixed_string{"FRME    "}, 77},
        std::pair{mph::fixed_string{"CEIX    "}, 78},
        std::pair{mph::fixed_string{"IHD     "}, 79},
        std::pair{mph::fixed_string{"GHSI    "}, 80},
        std::pair{mph::fixed_string{"DCP-B   "}, 81},
        std::pair{mph::fixed_string{"SB      "}, 82},
        std::pair{mph::fixed_string{"DSE     "}, 83},
        std::pair{mph::fixed_string{"CPRT    "}, 84},
        std::pair{mph::fixed_string{"NRZ     "}, 85},
        std::pair{mph::fixed_string{"VLYPO   "}, 86},
        std::pair{mph::fixed_string{"TDAC    "}, 87},
        std::pair{mph::fixed_string{"ZXZZT   "}, 88},
        std::pair{mph::fixed_string{"IWX     "}, 89},
        std::pair{mph::fixed_string{"NCSM    "}, 90},
        std::pair{mph::fixed_string{"WIRE    "}, 91},
        std::pair{mph::fixed_string{"SFST    "}, 92},
        std::pair{mph::fixed_string{"EWD     "}, 93},
        std::pair{mph::fixed_string{"DEACW   "}, 94},
        std::pair{mph::fixed_string{"TRPX    "}, 95},
        std::pair{mph::fixed_string{"UCTT    "}, 96},
        std::pair{mph::fixed_string{"ZAZZT   "}, 97},
        std::pair{mph::fixed_string{"CYD     "}, 98},
        std::pair{mph::fixed_string{"NURE    "}, 99},
        std::pair{mph::fixed_string{"WEAT    "}, 100},
    };

    const auto hash = mph::hash<std::uint8_t{}, keys>;

    verify(keys, hash);

    expect(0_u == hash("        "sv));
    expect(0_u == hash(" III    "sv));
    expect(0_u == hash("  III   "sv));
    expect(0_u == hash("   III  "sv));
    expect(0_u == hash("    III "sv));
    expect(0_u == hash("     III"sv));
  };

#if (defined(__GNUC__) and not defined(__clang__)) or \
    (defined(__clang__) and (__clang_major__ >= 18))
  "map"_test = [verify] {
    auto keys = mph::map<{"a", 0}, {"b", 1}, {"c", 2}>;

    expect(0_i == keys["a"sv]);
    expect(1_i == keys["b"]);
    expect(2_i == keys["c"]);

    expect(-1_i == keys[""sv]);
    expect(-1_i == keys["foo"sv]);
    expect(-1_i == keys["bar"sv]);
    expect(-1_i == keys["d"sv]);
  };
#endif
}
