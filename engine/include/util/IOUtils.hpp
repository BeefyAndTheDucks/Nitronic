//
// Created by simon on 11/01/2026.
//

#ifndef NITRONIC_IOUTILS_HPP
#define NITRONIC_IOUTILS_HPP
#include <filesystem>
#include <fstream>
#include <vector>

#include "engine/Log.hpp"

static std::vector<char> readFile(const std::filesystem::path& filePath) {
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        ENGINE_ERROR("Failed to open file: {} from {}", filePath.string(), std::filesystem::current_path().string());
        throw std::runtime_error("failed to open file!");
    }

    const std::streamsize fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

#endif //NITRONIC_IOUTILS_HPP