#ifndef CHIPE_APPLICATION_H
#define CHIPE_APPLICATION_H

#include <list>
#include <memory>
#include <thread>
#include <utils/utils.h>
#include <Window/Window.h>
#include <Types.h>
#include <set>
#include <array>
#include <cstdlib>


class chip8;

class Application {
private:
    std::list<std::string> m_argv;
    std::shared_ptr<chip8> m_chip8;
    std::shared_ptr<bde::system::Window> m_window;

    double target_ips{750};
    std::array<u32, 64*48> m_drawSpace;
    settings m_settings;

    u32 renderTextureId{0};

    std::thread m_inputThread;
    std::thread m_drawThread;
    std::thread m_emuThread;

    std::atomic_bool m_running{false};

    std::atomic_bool m_runInput{ false };
    std::atomic_bool m_runDraw{ false };
    std::atomic_bool m_runEmu{ false };

    std::atomic_bool m_pausedInput{ false };
    std::atomic_bool m_pausedDraw{ false };
    std::atomic_bool m_pausedEmu{ false };


    bool m_step{false};
    std::set<u16> m_breakPoints;
    u32 inst_c{0};
    u32 m_ips{0};

    ProgInfo m_progInfo{
        .title = "NO PROGRAM",
        .path = "NO_PATH",
        .size=0,
        .insts=0
    };

    std::vector<u8> m_currentProgramm;
    
    std::array<u32, 16> m_keyBind;

    using clock = std::chrono::steady_clock;
    using duration = std::chrono::duration<double, std::milli>;

    duration deltaTime;

public:


    explicit Application(int argc, char** argv);
    ~Application();

    [[nodiscard]] int exec();

private:
    void _initWindow();
    void _initGui();
    void _initEmulator();

    void _terminateGui();

    void _drawGui();

    void Input();
    void DrawEmu();
    void Emulate();

    void playSound();

    void loadProgramm(fs::path program_path);
};


#endif //CHIPE_APPLICATION_H
