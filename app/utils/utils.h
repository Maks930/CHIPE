//
// Created by Maks930 on 15/11/2025.
//

#ifndef CHIPE_UTILS_H
#define CHIPE_UTILS_H

#include <filesystem>
#include <fstream>
#include <vector>
#include <Types.h>

#include "fmt/xchar.h"

#define _BG 0xFF000000
#define _FG 0xFF00FF00


namespace fs = std::filesystem;
class chip8;

inline bool saveSettingTo(settings& data, fs::path path) {
    std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }
    file.write("CES0", 4);
    file.write(reinterpret_cast<i8*>(&data), sizeof(settings));
    file.close();
    return true;
}

inline bool loadSettingFrom(settings& data, fs::path path) {
    std::ifstream file(path, std::ios::binary | std::ios::in);
    if (!file.is_open()) {
        return false;
    }
    u32 magic;
    file.read(reinterpret_cast<i8*>(&magic), 4);
    if (magic != 0x30534543) {
        return false;
    }

    file.read(reinterpret_cast<i8*>(&data), sizeof(settings));
    file.close();
    return true;
}

inline std::vector<std::string> recursiveFileSearch(const fs::path& path) {
    std::vector<std::string> result;

    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_directory()) {
            auto p = recursiveFileSearch(entry.path());
            result.insert(result.end(), p.begin(), p.end()) ;
        } else {
            result.push_back(entry.path().lexically_normal().string());
        }
    }

    return result;
}

// void loadRom(std::vector<std::string>::const_reference rom_path, chip8 * chp, std::vector<std::pair<std::pair<u16, u16>, std::string>>& dism) {
//     std::ifstream rom(rom_path, std::ios::binary);
//
//     rom.seekg(0, std::ifstream::end);
//     const u32 size = rom.tellg();
//     rom.seekg(0, std::ifstream::beg);
//     std::vector<u8> romData;
//     romData.resize(size);
//     rom.read(reinterpret_cast<char *>(romData.data()), size);
//     rom.close();
//
//     dism = chp->disAsmProg(romData.data(), romData.size());
//     chp->loadProgram(romData.data(), romData.size());
//
// }

#endif //CHIPE_UTILS_H