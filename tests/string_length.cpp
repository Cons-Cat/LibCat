#include <string>

void meow() {
    char8_t const* p_string_1 = u8"Hello!";
    char const* const p_string_2 = "Hello!";

    char8_t const* const p_string_3 = u8"Hello!";
    char const* p_string_4 = "Hello!";

    char8_t const* const p_string_5 = u8"Hello!";
    char8_t const* const p_string_6 = u8"Hello!";

    isize len_1 = std::string_length(p_string_1);
    isize len_2 = std::string_length(p_string_2);
    isize len_3 = std::string_length(p_string_3);
    isize len_4 = std::string_length(p_string_4);
    isize len_5 = std::string_length(p_string_5);
    isize len_6 = std::string_length(p_string_6);

    Result(len_1 == len_2).or_print_panic();
    Result(len_3 == len_4).or_print_panic();
    Result(len_5 == len_6).or_print_panic();

    exit();
}
