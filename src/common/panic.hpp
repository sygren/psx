#pragma once

#include <cstdlib>
#include <print>
#include <source_location>
#include <string_view>

[[noreturn]] constexpr inline void panic(std::string_view message = "", std::source_location loc = std::source_location::current()) {
  std::println(stderr, "PANIC at {}:{} ({}): {}", loc.file_name(), loc.line(), loc.function_name(), message);
  std::abort();
}
