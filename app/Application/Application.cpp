#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstring>
#include <Application/Application.h>
#include <Event/Event.h>
#include <fmt/base.h>

#include <imgui.h>
#include <random>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include <Application/GuiS/Guis.h>

#include <chip/chip8.h>
#include <Application/AudioEngine/AEngine.h>



//const u8 prog[] = {0x00,0x00, 0x61,0x00, 0x71,0x01, 0xA2,0x10, 0xD1,0x01, 0x31,0x05, 0x12,0x04, 0x12,0x02, 0xFF,0xFF};
const u8 prog[] = { 0x12, 0x06, 0x82, 0x10, 0x00, 0xEE, 0x61, 0x05, 0x71, 0x01, 0x22, 0x02, 0x12, 0x08};


void Application::_initWindow() {
    m_window = std::make_shared<bde::system::Window>(
        APPLICATION_NAME,
        bde::system::Window::WinSize(960, 720),
        std::pair(
            bde::system::Window::WinSize(640,480),
            bde::system::Window::WinSize(20'000, 20'000)
            )
        );

    SoundEngine& e = SoundEngine::instance();
    if (e.init()) {
        e.generateAndLoadSineWave("chip_tone", 410.0f, 0.1f, 0.3f, 22700);
    }
}

void Application::_initGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch


    ImGui_ImplGlfw_InitForOpenGL(m_window->getHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::fill(m_keyBind.begin(), m_keyBind.end(), 0);

    m_keyBind = {
		//0         //1         //2         //3
        GLFW_KEY_X, GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3,
		//4         //5         //6         //7
        GLFW_KEY_Q, GLFW_KEY_W, GLFW_KEY_E, GLFW_KEY_A,
		//8         //9         //A         //B
        GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_Z, GLFW_KEY_C,
		//C         //D         //E         //F
        GLFW_KEY_4, GLFW_KEY_R, GLFW_KEY_F, GLFW_KEY_V
    };

}

void Application::_initEmulator() {
	m_drawSpace.fill(0);
    {
        auto m_drawSpaceE = reinterpret_cast<u32*>(m_drawSpace.data());        
        std::fill(m_drawSpaceE, m_drawSpaceE + (64 * 48), 0xFF'FF'FF'FF);
        glGenTextures(1, &renderTextureId);
        glBindTexture(GL_TEXTURE_2D, renderTextureId);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 48, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_drawSpace.data());
    }

    m_chip8 = std::make_shared<chip8>();
}

