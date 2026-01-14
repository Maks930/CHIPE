//
// Created by Maks930 on 15/11/2025.
//

#ifndef CHIPE_GUIS_H
#define CHIPE_GUIS_H

#include <string>
#include <Types.h>
#include <vector>
#include <set>
#include <functional>
#include <filesystem>

#include <imgui.h>
#include <imfilebrowser.h>
#include "imgui_memory_editor.h"

namespace fs = std::filesystem;

class Application;

class Guis {
public:
    static void InitMemDumpMenu();
    static void DrawInfoMenu(const char* label, u32 target_ips, double fps_count, const u32 ips_count);
    static void RenderSpace(u32 id, float w, float h);
    static void DrawSettingsMenu(settings* data);

    static void DrawEmulatorRegistersTable(const u8 *data);
    static void DrawEmulatorPointersTable(std::vector<std::pair<std::string, u32>> data);
    static void DrawKeyTableMenu(std::array<bool, 16> layout);

    static void DrawControlMenu(bool& isRunning, bool& nextStep);

    static void DrawBreakPointsMenu(std::set<u16>& breakPoints);

    static void DrawKeyBindMenu(std::array<u32, 16>& keyBind);

    static void DrawProgramLoadMenu(ProgInfo& info, std::function<void(fs::path)> callback = nullptr);

    static void DrawResetMenu(std::function<void(bool, bool, bool)>);
    static void DrawMemoryMap(std::array<u8, 4096> memory); //TODO: not safe memory usage

    static void DisAsmMenu(std::vector<std::pair<std::pair<u16,u16>, std::string>> program, u32 PC,std::set<u16>& breakPoints);

private:
    static ImGui::FileBrowser m_fd_roms;
    static ImGui::FileBrowser m_fd_settings;
    static MemoryEditor mem_edit;
};


#endif //CHIPE_GUIS_H