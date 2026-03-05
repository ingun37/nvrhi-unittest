//
// Created by Ingun Jon on 2/3/26.
//

#ifndef NVRHI_UNIT_TEST_BACKEND_H
#define NVRHI_UNIT_TEST_BACKEND_H
#include <string>
#include <vector>
#include <stdexcept>
#include <string_view>
#include <charconv>
#include <format>
#include <concepts>

inline std::string extension() {
#if defined(SCENARIO_VULKAN)
    return ".sprv";
#elif defined(SCENARIO_WGPU)
    return ".wgsl";
#else
    throw std::runtime_error("Unknown platform");
#endif
}

std::vector<char> read_shader(const std::string& name);

template <typename T>
void parse_single(std::string_view& sv, T* out) {
    if (!out) throw std::runtime_error("Null pointer passed to parser.");

    // 1. Skip leading whitespace
    size_t start = sv.find_first_not_of(" \t\n\r");
    if (start == std::string_view::npos) {
        throw std::runtime_error("Unexpected end of string: missing arguments.");
    }
    sv.remove_prefix(start);

    // 2. Identify the current word
    size_t end = sv.find_first_of(" \t\n\r");
    std::string_view word = sv.substr(0, end);

    // 3. Logic for Booleans
    if constexpr (std::same_as<T, bool>) {
        if (word == "true" || word == "1") *out = true;
        else if (word == "false" || word == "0") *out = false;
        else throw std::runtime_error(std::format("Invalid boolean: '{}'", word));
    } // 4. Logic for Numbers
    else {
        if constexpr (std::floating_point<T>) {
            // Floats handle their own signs, no base support in from_chars
            auto [ptr, ec] = std::from_chars(word.data(), word.data() + word.size(), *out);
            if (ec != std::errc{}) throw std::runtime_error(std::format("Float error: '{}'", word));
        } else if constexpr (std::integral<T>) {
            int base = 10;
            std::string_view parse_word = word;

            // Simple prefix detection (since we don't care about -0x...)
            if (parse_word.starts_with("0x") || parse_word.starts_with("0X")) {
                base = 16;
                parse_word.remove_prefix(2);
            } else if (parse_word.starts_with("0b") || parse_word.starts_with("0B")) {
                base = 2;
                parse_word.remove_prefix(2);
            }

            auto [ptr, ec] = std::from_chars(parse_word.data(), parse_word.data() + parse_word.size(), *out, base);

            // Note: from_chars for base 10 handles negative signs automatically
            if (ec != std::errc{}) throw std::runtime_error(std::format("Integer error: '{}'", word));
        }
    }

    // Advance the view
    if (end == std::string_view::npos) sv = {};
    else sv.remove_prefix(end);
}

template <typename... Args>
void parse_input(const std::string& input, Args*... args) {
    std::string_view sv = input;
    (parse_single(sv, args), ...);
}
#endif //NVRHI_UNIT_TEST_BACKEND_H