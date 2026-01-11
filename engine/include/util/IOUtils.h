//
// Created by simon on 11/01/2026.
//

#ifndef NITRONIC_IOUTILS_H
#define NITRONIC_IOUTILS_H
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

static std::vector<char> readFile(const std::filesystem::path& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        std::cout << std::filesystem::current_path() << std::endl;
        throw std::runtime_error("failed to open file!");
    }

    const size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

#endif //NITRONIC_IOUTILS_H