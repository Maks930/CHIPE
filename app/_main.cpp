#include <fmt/base.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <Window/Window.h>
#include <Event/Event.h>
#include <Shader/Shader.h>
#include <cmath>
#include <cstring>

#include "chip/chip8.h"
#include <fstream>


#include <imgui.h>
#include <random>
#include <thread>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "imgui_internal.h"
#include "Keyboard/Keys.h"
#include "imgui_memory_editor.h"
#include <filesystem>
#include <iostream>

#include "fmt/format.h"

#include "utils/utils.h"

// AA BB GG RR (ABGR)


void RenderU8ArrayTable(const uint8_t* data, size_t size, u16 I, u16 PC, u8 dT, u8 sT) {
    if (ImGui::BeginTable("U8 Array Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        // Заголовки колонок
        ImGui::TableSetupColumn("Register");
        ImGui::TableSetupColumn("Hex");
        ImGui::TableSetupColumn("Dec");
        // ImGui::TableSetupColumn("Char");
        ImGui::TableHeadersRow();
        

        for (size_t i = 0; i < size; ++i) {
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

        ImGui::TableNextRow();

        // Индекс
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("I");

        // Шестнадцатеричное значение
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%02X", I);

        // Десятичное значение
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%u", I);

        ImGui::TableNextRow();
        // Индекс
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("PC");

        // Шестнадцатеричное значение
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%02X", PC);

        // Десятичное значение
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%u", PC);

        ImGui::TableNextRow();
        // Индекс
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("DT");

        // Шестнадцатеричное значение
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%02X", dT);

        // Десятичное значение
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%u", dT);

        ImGui::TableNextRow();
        // Индекс
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ST");

        // Шестнадцатеричное значение
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%02X", sT);

        // Десятичное значение
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%u", sT);

        ImGui::EndTable();
    }
}

void RenderU8RawArrayTable (const uint8_t* data, size_t size) {
    if (ImGui::BeginTable("U8 Array Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        // Заголовки колонок
        ImGui::TableSetupColumn("Key");
        ImGui::TableSetupColumn("Value");

        ImGui::TableHeadersRow();

        for (size_t i = 0; i < size; ++i) {
            ImGui::TableNextRow();

            // Индекс
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("K[%zu]", i);

            // Шестнадцатеричное значение
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%u", data[i]);
        }
        ImGui::EndTable();
    }
}

void ImGuiDrawU8Texture(const u32 id, float w, float h) {
    const auto surSize = ImGui::GetContentRegionAvail();
    ImGui::Image((ImTextureID)id, ImVec2(w*(surSize.x/w), h*surSize.y/h));
}






int main(int argc, char *argv[]) {

    auto *win = new bde::system::Window("CHIP-8", {1280, 720}, {{640, 480}, {20'000, 20'000}});

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;


    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch


    ImGui_ImplGlfw_InitForOpenGL(win->getHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    u32 BG = _BG;
    u32 FG = _FG;

    auto roms = recursiveFileSearch("./roms");

    // chp->loadProgram(romData.data(), romData.size());

    bool run = false;


    u8* img = new u8[64*48*4];
    auto img2 = reinterpret_cast<u32*>(img);
    std::memset(img, 0, 64*48*4);

    u32 renderTextureId = 0;
    glBindTexture(GL_TEXTURE_2D, renderTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 48, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

    auto *chp = new chip8();
    u32 inst_c = 0;
    u32 ips = 0;
    double target_frequency = 750.0f;

    std::thread emu_thread([&run, win, chp, &inst_c, &target_frequency]() {
        using clock = std::chrono::steady_clock;

        auto cur_target_frequency = target_frequency;
        std::chrono::duration<double> target_interval(1.0 / cur_target_frequency);

        auto next_time = clock::now() + target_interval;

        while (win->isOpen()) {
            if (cur_target_frequency != target_frequency) {
                cur_target_frequency = target_frequency;
                target_interval = std::chrono::duration<double>(1.0 / cur_target_frequency);
            }

            if (run) {
                chp->emulateCycle();
                inst_c+=1;
            }
            std::this_thread::sleep_until(next_time);
            next_time += target_interval;
        }
    });

    MemoryEditor mem_edit;
    mem_edit.ReadOnly = true;
    mem_edit.GotoAddr = 0x200;

    std::vector<std::pair<std::pair<u16, u16>, std::string>> prog;
    // prog = chip8::disAsmProg(romData.data(), romData.size());

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> dt = end-start;

    std::thread videoOut([img2, chp, win, &BG, &FG]() {
        while (true) {
            auto vm = chp->getVideoMemory();
            for (int i = 0; i < 64*48; i++) {
                if (!win->isOpen()) {
                    return;
                }
                if (vm[i] != 0) {
                    img2[i] = FG;
                } else {
                    img2[i] = BG;
                }
            }
        }
    });


    auto st1 = std::chrono::high_resolution_clock::now();
    while (win->isOpen()) {
        start = end;


        while (std::optional e = win->pollEvent()) {
            if (e->is<bde::system::Event::Closed>()) {
                win->close();
            }
            if (e->is<bde::system::Event::JustKeyPressed>()) {
                // chp->emulateCycle();
                // chp->reset();
                auto key = e->get_if<bde::system::Event::JustKeyPressed>();
                if ( key->code == GLFW_KEY_R) {
                    chp->resetRegisters();
                    chp->resetStack();
                    chp->resetProgramCounter();
                    chp->resetVideoMemory();
                    std::memset(img, 0, 64*48*4);

                } else if (key->code == GLFW_KEY_ESCAPE) {
                    win->close();
                } else if (key->code == GLFW_KEY_TAB) {
                    run = !run;
                } else if (key->code == GLFW_KEY_N && !run) {
                    // chp->emulateCycle();
                }
            }
        }
        win->clear();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();


        {
            ImGui::Begin("Registers");
            RenderU8ArrayTable(chp->getV(), 16, chp->getI(), chp->getPC(), chp->getDT(), chp->getST());
            ImGui::End();
        }

        {
            ImGui::Begin("Registers");
            RenderU8RawArrayTable(reinterpret_cast<u8 *>(chp->getKeyLayout()), 16);
            ImGui::End();
        }

        {
            ImGui::Begin("Renderer");
            ImGuiDrawU8Texture(renderTextureId, 64, 48);
            ImGui::End();
        }


        mem_edit.DrawWindow("Memory Dump", chp->getMemory(), 4096);

        {
            ImGui::Begin("Program Code");
            for (int i = 0; i < prog.size(); i++) {
                const auto item = prog.at(i);
                bool f =false;

                if (chp->getPC() == item.first.first) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow text, for example
                    f=true;
                } else if (item.second == "UNKNOWN") {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                }


                ImGui::Text("0x%04X -> 0x%04X:\t%s",item.first.first, item.first.second, item.second.c_str());
                if (f || item.second == "UNKNOWN") {
                    ImGui::PopStyleColor();
                    f=false;
                }

            }
            ImGui::End();
        }

        {
            ImGui::Begin("Programs");

            static int item_current = 0; // Stores the index of the currently selected item

            auto VectorListBoxGetter = [](void* data, int idx, const char** out_text) {
                std::vector<std::string>* myVector = static_cast<std::vector<std::string>*>(data);
                if (idx >= 0 && idx < myVector->size())
                {
                    *out_text = myVector->at(idx).c_str();
                    return true;
                }
                return false;
            };

            if (ImGui::Combo("My List (Callback)", &item_current, VectorListBoxGetter, static_cast<void*>(&roms), roms.size())) {
                prog.clear();
                chp->reset();
                loadRom(roms[item_current], chp, prog);
            }
            ImGui::Text("Selected item: %s", roms[item_current].c_str());
            ImGui::End();
        }

        {
            ImGui::Begin("KeyPad");
            for (int i = 0; i < 4*4; i++) {
                ImGui::PushID(i);
                if (ImGui::Button(fmt::format("{:x}",i).c_str())) {
                    chp->switchKey(i);

                }
                ImGui::PopID();
                if (i%4 != 3)
                    ImGui::SameLine();
            }
            ImGui::End();
        }

        {
            ImGui::Begin("Appearance");
            float bg[4] = {
                static_cast<float>((BG & 0x00'00'00'FF))/0xFF,
                static_cast<float>((BG & 0x00'00'FF'00) >> (8 * 1))/0xFF,
                static_cast<float>((BG & 0x00'FF'00'00) >> (8 * 2))/0xFF, 1
            };
            float fg[4] = {
                static_cast<float>((FG & 0x00'00'00'FF))/0xFF,
                static_cast<float>((FG & 0x00'00'FF'00) >> (8 * 1))/0xFF,
                static_cast<float>((FG & 0x00'FF'00'00) >> (8 * 2))/0xFF, 1
            };
            if (ImGui::ColorEdit4("Background Color: ", bg)) {
                BG = (static_cast<u32>(0xFF * bg[3]) << (8 * 3)) | (static_cast<u32>(0xFF * bg[2]) << (8 * 2)) | (static_cast<u32>(0xFF * bg[1]) << 8) | static_cast<u32>(0xFF * bg[0]);
            }
            if (ImGui::ColorEdit4("Foreground Color: ", fg)) {
                FG = (static_cast<u32>(0xFF * fg[3]) << (8 * 3)) | (static_cast<u32>(0xFF * fg[2]) << (8 * 2)) | (static_cast<u32>(0xFF * fg[1]) << 8) | static_cast<u32>(0xFF * fg[0]);
            }

            ImGui::End();
        }

        {

            auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end-st1).count();
            if (t >= 1000) {
                ips = inst_c;
                inst_c = 0;
                st1 = end;
            }
            auto fps = 1000.0f/dt.count();
            ImGui::Begin("Debug");
                ImGui::Text("DT: %u", std::chrono::duration_cast<std::chrono::milliseconds>(dt).count());
            ImGui::Text("FPS: %f", fps);
            ImGui::Text("IPS: %u", ips);
            ImGui::Text("IPST: %u", t);
            ImGui::InputDouble("Target IPS", &target_frequency, 0.1, 1, "%.1f");
            if (target_frequency < 20) {
                target_frequency = 20.0;
            } else if (target_frequency >= 2500) {
                target_frequency = 2500.0;
            }
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        win->update();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 48, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

        win->swapBuffers();
        end = std::chrono::high_resolution_clock::now();
        dt = end-start;

    }

    if (emu_thread.joinable()) {
        emu_thread.join();
    }
    if (videoOut.joinable())
        videoOut.join();


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete win;
    delete chp;
    delete[] img;

    return 0;
}
