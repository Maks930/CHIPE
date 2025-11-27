//
// Created by Maks930 on 15/11/2025.
//

#ifndef CHIPE_GUIS_H
#define CHIPE_GUIS_H

#include <string>
#include <Types.h>
#include <vector>
#include <set>


class Guis {
public:
    static void DrawInfoMenu(const char* label, u32 target_ips, double fps_count, const u32 ips_count);
    static void RenderSpace(u32 id, float w, float h);
    static void DrawSettingsMenu(settings* data);

    static void DrawEmulatorRegistersTable(const u8 *data);
    static void DrawEmulatorPointersTable(std::vector<std::pair<std::string, u32>> data);
    static void DrawEmulatorMemoryTable();
    static void DrawKeyTableMenu(std::array<bool, 16> layout);

    static void DrawControlMenu(bool& isRunning, bool& nextStep);

    static void DrawBreakPointsMenu(std::set<u16>& breakPoints);

    static void DrawKeyBindMenu(std::array<u32, 16>& keyBind);

    static void DisAsmMenu(std::vector<std::pair<std::pair<u16,u16>, std::string>> program, u32 PC,std::set<u16>& breakPoints);
};


#endif //CHIPE_GUIS_H