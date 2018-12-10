#include "utils.hpp"

template<std::size_t N>
void generate_init_vector(uint8_t (&IV_buff)[N]) {
    using bytes_randomizer = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, uint8_t>;
    std::default_random_engine rd;
    bytes_randomizer bytes(rd);

    std::generate(std::begin(IV_buff), std::end(IV_buff), std::ref(bytes));
}

