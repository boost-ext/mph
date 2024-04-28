<a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a>
<a href="https://github.com/boost-ext/mph/releases" target="_blank">![Version](https://badge.fury.io/gh/boost-ext%2Fmph.svg)</a>
<a href="https://godbolt.org/z/x94s9WPv1">![build](https://img.shields.io/badge/build-blue.svg)</a>
<a href="https://godbolt.org/z/4qsGj7741">![Try it online](https://img.shields.io/badge/try%20it-online-blue.svg)</a>

---------------------------------------

## Perfect hash library

> https://en.wikipedia.org/wiki/Perfect_hash_function#Perfect_hash_function

### Use case

> Given a list of N keys (known at compile-time) find a perfect hash function (maps keys into range from 0 to N-1) with the fastest run-time execution.

### Features

- Single header (https://raw.githubusercontent.com/boost-ext/mph/main/mph)
    - Easy integration (see [FAQ](#faq))
- Self verfication upon include (can be disabled by `DISABLE_STATIC_ASSERT_TESTS`)
- Compiles cleanly with ([`-Wall -Wextra -Werror -pedantic -pedantic-errors`](https://godbolt.org/z/sdqW48MEv))
- Minimal [API](#api)
- Optimized run-time execution (see [performance](#performance) / [benchmarks](#benchmarks))

### Requirements

- C++20 ([gcc-12+](https://godbolt.org/z/sdqW48MEv), [clang-15+](https://godbolt.org/z/sdqW48MEv))

    - No STL headers required

### Hello world (https://godbolt.org/z/4qsGj7741)

```cpp
enum class color { red = 1, green = 2, blue = 3 };

constexpr auto colors = std::array{
  std::pair{mph::fixed_string{"red"}, color::red},
  std::pair{mph::fixed_string{"green"}, color::green},
  std::pair{mph::fixed_string{"blue"}, color::blue},
};

static_assert(color::green == mph::hash<colors, color(0)>("green"));
static_assert(color::red   == mph::hash<colors, color(0)>("red"));
static_assert(color::blue  == mph::hash<colors, color(0)>("blue"));

std::print("{}", mph::hash<colors, color(0)>("green")); // prints 1
```

---

<a name="performance"></a>
### Performance (https://godbolt.org/z/n148debY7)

```cpp
int main(int argc, char**)
  constexpr std::array ids{
    std::pair{ 54u,  91u},
    std::pair{324u,  54u},
    std::pair{ 64u, 324u},
    std::pair{234u,  64u},
    std::pair{ 91u, 234u},
  };
  return mph::hash<ids>(argc);
}
```

```cpp
main(int): // g++ -DNDEBUG -std=c++20 -O3 -march=skylake
  movl $7, %edx
  xorl %eax, %eax
  pext %edx, %edi, %edx
  movl %edx, %edx
  cmpl %edi, lut(,%rdx,8)
  cmove lut+4(,%rdx,8), %eax
  ret
```

---

### Performance (https://godbolt.org/z/x677cYzrf)

```cpp
int main(int argc, const char** argv) {
  constexpr auto symbols = std::array{
    std::pair{mph::fixed_string{"AAPL    "}, 1},
    std::pair{mph::fixed_string{"AMZN    "}, 2},
    std::pair{mph::fixed_string{"GOOGL   "}, 3},
    std::pair{mph::fixed_string{"MSFT    "}, 4},
    std::pair{mph::fixed_string{"NVDA    "}, 5},
  };
  return mph::hash<symbols>(std::span<const char, 8u>(argv[1], argv[1] + 8u));
}
```

```
main: // g++ -DNDEBUG -std=c++20 -O3 -march=skylake
  movq 8(%rsi), %rax
  xorl %ecx, %ecx
  movq (%rax), %rdx
  movl $1029, %eax
  pext %rax, %rdx, %rax
  cmpq lut(%rax), %rdx
  jne .L1
  movl lut+8(%rax), %ecx
.L1:
  movl %ecx, %eax
  ret
```

### Performance [potentially unsafe] (https://godbolt.org/z/vjrsY35x8)

> If `all` possible inputs are known AND can be found in the keys, then `unconditional` policy for the config can be used which will avoid one comparison

```cpp
int main(int argc, [[maybe_unused]] const char** argv) {
  static constexpr auto symbols = std::array{
    std::pair{std::array{'A', 'A', 'P', 'L', ' ', ' ', ' ', ' '}, 1},
    std::pair{std::array{'A', 'M', 'Z', 'N', ' ', ' ', ' ', ' '}, 2},
    std::pair{std::array{'G', 'O', 'O', 'G', 'L', ' ', ' ', ' '}, 3},
    std::pair{std::array{'M', 'S', 'F', 'T', ' ', ' ', ' ', ' '}, 4},
    std::pair{std::array{'N', 'V', 'D', 'A', ' ', ' ', ' ', ' '}, 5},
  };

  return mph::hash<symbols, 0, mph::unconditionl>(symbols[argc].first);
}
```

```
main: // g++ -DNDEBUG -std=c++20 -O3 -march=skylake
  movq 8(%rsi), %rax
  movl $1029, %edx
  movq (%rax), %rax
  pext %rdx, %rax, %rax
  movl lut+8(%rax), %eax
  ret
```

---

<a name="benchmarks"></a>
### Benchmarks (https://github.com/boost-ext/mph/tree/benchmark)

> `clang++ -std=c++20 -O3 -DNDEBUG -march=skylake benchmark.cpp`

```
| ns/op |           op/s | err% |total | benchmark
|------:|---------------:|-----:|-----:|:----------
| 12.25 |  81,602,449.70 | 0.3% | 0.15 | `random_strings_5_len_4.std.map`
|  5.56 | 179,750,906.50 | 0.2% | 0.07 | `random_strings_5_len_4.std.unordered_map`
|  9.17 | 109,096,850.98 | 0.2% | 0.11 | `random_strings_5_len_4.boost.unordered_map`
| 13.48 |  74,210,250.54 | 0.3% | 0.16 | `random_strings_5_len_4.boost.flat_map`
|  7.70 | 129,942,965.18 | 0.3% | 0.09 | `random_strings_5_len_4.gperf`
|  1.61 | 621,532,188.81 | 0.1% | 0.02 | `random_strings_5_len_4.mph`
| 14.66 |  68,218,086.71 | 0.8% | 0.18 | `random_strings_5_len_8.std.map`
| 13.45 |  74,365,239.56 | 0.2% | 0.16 | `random_strings_5_len_8.std.unordered_map`
|  9.68 | 103,355,605.09 | 0.2% | 0.12 | `random_strings_5_len_8.boost.unordered_map`
| 16.00 |  62,517,180.19 | 0.4% | 0.19 | `random_strings_5_len_8.boost.flat_map`
|  7.70 | 129,809,356.36 | 0.3% | 0.09 | `random_strings_5_len_8.gperf`
|  1.58 | 633,084,194.24 | 0.1% | 0.02 | `random_strings_5_len_8.mph`
| 17.21 |  58,109,576.87 | 0.3% | 0.21 | `random_strings_6_len_2_5.std.map`
| 15.28 |  65,461,167.99 | 0.2% | 0.18 | `random_strings_6_len_2_5.std.unordered_map`
| 12.21 |  81,931,391.20 | 0.4% | 0.15 | `random_strings_6_len_2_5.boost.unordered_map`
| 17.15 |  58,323,741.08 | 0.5% | 0.21 | `random_strings_6_len_2_5.boost.flat_map`
|  7.94 | 125,883,197.55 | 0.5% | 0.09 | `random_strings_6_len_2_5.gperf`
|  6.05 | 165,239,616.00 | 0.5% | 0.07 | `random_strings_6_len_2_5.mph`
| 31.61 |  31,631,402.94 | 0.2% | 0.38 | `random_strings_100_len_8.std.map`
| 15.32 |  65,280,594.09 | 0.2% | 0.18 | `random_strings_100_len_8.std.unordered_map`
| 17.13 |  58,383,850.20 | 0.3% | 0.20 | `random_strings_100_len_8.boost.unordered_map`
| 31.42 |  31,822,519.67 | 0.2% | 0.38 | `random_strings_100_len_8.boost.flat_map`
|  8.04 | 124,397,773.85 | 0.2% | 0.10 | `random_strings_100_len_8.gperf`
|  1.58 | 632,813,481.73 | 0.1% | 0.02 | `random_strings_100_len_8.mph`
| 32.62 |  30,656,015.03 | 0.3% | 0.39 | `random_strings_100_len_1_8.std.map`
| 19.34 |  51,697,107.73 | 0.5% | 0.23 | `random_strings_100_len_1_8.std.unordered_map`
| 19.51 |  51,254,525.17 | 0.3% | 0.23 | `random_strings_100_len_1_8.boost.unordered_map`
| 33.58 |  29,780,574.17 | 0.6% | 0.40 | `random_strings_100_len_1_8.boost.flat_map`
| 13.06 |  76,577,037.07 | 0.7% | 0.16 | `random_strings_100_len_1_8.gperf`
|  6.02 | 166,100,665.07 | 0.2% | 0.07 | `random_strings_100_len_1_8.mph`
|  1.28 | 778,723,795.75 | 0.1% | 0.02 | `random_uints_5.mph`
```

> `g++ -std=c++20 -O3 -DNDEBUG -march=skylake benchmark.cpp`

```cpp
| ns/op |           op/s | err% |total | benchmark
|------:|---------------:|-----:|-----:|:----------
| 12.28 |  81,460,330.38 | 0.9% | 0.15 | `random_strings_5_len_4.std.map`
|  5.29 | 188,967,241.90 | 0.3% | 0.06 | `random_strings_5_len_4.std.unordered_map`
|  9.69 | 103,163,192.67 | 0.2% | 0.12 | `random_strings_5_len_4.boost.unordered_map`
| 13.56 |  73,756,333.08 | 0.4% | 0.16 | `random_strings_5_len_4.boost.flat_map`
|  7.69 | 130,055,662.66 | 0.6% | 0.09 | `random_strings_5_len_4.gperf`
|  1.39 | 718,910,252.82 | 0.1% | 0.02 | `random_strings_5_len_4.mph`
| 14.26 |  70,103,007.82 | 2.4% | 0.17 | `random_strings_5_len_8.std.map`
| 13.36 |  74,871,047.51 | 0.4% | 0.16 | `random_strings_5_len_8.std.unordered_map`
|  9.82 | 101,802,074.00 | 0.3% | 0.12 | `random_strings_5_len_8.boost.unordered_map`
| 15.97 |  62,621,571.95 | 0.3% | 0.19 | `random_strings_5_len_8.boost.flat_map`
|  7.92 | 126,265,206.30 | 0.3% | 0.09 | `random_strings_5_len_8.gperf`
|  1.40 | 713,596,376.62 | 0.4% | 0.02 | `random_strings_5_len_8.mph`
| 15.98 |  62,576,142.34 | 0.5% | 0.19 | `random_strings_6_len_2_5.std.map`
| 17.56 |  56,957,868.12 | 0.5% | 0.21 | `random_strings_6_len_2_5.std.unordered_map`
| 11.68 |  85,637,378.45 | 0.3% | 0.14 | `random_strings_6_len_2_5.boost.unordered_map`
| 17.25 |  57,965,732.68 | 0.6% | 0.21 | `random_strings_6_len_2_5.boost.flat_map`
|  9.13 | 109,580,632.48 | 0.7% | 0.11 | `random_strings_6_len_2_5.gperf`
|  7.17 | 139,563,745.72 | 0.4% | 0.09 | `random_strings_6_len_2_5.mph`
| 30.20 |  33,117,522.76 | 0.7% | 0.36 | `random_strings_100_len_8.std.map`
| 15.01 |  66,627,962.89 | 0.4% | 0.18 | `random_strings_100_len_8.std.unordered_map`
| 16.79 |  59,559,414.60 | 0.6% | 0.20 | `random_strings_100_len_8.boost.unordered_map`
| 31.36 |  31,884,629.57 | 0.8% | 0.38 | `random_strings_100_len_8.boost.flat_map`
|  7.75 | 128,973,947.61 | 0.7% | 0.09 | `random_strings_100_len_8.gperf`
|  1.50 | 667,041,673.54 | 0.1% | 0.02 | `random_strings_100_len_8.mph`
| 30.92 |  32,340,612.08 | 0.4% | 0.37 | `random_strings_100_len_1_8.std.map`
| 25.35 |  39,450,222.09 | 0.4% | 0.30 | `random_strings_100_len_1_8.std.unordered_map`
| 19.76 |  50,609,820.90 | 0.2% | 0.24 | `random_strings_100_len_1_8.boost.unordered_map`
| 32.39 |  30,878,018.77 | 0.6% | 0.39 | `random_strings_100_len_1_8.boost.flat_map`
| 11.20 |  89,270,687.92 | 0.2% | 0.13 | `random_strings_100_len_1_8.gperf`
|  7.17 | 139,471,159.67 | 0.5% | 0.09 | `random_strings_100_len_1_8.mph`
|  1.93 | 519,047,110.39 | 0.3% | 0.02 | `random_uints_5.mph`
```

---

### API

```cpp
/**
 * Perfect hash function
 *
 * @tparam kv constexpr array of key/value pairs
 * @tparam unknown default value
 * @param key input data
 */
template<
  auto kv,
  typename decltype(kv)::value_type::second_type unknown = {},
  auto policy = conditional
>
[[nodiscard]] constexpr auto hash(auto&& key) noexcept -> decltype(unknown);
```

> Policies

```cpp
inline constexpr auto conditional =
  [](const bool cond, const auto lhs, const auto rhs) {
    return cond ? lhs : rhs; // generates jmp (x86-64)
  };
```

```cpp
inline constexpr auto unconditional =
  []([[maybe_unused]] const bool cond, const auto lhs, [[maybe_unused]] const auto rhs) {
    return lhs; // [unsafe] returns unconditionally
  };
```

```cpp
inline constexpr auto likely =
  [](const bool cond, const auto lhs, const auto rhs) {
    if (cond) [[likely]] {
      return lhs;
    } else {
      return rhs;
    }
  };
```

```cpp
inline constexpr auto unlikely =
  [](const bool cond, const auto lhs, const auto rhs) {
    if (cond) [[unlikely]] {
      return lhs;
    } else {
      return rhs;
    }
  };
```

```cpp
template<auto Probablity>
inline constexpr auto conditional_probability =
  [](const bool cond, const auto lhs, const auto rhs) {
    if (__builtin_expect_with_probability(cond, 1, Probablity)) {
      return lhs;
    } else {
      return rhs;
    }
  };
```

```cpp
inline constexpr auto branchless =
  [](const bool cond, const auto lhs, [[maybe_unused]] const auto rhs) {
    return cond * lhs; // generates cmov (x86-64)
  };
```

> Configuration

```cpp
#define MPH 2'0'0 // Current library version (SemVer)
#define MPH_PAGE_SIZE 4096u // Only used for string-like keys
```

---

### FAQ

- How `mph` works under the hood?

    > `mph` takes advantage of knowing the key/value pairs at compile-time as well as the specific hardware instructions.
      `mph` evaluates, at compile-time, which policies can be used and which will deliver the fastest performance.
      `mph`, then, picks the 'best' one and apply input data to it.

- How to get the max performance out of `mph`?

    > Always measure! Custom polices might be a good place to start. For strings, consider aligning the input data and passing it with compile-time size via std::span, std::array.

- Can I disable running tests at compile-time for faster compilation times?

    > When `DISABLE_STATIC_ASSERT_TESTS` is defined static_asserts tests won't be executed upon inclusion.
    Note: Use with caution as disabling tests means that there are no gurantees upon inclusion that given compiler/env combination works as expected.

- How to integrate with CMake/CPM?

    ```
    CPMAddPackage(
      Name mph
      GITHUB_REPOSITORY boost-ext/mph
      GIT_TAG v2.0.0
    )
    add_library(mph INTERFACE)
    target_include_directories(mph SYSTEM INTERFACE ${mph_SOURCE_DIR})
    add_library(mph::mph ALIAS mph)
    ```

    ```
    target_link_libraries(${PROJECT_NAME} mph::mph);
    ```

- Similar projects?

    > [gperf](https://www.gnu.org/software/gperf), [frozen](https://github.com/serge-sans-paille/frozen), [nbperf](https://github.com/rurban/nbperf), [cmph](https://cmph.sourceforge.net), [perfecthash](https://github.com/tpn/perfecthash), [lemonhash](https://github.com/ByteHamster/LeMonHash), [pthash](https://github.com/jermp/pthash), [shockhash](https://github.com/ByteHamster/ShockHash), [BuRR](https://github.com/lorenzhs/BuRR), [hash-prospector](https://github.com/skeeto/hash-prospector)

- Acknowledgments

    > http://0x80.pl, https://lemire.me/blog, [pefect-hashing](https://github.com/tpn/pdfs/tree/master/Perfect%20Hashing), [gperf](https://www.dre.vanderbilt.edu/~schmidt/PDF/C++-USENIX-90.pdf), [cmph](https://cmph.sourceforge.net/papers), [smasher](https://github.com/rurban/smhasher), [minimal perfect hashing](http://stevehanov.ca/blog/index.php?id=119), [prospecting for hash functions](https://nullprogram.com/blog/2018/07/31)

---

**Disclaimer** `mph` is not an official Boost library.