void Application::_terminateGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::_drawGui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();

    Guis::DrawInfoMenu("Emulator Info Menu", m_settings.target_ips, 1000.0/deltaTime.count(), m_ips);
    Guis::RenderSpace(renderTextureId,64,48);
    Guis::DrawSettingsMenu(&m_settings);
    Guis::DrawEmulatorRegistersTable(m_chip8->getV().data());
    Guis::DrawEmulatorPointersTable({
        {"PC", m_chip8->getPC()},
        {"I", m_chip8->getI()},
        {"DT", m_chip8->getDT()},
        {"ST", m_chip8->getST()},
    });

    
    Guis::DisAsmMenu(
        chip8::disAsmProg(
            m_chip8->getMemory().begin()+0x200,
            4096-0x200
        ),
        m_chip8->getPC(),
        m_breakPoints
    );

    Guis::DrawKeyBindMenu(m_keyBind);
    //Guis::DrawKeyTableMenu(m_chip8->getKeyLayout());

    //Guis::DrawBreakPointsMenu(m_breakPoints);

    Guis::DrawProgramLoadMenu(m_progInfo, [this](fs::path path) {
        this->loadProgramm(path);
    });

    Guis::DrawResetMenu([this](bool mem, bool regs, bool video) {
        m_pausedEmu = true;
        m_pausedDraw = true;

        if (mem) {
            m_chip8->resetMemory();
        }

        if (regs) {
            m_chip8->resetRegisters();
            m_chip8->resetStack();
            m_chip8->resetProgramCounter();
        }

        if (video) {
            m_chip8->resetVideoMemory();
        }

        m_pausedEmu = false;
        m_pausedDraw = false;
    });

    Guis::DrawMemoryMap(m_chip8->getMemory());

    bool isRunning = m_running;
    Guis::DrawControlMenu(isRunning, m_step);
    m_running = isRunning;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::Input() {
    while (m_window->isOpen() && m_runInput) {
        if (m_pausedInput) {
            continue;
        }
        while (std::optional e = m_window->pollEvent()) {
            if (e->is<bde::system::Event::Closed>()) {
                m_window->close();
            }

            if (const auto key = e->get_if<bde::system::Event::JustKeyReleased>()) {
                i8 any_pressed = [](u32 code, std::array<u32, 16>& arr) {
                    for (int i = 0; i < arr.size(); i++) {
                        if (arr.at(i) == code) {
                            return i;
                        }
                    }
                    return -1;
                    }(key->code, m_keyBind);

                if (any_pressed != -1) {
                    m_chip8->relKey((u8)any_pressed);
                }
            }
            if (const auto key = e->get_if<bde::system::Event::JustKeyPressed>()) {
                switch (key->code) {
                    case GLFW_KEY_ESCAPE:
                        m_window->close();
                        break;
                    case GLFW_KEY_TAB:
                        m_running = !m_running;
                        break;
                   case GLFW_KEY_F2:
                        m_running = false;
                        m_chip8->reset();
                        m_currentProgramm.clear();
                        break;
                    default:
                        break;
                }

                i8 any_pressed = [](u32 code, std::array<u32, 16>& arr) {
                    for (int i = 0; i < arr.size(); i++) {
                        if (arr.at(i) == code) {
                            return i;
                        }
                    }
                    return -1;
                }(key->code, m_keyBind);

                if (any_pressed != -1) {
                    m_chip8->pressKey((u8)any_pressed);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void Application::DrawEmu() {
    while (m_runDraw) {
        if (m_pausedDraw) {
            continue;
        }
        const auto m_drawSpaceE = reinterpret_cast<u32*>(m_drawSpace.data());

        auto vm = m_chip8->getVideoMemory();
        for (int i = 0; i < 64*48; i++) {
            if (!m_window->isOpen()) {
                return;
            }
            if (vm[i] == 0) {
                m_drawSpaceE[i] = m_settings.background_color;
            } else {
                m_drawSpaceE[i] = m_settings.foreground_color;
            }
        }
    }
}

void Application::Emulate() {
    auto cur_target_frequency = m_settings.target_ips;
    std::chrono::duration<double> target_interval(1.0 / cur_target_frequency);

    std::chrono::duration<double> sound_target_interval(1.0 / 60);
    auto sound_next_time = clock::now() + sound_target_interval;
    
    auto next_time = clock::now() + target_interval;
    
    while (m_window->isOpen() && m_runEmu) {
       
        if (m_pausedEmu) {
            continue;
        }
        if (cur_target_frequency != m_settings.target_ips) {
            cur_target_frequency = m_settings.target_ips;
            target_interval = std::chrono::duration<double>(1.0 / cur_target_frequency);
        }
        
        if (m_breakPoints.contains(m_chip8->getPC())) {
            m_running = false;
        }

        if (m_running) {
            m_chip8->emulateCycle();
            inst_c+=1;
        }

        auto current_time = clock::now();
        if (current_time >= sound_next_time) {
            sound_next_time += sound_target_interval;
            m_chip8->updateTimers();
        }

        if (m_chip8->playeSound()) {
            playSound();
        }

        std::this_thread::sleep_until(next_time);
        next_time += target_interval;

        
    }
}

void Application::playSound()
{
    SoundEngine::instance().playSound("chip_tone", false);
}

void Application::loadProgramm(fs::path program_path)
{
    m_running = false;
    m_pausedDraw = true;
    m_pausedEmu = true;


    std::ifstream file(program_path, std::ios::binary);
    
    file.seekg(0, std::ifstream::end);
    const u32 size = file.tellg();
    file.seekg(0, std::ifstream::beg);

    m_currentProgramm.resize(size);
	file.read(reinterpret_cast<char*>(m_currentProgramm.data()), size);

    file.close();

    m_chip8->loadProgram(m_currentProgramm.data(), m_currentProgramm.size());
    Guis::InitMemDumpMenu();

    m_progInfo.title = program_path.filename().string();
    m_progInfo.path = program_path.string();
    m_progInfo.size = size;
    m_progInfo.insts = size / 2;

    m_pausedDraw = false;
    m_pausedEmu = false;


}

Application::Application(int argc, char **argv) {
    m_argv.assign(argv, argv+argc);

    if (fs::exists("./settings.ces")) {
        if (!loadSettingFrom(m_settings, "./settings.ces")) {
            saveSettingTo(m_settings, "./settings.ces");
        }
    } else {
        saveSettingTo(m_settings, "./settings.ces");
    }

    _initWindow();
    _initGui();
    _initEmulator();

    m_inputThread = std::thread(&Application::Input, this);
    m_drawThread = std::thread(&Application::DrawEmu, this);
    m_emuThread = std::thread(&Application::Emulate, this);

}

Application::~Application() {
    if (m_emuThread.joinable()) {
        m_emuThread.join();
    }
    if (m_inputThread.joinable()) {
        m_inputThread.join();
    }
    if (m_drawThread.joinable()) {
        m_drawThread.join();
    }

    _terminateGui();

    glDeleteTextures(1, &renderTextureId);
}

int Application::exec() {

    m_runInput = true;
    m_runEmu = true;
    m_runDraw = true;
        

    auto start = clock::now();
    auto end = clock::now();
    auto ips_timer = clock::now();
    deltaTime = end-start;

    while (m_window->isOpen()) {

        start = end;
        m_window->processAllEvents();

        m_window->clear(0,0,0);

        _drawGui();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 48, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_drawSpace.data());
        m_window->swapBuffers();

        end = clock::now();
        deltaTime = end-start;

        if (std::chrono::duration_cast<std::chrono::milliseconds>(end-ips_timer).count() >= 1000) {
            m_ips = inst_c;
            inst_c = 0;
            ips_timer = end;
        }

        if (!m_running && m_step) {
            m_chip8->emulateCycle();
            m_step = false;
        }

    }

    m_runInput = false;
    m_runEmu = false;
    m_runDraw = false;

    return 0;
}


