#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

uint8_t* GetFileData(const std::filesystem::path& file_name, size_t start_pos, size_t amount);
void FileInsertData(const std::filesystem::path& file_name, uint8_t*& data, size_t data_size);
void FileDeleteData(const std::filesystem::path& file_name, size_t absolute_start, size_t absolute_size);