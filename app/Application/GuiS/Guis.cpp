#include <Application/GuiS/Guis.h>
#include <imgui.h>
#include <utils/utils.h>
#include <imfilebrowser.h>

ImGui::FileBrowser Guis::m_fd_roms;
ImGui::FileBrowser Guis::m_fd_settings;
MemoryEditor Guis::mem_edit;


void Guis::InitMemDumpMenu()
{
    mem_edit.ReadOnly = true;
    mem_edit.GotoAddr = 0x200;
}

void Guis::DrawInfoMenu(const char *label, u32 target_ips, const double fps_count, const u32 ips_count) {
    ImGui::Begin(label);

    ImGui::Text("FPS: %f", fps_count);
    ImGui::Text("IPS: %u", ips_count);
    ImGui::Text("TIPS: %u", target_ips);
    ImGui::End();
}

void Guis::RenderSpace(const u32 id, const float w, const float h) {
    ImGui::Begin("Render Space");
    const auto surSize = ImGui::GetContentRegionAvail();
    ImGui::Image((ImTextureID)id, ImVec2(w*(surSize.x/w), h*surSize.y/h));
    ImGui::End();
}

void Guis::DrawSettingsMenu(settings *data) {
    ImGui::Begin("Settings");
    

    float bg[4] = {
        static_cast<float>((data->background_color & 0x00'00'00'FF))/0xFF,
        static_cast<float>((data->background_color & 0x00'00'FF'00) >> (8 * 1))/0xFF,
        static_cast<float>((data->background_color & 0x00'FF'00'00) >> (8 * 2))/0xFF, 1
    };
    float fg[4] = {
        static_cast<float>((data->foreground_color & 0x00'00'00'FF))/0xFF,
        static_cast<float>((data->foreground_color & 0x00'00'FF'00) >> (8 * 1))/0xFF,
        static_cast<float>((data->foreground_color & 0x00'FF'00'00) >> (8 * 2))/0xFF, 1
    };
    if (ImGui::ColorEdit4("Background Color", bg)) {
        data->background_color = (static_cast<u32>(0xFF * bg[3]) << (8 * 3)) | (static_cast<u32>(0xFF * bg[2]) << (8 * 2)) | (static_cast<u32>(0xFF * bg[1]) << 8) | static_cast<u32>(0xFF * bg[0]);
    }
    if (ImGui::ColorEdit4("Foreground Color", fg)) {
        data->foreground_color = (static_cast<u32>(0xFF * fg[3]) << (8 * 3)) | (static_cast<u32>(0xFF * fg[2]) << (8 * 2)) | (static_cast<u32>(0xFF * fg[1]) << 8) | static_cast<u32>(0xFF * fg[0]);
    }

    // ImGui::InputInt("Target IPS", reinterpret_cast<i32*>(&settings->target_ips), 1, 10);
    ImGui::SliderInt("Target IPS", reinterpret_cast<i32*>(&data->target_ips), TIPS_MIN, TIPS_MAX);
    if (ImGui::BeginPopupContextItem("Some")) {
        // ImGui::MenuItem("Some");
        ImGui::InputInt("##", reinterpret_cast<i32*>(&data->target_ips), 1, 10);
        if (ImGui::MenuItem("Reset")) {
            data->target_ips = TIPS_STANDARD;
        }
        ImGui::EndPopup();
    }

    data->target_ips = std::min(TIPS_MAX, std::max(static_cast<i32>(data->target_ips), TIPS_MIN));

    if (ImGui::Button("Save")) {
        saveSettingTo(*data, fs::path("settings.ces"));
    }
    ImGui::SameLine();


    if (ImGui::Button("Load")) {
        m_fd_settings.SetTitle("Select settings file");
        m_fd_settings.SetTypeFilters({".ces"});
        m_fd_settings.Open();
    }

    ImGui::End();
    m_fd_settings.Display();

    if (m_fd_settings.HasSelected()) {

        loadSettingFrom(*data, fs::path(m_fd_settings.GetSelected().string()));
        

        m_fd_settings.ClearSelected();
    }
}

void Guis::DrawEmulatorRegistersTable(const u8* data) {
    ImGui::Begin("Registers");
    if (ImGui::BeginTable("U8 Array Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        // Заголовки колонок
        ImGui::TableSetupColumn("Register");
        ImGui::TableSetupColumn("Hex");
        ImGui::TableSetupColumn("Dec");

        ImGui::TableHeadersRow();

        for (size_t i = 0; i < 16; ++i) {
            ImGui::TableNextRow();

            // Индекс
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("V[%zu]", i);

            // Шестнадцатеричное значение
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("0x%02X", data[i]);

            // Десятичное значение
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%u", data[i]);
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void Guis::DrawEmulatorPointersTable(std::vector<std::pair<std::string, u32>> data) {
    ImGui::Begin("Pointers");
    if (ImGui::BeginTable("U8 Array Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        // Заголовки колонок
        ImGui::TableSetupColumn("Register");
        ImGui::TableSetupColumn("Hex");
        ImGui::TableSetupColumn("Dec");

        ImGui::TableHeadersRow();

        for (size_t i = 0; i < data.size(); ++i) {
            ImGui::TableNextRow();

            // Индекс
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", data.at(i).first.c_str());

            // Шестнадцатеричное значение
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("0x%02X", data.at(i).second);

            // Десятичное значение
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%u", data.at(i).second);
        }
        ImGui::EndTable();

    }
    ImGui::End();
}

void Guis::DrawKeyTableMenu(std::array<bool, 16> layout)
{
    ImGui::Begin("Keyboard Layout");
    if (ImGui::BeginTable("Keyboard Layout Table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        // Заголовки колонок
        ImGui::TableSetupColumn("Keys");
        ImGui::TableSetupColumn("Pressed");

        ImGui::TableHeadersRow();

        for (size_t i = 0; i < layout.size(); ++i) {
            ImGui::TableNextRow();

            // Индекс
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%x", i);

            // Десятичное значение
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%i", layout.at(i));
        }
        ImGui::EndTable();

    }
    ImGui::End();
}

void Guis::DrawControlMenu(bool& isRunning, bool& nextStep) {
    ImGui::Begin("Controll");

    if (ImGui::Button((!isRunning ? "RUN" : "STOP"), ImVec2(40, 20))) {
        isRunning = !isRunning;
    }

    ImGui::SameLine();

    if (ImGui::Button("STEP",  ImVec2(40, 20))) {
        nextStep = true;
    }

    ImGui::End();
}

void Guis::DrawBreakPointsMenu(std::set<u16>& breakPoints)
{
    ImGui::Begin("Break Points");
    int id = 0;
    for (const auto& point : breakPoints) {
        ImGui::PushID(id);
        ImGui::Text("0x%04X", point);
        ImGui::PopID();
        id++;
    }
    ImGui::End();
}

void Guis::DrawKeyBindMenu(std::array<u32, 16>& keyBind)
{
    if (ImGui::Begin("Keybinds")) {
        // Loop through your keybinds and display them
        for (auto& item : keyBind) {
            ImGui::Text("0x%04X", item);
            ImGui::SameLine();
        }
    }
    ImGui::End();
}

void Guis::DrawProgramLoadMenu(ProgInfo& info, std::function<void(fs::path)> callback)
{
    ImGui::Begin("Program Loader");
    
    if (ImGui::BeginTable("Program Info", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {

        ImGui::TableHeadersRow();

        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Name");

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", info.title.c_str());
        }

        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Size");

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%u", info.size);
        }

        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Instructions");

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%u", info.insts);
        }

        ImGui::EndTable();
    }
     
    
    if (ImGui::Button("Load Program")) {
        m_fd_roms.SetTitle("Select CHIP-8 Program");
        m_fd_roms.SetTypeFilters({".ch8", ".c8", ".rom", ".bin", ".*"});
        m_fd_roms.Open();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reaload Current Program")) {
        if (callback != nullptr && !info.path.empty()) {
            if (fs::exists(info.path) && fs::is_regular_file(info.path))
                callback(info.path);
        }
    }

    ImGui::End();
    m_fd_roms.Display();

    if (m_fd_roms.HasSelected() && callback != nullptr) {
        callback(m_fd_roms.GetSelected());
		m_fd_roms.ClearSelected();
    }
}

void Guis::DrawResetMenu(std::function<void(bool, bool, bool)> callback)
{
    ImGui::Begin("Reset Menu");
    bool resetMem = false;
    bool resetRegs = false;
    bool resetVideo = false;

    if (ImGui::Button("Reset Registers")) {
        resetRegs = true;
    }

    if (ImGui::Button("Reset Memory")) {
        resetMem = true;
    }
    
    if (ImGui::Button("Reset Video Memory")) {
        resetVideo = true;
    }

    if (ImGui::Button("Reset All")) {
        resetVideo = true;
        resetMem = true;
        resetRegs = true;
    }

    callback(resetMem, resetRegs, resetVideo);
    ImGui::End();
}


void Guis::DrawMemoryMap(std::array<u8, 4096> memory)
{
    void* dump = reinterpret_cast<void*>(memory.data());
    mem_edit.DrawWindow("Memory Dump", dump, 4096);
}

void Guis::DisAsmMenu(std::vector<std::pair<std::pair<u16, u16>, std::string>> program, u32 PC, std::set<u16>& breakPoints) {
    ImGui::Begin("Disassembly");

    for (const auto& [techData, dism] : program) {
        bool f =false;
        bool contains = breakPoints.contains(techData.first);


        if (PC == techData.first) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow text, for example
            f=true;
        } else if (dism == "UNKNOWN") {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        } else if (contains) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(230.0/255, 121.0/255, 32.0/255, 1.0f));
        }

        ImGui::PushID(techData.first);

        if (ImGui::RadioButton("##", contains)) {
            if (contains) {
                breakPoints.erase(techData.first);
            } else {
                breakPoints.emplace(techData.first);
            }
        }
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::Text("0x%04X: (0x%04X)\t%s", techData.first, techData.second, dism.c_str());

        if (f || dism == "UNKNOWN" || contains) {
            ImGui::PopStyleColor();
            f=false;
        }
    }
    ImGui::End();
}
