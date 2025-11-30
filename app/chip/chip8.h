#ifndef CHIPE_CHIP8_H
#define CHIPE_CHIP8_H
#include <cstring>
#include <stack>
#include <vector>
#include <bits/basic_string.h>

#include <mutex>
#include <array>

#include <Types.h>



class chip8 {
private:
    u8 memory[4096]{};
    u8 V[16]{};
    u16 I;
    u16 PC{0x200};
    std::stack<u16> stack;
    u8 videMemory[64*48];

    u8 m_delayTimer{ 255 };
    u8 m_soundTimer{ 255 };

    bool key_layout[16];

    std::mutex mutex;


    u8 m_chip8_fontset[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

public:
    explicit chip8();
    ~chip8();

    std::pair<u16, u16> emulateCycle();

    void loadProgram(const u8* program, u32 size);

    void emulate(bool f);

    std::array<u8, 4096> getMemory() {
        std::lock_guard<std::mutex> lock(mutex);
        return std::to_array(memory);
    }
    std::array<u8,16> getV() {
        std::lock_guard<std::mutex> lock(mutex);
        return std::to_array(V);
    }
    [[nodiscard]] u16 getI() const {
        return I;
    }
    [[nodiscard]] u16 getPC() const { return PC; }
    [[nodiscard]] u8 getDT() const { return m_delayTimer; }
    [[nodiscard]] u8 getST() const { return m_soundTimer; }
    [[nodiscard]] std::array<bool,16> getKeyLayout() {
        std::lock_guard<std::mutex> lock(mutex);
        return std::to_array(key_layout);
    }

    std::stack<u16>& getStack() {
        std::lock_guard<std::mutex> lock(mutex);
        return stack;
    }

    [[nodiscard]] std::array<u8, 64*48> getVideoMemory() {
        std::lock_guard<std::mutex> lock(mutex);
        return std::to_array(videMemory);
    }

    void pressKey(const u8& key) {
        std::lock_guard<std::mutex> lock(mutex);
        key_layout[key] = true;
    };
    void relKey(const u8& key) {
        std::lock_guard<std::mutex> lock(mutex);
        key_layout[key] = false;
    };
    void switchKey(const u8& key) {
        std::lock_guard<std::mutex> lock(mutex);
        key_layout[key] = !key_layout[key];
    };

    [[nodiscard]] bool playeSound() {
        return m_soundTimer > 0;
    }

    void reset();
    void resetStack();
    void resetRegisters();
    void resetMemory();
    void resetProgramCounter();
    void resetVideoMemory();
    void updateTimers();

    static std::vector<std::pair<std::pair<u16,u16>, std::string>> disAsmProg(const u8* prog, u32 size);

};


#endif //CHIPE_CHIP8_H