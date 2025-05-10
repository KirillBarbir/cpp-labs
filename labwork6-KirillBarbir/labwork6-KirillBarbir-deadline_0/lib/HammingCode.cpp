#include "HammingCode.h"

void BeautifulOutput(uint8_t* data, size_t data_size);

uint8_t GetBit(const uint8_t* data, size_t ind, size_t to_pos) {
    size_t data_bit = ind / 8;
    size_t bit_number = 7 - ind % 8;
    return ((data[data_bit] & (1 << bit_number)) >> bit_number) << (7 - to_pos % 8);
}

void XorBit(uint8_t* data, size_t ind, uint8_t value) {
    data[ind / 8] ^= value;
}

bool IsPowerOfTwo(size_t x) {
    while (x % 2 == 0) {
        x /= 2;
    }
    return x == 1;
}

uint8_t* Encode(uint8_t*& data, size_t parity, size_t& data_size) {//в битах
    size_t bits_in_block = (1 << parity);
    size_t data_bits_in_block = bits_in_block - parity - 1;
    size_t output_bytes = (((data_size + data_bits_in_block - 1) / data_bits_in_block) * bits_in_block + 7) / 8;
    uint8_t* out = new uint8_t[output_bytes]{};
    for (size_t current_block = 0; current_block < (data_size + data_bits_in_block - 1) / data_bits_in_block;
         ++current_block) {
        size_t j = 0;
        for (size_t i = 0; i < bits_in_block && current_block * data_bits_in_block + j < data_size; ++i) {
            if (IsPowerOfTwo(i + 1)) {
                continue;
            }
            out[(i + current_block * bits_in_block) / 8] |=
                GetBit(data, current_block * data_bits_in_block + j, (i + current_block * bits_in_block) % 8);
            ++j;

        }
        for (size_t k = 0; k < parity; ++k) {
            for (size_t i = (1 << k) - 1; i < bits_in_block; i += (1 << (k + 1))) {
                for (size_t l = 0; l < (1 << k) && l + i < bits_in_block; ++l) {
                    XorBit(out,
                           (1 << k) - 1 + current_block * bits_in_block,
                           GetBit(out,
                                  current_block * bits_in_block + i + l,
                                  ((1 << k) - 1 + current_block * bits_in_block) % 8));
                }
            }
        }
        //DED
        for (size_t i = 0; i < bits_in_block - 1; ++i) {
            XorBit(out,
                   bits_in_block - 1 + current_block * bits_in_block,
                   GetBit(out,
                          current_block * bits_in_block + i,
                          (bits_in_block - 1 + current_block * bits_in_block) % 8));
        }
        //
    }
    delete[] data;
    data_size = output_bytes * 8;
    return out;
}

uint8_t* Decode(uint8_t*& data, size_t parity, size_t& data_size) {//в битах
    size_t bits_in_block = (1 << parity);
    size_t data_bits_in_block = bits_in_block - parity - 1;
    size_t output_bytes = ((data_size + bits_in_block - 1) / bits_in_block) * data_bits_in_block;
    output_bytes = (output_bytes + 7) / 8;
    uint8_t* out = new uint8_t[output_bytes]{};
    for (size_t current_block = 0; current_block < (data_size / data_bits_in_block); ++current_block) {
        //Проверка1 DED
        uint8_t* DED_bit = new uint8_t[1]{};
        for (size_t i = 0; i < bits_in_block - 1; ++i) {
            XorBit(DED_bit, 7, GetBit(data, current_block * bits_in_block + i, 7));
        }
        bool is_DEDbit_swapped = false;
        if (GetBit(data, current_block * bits_in_block + ((1 << parity) - 1), 7) != DED_bit[0]) {
            is_DEDbit_swapped = true;
        }
        //delete[] DED_bit;

        uint8_t* cur_bit = new uint8_t[1]{};
        uint8_t bit_to_fix = 0;
        for (size_t k = 0; k < parity; ++k) {
            if (current_block * bits_in_block + (1 << k) - 1 >= data_size) break;
            cur_bit[0] = GetBit(data, current_block * bits_in_block + (1 << k) - 1, 0);
            for (size_t i = (1 << k) - 1; i < bits_in_block; i += (1 << (k + 1))) {
                for (size_t l = 0; l < (1 << k) && l + i < bits_in_block; ++l) {
                    XorBit(cur_bit, 0, GetBit(data, current_block * bits_in_block + i + l, 0));
                }
            }
            if (GetBit(data, current_block * bits_in_block + ((1 << k) - 1), 0) != cur_bit[0]) {
                bit_to_fix += (1 << k);
            }
        }
        //delete[] cur_bit;
        if (bit_to_fix != 0) {
            if (!is_DEDbit_swapped) {
                std::cerr << "DATA IS CORRUPTED AND UNRECOVERABLE \n";
                return nullptr;
            }
            --bit_to_fix;
            XorBit(data, current_block * bits_in_block + bit_to_fix, 1 << (7 - bit_to_fix % 8));
        }

        size_t i = 0;

        for (size_t j = 0; j < bits_in_block && current_block * bits_in_block + j < data_size; ++j) {
            if (IsPowerOfTwo(j + 1)) {
                continue;
            }
            out[(current_block * data_bits_in_block + i) / 8] |=
                GetBit(data, current_block * bits_in_block + j, (i + current_block * data_bits_in_block) % 8);
            ++i;
        }
    }
    data_size = output_bytes * 8;
    delete[] data;
    return out;
}