#pragma once

#include <cinttypes>
#include <iostream>
#include <bitset>

uint8_t GetBit(const uint8_t* data, size_t ind, size_t to_pos);

void XorBit(uint8_t* data, size_t ind, uint8_t value);

bool IsPowerOfTwo(size_t x);

uint8_t* Encode(uint8_t*& data, size_t parity, size_t& data_size);

uint8_t* Decode(uint8_t*& data, size_t parity, size_t& data_size);
//if data is corrupted returns nullptr
// Unmatching Encode/Decode parity = UB
