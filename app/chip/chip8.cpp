#include "chip8.h"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <random>
#include <string>
#include <fmt/base.h>
#include <fmt/format.h>

#define SW 64
#define SH 48

#define NEXT {PC+=2;}
#define SKIP {NEXT; PC+=2;}



chip8::chip8() {
    //std::memset(memory, 0, 4096);
    std::fill(memory, memory + 4096, 0);

    //std::memcpy(memory+0x50, m_chip8_fontset, 80);
    std::copy(m_chip8_fontset, m_chip8_fontset+80, memory+0x50);

    //std::memset(V, 0, 16);
    std::fill(V, V + 16, 0);

    //std::memset(videMemory, 0, SW*SH);
    std::fill(videMemory, videMemory + (SW * SH), 0);

    //std::memset(key_layout, 0, 16);
    std::fill(key_layout, key_layout + 16, 0);

    dT = 0;
    sD = 0;
    I=0;
    PC=0x200;
}

chip8::~chip8()
{
}

// std::pair<u16, u16> chip8::emulateCycle() {
//     u16 opcode = (memory[PC] << 8) | memory[PC + 1];
//
//     std::pair<u16, u16> ret_val;
//     ret_val.first = opcode;
//     ret_val.second = PC;
//
//     // fmt::format("{:#X}:", PC);
//     switch (opcode & 0xF000) {
//         case 0x0000:
//             switch (opcode & 0x0FFF) {
//                 case 0x00E0:
//                     NEXT; return ret_val;
//                 case 0x00EE: // PC to top of the stack
//                     PC = stack.top();
//                     stack.pop();
//                     NEXT; return ret_val;
//                 default:
//                     NEXT; return ret_val;
//             }
//             NEXT; return ret_val;
//
//         case 0x1000: // 1NNN jump to addr NNN
//             PC = opcode & 0x0FFF;
//             return ret_val;
//
//         case 0x2000: // 2NNN call to addr NNN
//             stack.push(PC);
//             PC = opcode & 0x0FFF; // TODO: Maybe
//             return ret_val;
//
//         case 0x3000: //3xkk se vx, byte
//         {
//             u8 vx = V[(opcode & 0x0F00) >> 8];
//             u16 kk = opcode & 0x00FF;
//             if (vx == kk) {NEXT;}
//         }
//             NEXT; return ret_val;
//
//         case 0x4000: //4xkk sne vx, byte
//         {
//             u8 vx = V[(opcode & 0x0F00) >> 8];
//             u16 kk = opcode & 0x00FF;
//             if (vx != kk) {NEXT;}
//         }
//             NEXT; return ret_val;
//
//         case 0x5000: //5xy0 se vx, vy
//         {
//             u8 vx = V[(opcode & 0x0F00) >> 8];
//             u8 vy = V[(opcode & 0x00F0) >> 4];
//             if (vx == vy) {NEXT;}
//         }
//             NEXT; return ret_val;
//
//         case 0x6000: // 6xkk LD Vx, byte
//             V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
//             NEXT; return ret_val;
//
//         case 0x7000: //7xkk add vx, byte
//             V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
//             NEXT; return ret_val;
//
//         case 0x8000: // 8-page;
//             switch (opcode & 0x000F) {
//                 case 0x0000: // 8xy0 ld Vx, vy
//                     V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
//                     NEXT; return ret_val;
//                 case 0x0001: // 8xy1 or vx,vy
//                     V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
//                     NEXT; return ret_val;
//                 case 0x0002: // 8xy2 and vx,vy
//                     V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
//                     NEXT; return ret_val;
//                 case 0x0003: // 8xy3 xor vx,vy
//                     V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
//                     NEXT; return ret_val;
//                 case 0x0004: // 8xy4 add vx,vy
//                     V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4];
//                     V[0xF] = (V[(opcode & 0x0F00) >> 8] > 255) ? 1 : 0;
//                     NEXT; return ret_val;
//                 case 0x0005: // 8xy5 sub vx,vy
//                 {
//                     auto x = V[(opcode & 0x0F00) >> 8];
//                     auto y = V[(opcode & 0x00F0) >> 4];
//                     V[0xF] = x > y;
//                     V[(opcode & 0x0F00) >> 8] = x - y;
//                 }
//                     NEXT; return ret_val;
//                 case 0x0006: // 8xy6 SHR Vx {, Vy}
//                 {
//                     auto x = V[(opcode & 0x0F00) >> 8];
//                     V[0xF] = x&1;
//                     V[(opcode & 0x0F00) >> 8] /= 2;
//                 }
//                     NEXT; return ret_val;
//                 case 0x0007: // 8xy7 subn vx,vy
//                 {
//                     auto x = V[(opcode & 0x0F00) >> 8];
//                     auto y = V[(opcode & 0x00F0) >> 4];
//                     V[0xF] = y > x;
//                     V[(opcode & 0x0F00) >> 8] = y-x;
//                 }
//                     NEXT; return ret_val;
//                 case 0x000E: // 8xyE SHL Vx {, Vy}
//                 {
//                     auto x = V[(opcode & 0x0F00) >> 8];
//                     V[0xF] = x&1;
//                     V[(opcode & 0x0F00) >> 8] *= 2;
//                 }
//                     NEXT; return ret_val;
//                 default:
//                     NEXT; return ret_val;
//             }
//             NEXT; return ret_val;
//
//         case 0x9000: //SNE vx,vy
//         {
//             if (V[(opcode & 0x0F00)>>8] != V[(opcode & 0x00F0)>>4]) {NEXT;}
//         }
//             NEXT; return ret_val;
//
//         case 0xA000: // Annn LD i, addr
//             I = opcode & 0x0FFF;
//             NEXT; return ret_val;
//
//         case 0xB000: // Bnnn JP V0, addr
//             PC = V[0] + (opcode & 0x0FFF); //TODO: Maybe delete
//             NEXT; return ret_val;
//
//         case 0xC000: // Cxkk RND Vx, byte
//             std::srand(std::time(nullptr));
//             V[(opcode & 0x0F00) >> 8] = (std::rand() % 256) & (opcode & 0x00FF);
//             NEXT; return ret_val;
//
//             //TODO: FUCK
//         case 0xD000: // Dxyn DRW Vx, Vy, nibble
//         {
//             uint8_t x = V[(opcode & 0x0F00) >> 8] % SW;
//             uint8_t y = V[(opcode & 0x00F0) >> 4] % SH;
//             uint8_t h = opcode & 0x000F;
//             V[0xF] = 0;
//
//             for (u16 row = 0; row < h; ++row) {
//                 u8 spriteByte = memory[I + row];
//                 for (u16 col = 0; col < 8; ++col) {
//                     u8 spritePixel = spriteByte & (0x80 >> col);
//                     u8* screenPixel = &videMemory[(y + row) * SW + (x + col)];
//
//                     if (spritePixel)
//                     {
//                         // Screen pixel also on - collision
//                         if (*screenPixel == 0xFFFFFFFF)
//                         {
//                             V[0xF] = 1;
//                         }
//
//                         // Effectively XOR with the sprite pixel
//                         *screenPixel ^= 0xFFFFFFFF;
//                     }
//                 }
//             }
//         }
//             NEXT; return ret_val;
//
//         case 0xE000: // E-page
//             switch (opcode & 0x00FF) {
//                 case 0x009E: // Ex9E SKP Vx
//                 {
//                     if (key_layout[V[(opcode & 0x0F00) >> 8]] == 1) {NEXT;}
//                 }
//                     NEXT; return ret_val;
//                 case 0x00A1: // ExA1 SKPN Vx
//                 {
//                     if (key_layout[V[(opcode & 0x0F00) >> 8]] != 1) {NEXT;}
//                 }
//                     NEXT; return ret_val;
//                 default:
//                     NEXT; return ret_val;
//             }
//
//         case 0xF000: //F-page
//             switch (opcode & 0x00FF) {
//                 case 0x0007: // Fx07 LD Vx, DT
//                     V[(opcode & 0x0F00) >> 8] = dT;
//                     NEXT; return ret_val;
//                 case 0x000A: // Fx0A LD Vx, K
//                 {
//                     {
//                         uint8_t Vx = (opcode & 0x0F00u) >> 8u;
//
//                         if (key_layout[0])
//                         {
//                             V[Vx] = 0;
//                         }
//                         else if (key_layout[1])
//                         {
//                             V[Vx] = 1;
//                         }
//                         else if (key_layout[2])
//                         {
//                             V[Vx] = 2;
//                         }
//                         else if (key_layout[3])
//                         {
//                             V[Vx] = 3;
//                         }
//                         else if (key_layout[4])
//                         {
//                             V[Vx] = 4;
//                         }
//                         else if (key_layout[5])
//                         {
//                             V[Vx] = 5;
//                         }
//                         else if (key_layout[6])
//                         {
//                             V[Vx] = 6;
//                         }
//                         else if (key_layout[7])
//                         {
//                             V[Vx] = 7;
//                         }
//                         else if (key_layout[8])
//                         {
//                             V[Vx] = 8;
//                         }
//                         else if (key_layout[9])
//                         {
//                             V[Vx] = 9;
//                         }
//                         else if (key_layout[10])
//                         {
//                             V[Vx] = 10;
//                         }
//                         else if (key_layout[11])
//                         {
//                             V[Vx] = 11;
//                         }
//                         else if (key_layout[12])
//                         {
//                             V[Vx] = 12;
//                         }
//                         else if (key_layout[13])
//                         {
//                             V[Vx] = 13;
//                         }
//                         else if (key_layout[14])
//                         {
//                             V[Vx] = 14;
//                         }
//                         else if (key_layout[15])
//                         {
//                             V[Vx] = 15;
//                         }
//                         else
//                         {
//                             PC -= 2;
//                         }
//                     }
//                     NEXT; return ret_val;
//                 }
//                 case 0x0015: // Fx15 LD DT, Vx
//                     NEXT; return ret_val;
//                 case 0x0018: // Fx18 LD ST, Vx
//                     NEXT; return ret_val;
//                 case 0x001E: // Fx1E ADD I, Vx
//                     I += V[(opcode & 0x0F00) >> 8];
//                     NEXT; return ret_val;
//                 case 0x0029: // Fx29 LD F, Vx
//                     //TODO: WARNING
//                     I = 0x50 + (V[(opcode & 0x0F00) >> 8]-1) * 5;
//                     NEXT; return ret_val;
//                 case 0x0033: // Fx33 LD B, Vx
//                 {
//                     uint8_t value = V[ (opcode & 0x0F00) >> 8];
//                     memory[I+2] = value % 10;
//                     value /= 10;
//                     memory[I+1] = value % 10;
//                     value /= 10;
//                     memory[I] = value % 10;
//                 }
//                     NEXT; return ret_val;
//             case 0x0055: // Fx55 LD [I], Vx
//                     for (int i = 0; i <= ((opcode&0x0F00) >> 8) ; i++) {
//                         memory[I + i] = V[i];
//                     }
//                     NEXT; return ret_val;
//                 case 0x0065: // Fx65 LD Vx, [I]
//                     for (int i = 0; i <= ((opcode&0x0F00) >> 8) ; i++) {
//                         V[i] = memory[I + i];
//                     }
//                     NEXT; return ret_val;
//             }
//             NEXT; return ret_val;
//         default:
//             NEXT; return ret_val;
//     }
// }


std::pair<u16, u16> chip8::emulateCycle() {
    std::lock_guard<std::mutex> lock(mutex);
    u16 opcode = (memory[PC] << 8) | memory[PC + 1];

    std::pair<u16, u16> ret_val;
    ret_val.first = opcode;
    ret_val.second = PC;



    // fmt::format("{:#X}:", PC);
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x0FFF) {
            case 0x00E0:
                    //std::memset(videMemory, 0, SW*SH);
                    std::fill(videMemory, videMemory + (SW * SH), 0);
                    break;
                case 0x00EE: // PC to top of the stack
                    PC = stack.top();
                    stack.pop();
                    break;
                default:
                    break;
            }
            break;

        case 0x1000: // 1NNN jump to addr NNN
            PC = opcode & 0x0FFF;
            PC-=2;
            break;

        case 0x2000: // 2NNN call to addr NNN
            stack.push(PC);
            PC = opcode & 0x0FFF; // TODO: Maybe
            PC-=2;
            break;

        case 0x3000: //3xkk se vx, byte
        {
            u8 vx = V[(opcode & 0x0F00) >> 8];
            u16 kk = opcode & 0x00FF;
            if (vx == kk) {NEXT;}
        }
            break;

        case 0x4000: //4xkk sne vx, byte
        {
            u8 vx = V[(opcode & 0x0F00) >> 8];
            u16 kk = opcode & 0x00FF;
            if (vx != kk) {NEXT;}
        }
            break;

        case 0x5000: //5xy0 se vx, vy
        {
            u8 vx = V[(opcode & 0x0F00) >> 8];
            u8 vy = V[(opcode & 0x00F0) >> 4];
            if (vx == vy) {NEXT;}
        }
            break;

        case 0x6000: // 6xkk LD Vx, byte
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            break;

        case 0x7000: //7xkk add vx, byte
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            break;

        case 0x8000: // 8-page;
            switch (opcode & 0x000F) {
                case 0x0000: // 8xy0 ld Vx, vy
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0001: // 8xy1 or vx,vy
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0002: // 8xy2 and vx,vy
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0003: // 8xy3 xor vx,vy
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0004: // 8xy4 add vx,vy
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4];
                    V[0xF] = (V[(opcode & 0x0F00) >> 8] > 255) ? 1 : 0;
                    break;
                case 0x0005: // 8xy5 sub vx,vy
                {
                    auto x = V[(opcode & 0x0F00) >> 8];
                    auto y = V[(opcode & 0x00F0) >> 4];
                    V[0xF] = (x > y)?1:0;
                    V[(opcode & 0x0F00) >> 8] -= y;
                }
                    break;
                case 0x0006: // 8xy6 SHR Vx {, Vy}
                {
                    auto x = V[(opcode & 0x0F00) >> 8];
                    V[0xF] = x&1;
                    V[(opcode & 0x0F00) >> 8] /= 2;
                }
                    break;
                case 0x0007: // 8xy7 subn vx,vy
                {
                    auto x = V[(opcode & 0x0F00) >> 8];
                    auto y = V[(opcode & 0x00F0) >> 4];
                    V[0xF] = y > x;
                    V[(opcode & 0x0F00) >> 8] = y-x;
                }
                    break;
                case 0x000E: // 8xyE SHL Vx {, Vy}
                {
                    auto x = V[(opcode & 0x0F00) >> 8];
                    V[0xF] = x&1;
                    V[(opcode & 0x0F00) >> 8] *= 2;
                }
                    break;
                default:
                    break;
            }
            break;

        case 0x9000: //SNE vx,vy
        {
            if (V[(opcode & 0x0F00)>>8] != V[(opcode & 0x00F0)>>4]) {NEXT;}
        }
            break;

        case 0xA000: // Annn LD i, addr
            I = opcode & 0x0FFF;
            break;

        case 0xB000: // Bnnn JP V0, addr
            PC = V[0] + (opcode & 0x0FFF); //TODO: Maybe delete
            break;

        case 0xC000: // Cxkk RND Vx, byte
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<u8> rnd(0, 255);

            V[(opcode & 0x0F00) >> 8] = rnd(gen) & (opcode & 0x00FF);
        }
            break;

        case 0xD000: // Dxyn DRW Vx, Vy, nibble
            {

                uint8_t x = V[(opcode & 0x0F00) >> 8] % SW;
                uint8_t y = V[(opcode & 0x00F0) >> 4] % SH;
                uint8_t h = opcode & 0x000F;
                V[0xF] = 0;

                for (u16 row = 0; row < h; ++row) {
                    u8 spriteByte = memory[I + row];
                    for (u16 col = 0; col < 8; ++col) {
                        u8 spritePixel = spriteByte & (0x80 >> col);
                        if (spritePixel >= 0 && spritePixel < SW * SH) {
                            u8* screenPixel = &videMemory[(y + row) * SW + (x + col)];
                            if (spritePixel)
                            {
                                // Screen pixel also on - collision
                                if (*screenPixel == 0xFF)
                                {
                                    V[0xF] = 1;
                                }
                                // Effectively XOR with the sprite pixel
                                *screenPixel ^= 0xFF;
                            }
                        }
                    }
                }
            }
            break;

        case 0xE000: // E-page
            switch (opcode & 0x00FF) {
                case 0x009E: // Ex9E SKP Vx
                {
                    if (key_layout[V[(opcode & 0x0F00) >> 8]]) {NEXT;}
                }
                    break;
                case 0x00A1: // ExA1 SKPN Vx
                {
                    if (!key_layout[V[(opcode & 0x0F00) >> 8]]) {NEXT;}
                }
                    break;
                default:
                    break;
            }

        case 0xF000: //F-page
            switch (opcode & 0x00FF) {
                case 0x0007: // Fx07 LD Vx, DT
                    V[(opcode & 0x0F00) >> 8] = dT;
                    break;
                case 0x000A: // Fx0A LD Vx, K
                {

                    {
                        uint8_t Vx = (opcode & 0x0F00u) >> 8u;

                        if (key_layout[0])
                        {
                            V[Vx] = 0;
                        }
                        else if (key_layout[1])
                        {
                            V[Vx] = 1;
                        }
                        else if (key_layout[2])
                        {
                            V[Vx] = 2;
                        }
                        else if (key_layout[3])
                        {
                            V[Vx] = 3;
                        }
                        else if (key_layout[4])
                        {
                            V[Vx] = 4;
                        }
                        else if (key_layout[5])
                        {
                            V[Vx] = 5;
                        }
                        else if (key_layout[6])
                        {
                            V[Vx] = 6;
                        }
                        else if (key_layout[7])
                        {
                            V[Vx] = 7;
                        }
                        else if (key_layout[8])
                        {
                            V[Vx] = 8;
                        }
                        else if (key_layout[9])
                        {
                            V[Vx] = 9;
                        }
                        else if (key_layout[10])
                        {
                            V[Vx] = 10;
                        }
                        else if (key_layout[11])
                        {
                            V[Vx] = 11;
                        }
                        else if (key_layout[12])
                        {
                            V[Vx] = 12;
                        }
                        else if (key_layout[13])
                        {
                            V[Vx] = 13;
                        }
                        else if (key_layout[14])
                        {
                            V[Vx] = 14;
                        }
                        else if (key_layout[15])
                        {
                            V[Vx] = 15;
                        }
                        else
                        {
                            PC -= 2;
                        }

                    }
                    break;
                }
                    break;
                case 0x0015: // Fx15 LD DT, Vx
                    break;
                case 0x0018: // Fx18 LD ST, Vx
                    break;
                case 0x001E: // Fx1E ADD I, Vx
                    I += V[(opcode & 0x0F00) >> 8];
                    break;
                case 0x0029: // Fx29 LD F, Vx
                    //TODO: WARNING
                {
                    u8 digit = V[(opcode & 0x0F00) >> 8] & 0x0F;
                    I = 0x50 + digit * 5;
                }
                    break;
                case 0x0033: // Fx33 LD B, Vx
                {
                    uint8_t value = V[ (opcode & 0x0F00) >> 8];
                    memory[I+2] = value % 10;
                    value /= 10;
                    memory[I+1] = value % 10;
                    value /= 10;
                    memory[I] = value % 10;
                }
                    break;
            case 0x0055: // Fx55 LD [I], Vx
                    for (int i = 0; i <= ((opcode&0x0F00) >> 8) ; i++) {
                        memory[I + i] = V[i];
                    }
                    break;
                case 0x0065: // Fx65 LD Vx, [I]
                    for (int i = 0; i <= ((opcode&0x0F00) >> 8) ; i++) {
                        V[i] = memory[I + i];
                    }
                    break;
            }
            break;
        default:
            break;

    }

    NEXT; return ret_val;
}

void chip8::loadProgram(const u8 *program, const u32 size) {
    //std::memcpy(memory+0x200, program, size);
    std::copy(program, program+size, memory+0x200);
}

void chip8::emulate(const bool f) {
    while (f) {
        emulateCycle();
    }
}

void chip8::reset() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        //std::memset(key_layout, 0, 16);
        std::fill(key_layout, key_layout + 16, 0);
    }

    resetMemory();
    resetStack();
    resetRegisters();
    resetProgramCounter();
}

void chip8::resetStack() {
    //CLEAR STACK
    std::lock_guard<std::mutex> lock(mutex);
    stack = std::stack<u16>();
}

void chip8::resetRegisters() {
    std::lock_guard<std::mutex> lock(mutex);
    //std::memset(V, 0, 16);
    std::fill(V, V + 16, 0);
    I = 0;
    dT = 255;
    sD = 255;
}

void chip8::resetMemory() {
    std::lock_guard<std::mutex> lock(mutex);
    //std::memset(memory, 0, 4096);
    std::fill(memory, memory + 4096, 0);
    std::copy(m_chip8_fontset, m_chip8_fontset + 80, memory + 0x50);
}

void chip8::resetProgramCounter() {
    std::lock_guard<std::mutex> lock(mutex);
    PC= 0x200;
}

void chip8::resetVideoMemory()
{
    std::lock_guard<std::mutex> lock(mutex);
    std::fill(videMemory, videMemory + (SW * SH), 0);
}

void chip8::updateTimers()
{
    std::lock_guard<std::mutex> lock(mutex);
    sD--;
    dT--;
}

std::vector<std::pair<std::pair<u16,u16>, std::string>> chip8::disAsmProg(const u8 *prog, u32 size) {
    std::vector<std::pair<std::pair<u16,u16>, std::string>> ret;
    for (int i = 0; i < size; i+=2) {
        auto opcode = (prog[i]<<8) | prog[i+1];
        std::string dism = "NAH";


        switch (opcode & 0xF000) {
            case 0x0000:
                switch (opcode & 0x0FFF) {
                case 0x00E0:
                        dism="CLS";
                        break;
                case 0x00EE: // PC to top of the stack
                        dism="RET";
                        break;
                default:
                        dism="NOP";
                        break;
                }
                break;

            case 0x1000: // 1NNN jump to addr NNN
                dism=fmt::format("JP 0x{:04X}", opcode & 0x0FFF);
                break;

            case 0x2000: // 2NNN call to addr NNN
                dism=fmt::format("CALL 0x{:04X}", opcode & 0x0FFF);
                break;

            case 0x3000: //3xkk se vx, byte
                dism=fmt::format("SE V[{}], 0x{:04X}", (opcode & 0x0F00) >> 8, opcode & 0x00FF);
                break;

            case 0x4000: //4xkk sne vx, byte
                dism=fmt::format("SNE V[{}], 0x{:04X}", (opcode & 0x0F00) >> 8, opcode & 0x00FF);
                break;

            case 0x5000: //5xy0 se vx, vy
                dism=fmt::format("SE V[{}], V[{}]", (opcode & 0x0F00) >> 8, (opcode & 0x00F0)>>4);
                break;

            case 0x6000: // 6xkk LD Vx, byte
                dism=fmt::format("LD V[{}], 0x{:04X}", (opcode & 0x0F00) >> 8, opcode & 0x00FF);
                break;

            case 0x7000: //7xkk add vx, byte
                dism=fmt::format("ADD V[{}], 0x{:04X}", (opcode & 0x0F00) >> 8, opcode & 0x00FF);
                break;

            case 0x8000: // 8-page;
                switch (opcode & 0x000F) {
                case 0x0000: // 8xy0 ld Vx, vy
                        dism=fmt::format("LD V[{}], V[{}]", (opcode & 0x0F00) >> 8, (opcode & 0x00F0)>>4);
                        break;
                case 0x0001: // 8xy1 or vx,vy
                        dism=fmt::format("OR V[{}], V[{}]", (opcode & 0x0F00) >> 8, (opcode & 0x00F0)>>4);
                        break;
                case 0x0002: // 8xy2 add vx,vy
                        dism=fmt::format("ADD V[{}], V[{}]", (opcode & 0x0F00) >> 8, (opcode & 0x00F0)>>4);
                        break;
                case 0x0003: // 8xy3 xor vx,vy
                        dism=fmt::format("XOR V[{}], V[{}]", (opcode & 0x0F00) >> 8, (opcode & 0x00F0)>>4);
                        break;
                case 0x0004: // 8xy4 add vx,vy
                        dism=fmt::format("ADD V[{}], V[{}]", (opcode & 0x0F00) >> 8, (opcode & 0x00F0)>>4);
                        break;
                case 0x0005: // 8xy5 sub vx,vy
                        dism=fmt::format("SUB V[{}], V[{}]", (opcode & 0x0F00) >> 8, (opcode & 0x00F0)>>4);
                        break;
                case 0x0006: // 8xy6 SHR Vx {, Vy}
                        dism=fmt::format("SHR V[{}]", (opcode & 0x0F00) >> 8);
                        break;
                case 0x0007: // 8xy7 subn vx,vy
                        dism=fmt::format("SUBN V[{}], V[{}]", (opcode & 0x0F00) >> 8, (opcode & 0x00F0)>>4);
                        break;
                case 0x000E: // 8xyE SHL Vx {, Vy}
                        dism=fmt::format("SHL V[{}]", (opcode & 0x0F00) >> 8);
                        break;
                default:
                        dism="UNKNOWN";
                        break;
                }
                break;

            case 0x9000: //SNE vx,vy
                dism=fmt::format("SNE V[{}], V[{}]", (opcode & 0x0F00) >> 8, (opcode & 0x00F0)>>4);
                break;

            case 0xA000: // Annn LD i, addr
                dism=fmt::format("LD I, 0x{:04X}", opcode & 0x0FFF);
                break;

            case 0xB000: // Bnnn JP V0, addr
                dism=fmt::format("JP V[0], 0x{:04X}", opcode & 0x0FFF);
                break;

            case 0xC000: // Cxkk RND Vx, byte
                dism=fmt::format("RND V[{}], 0x{:04X}",(opcode & 0x0F00) >> 8, opcode & 0x00FF);
                break;

            case 0xD000: // Dxyn DRW Vx, Vy, nibble
                dism=fmt::format("DRW V[{}], V[{}], 0x{:04X}",(opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4, opcode & 0x000F);
                break;

            case 0xE000: // E-page
                switch (opcode & 0x00FF) {
                case 0x009E: // Ex9E SKP Vx
                        dism=fmt::format("SKP V[{}]", (opcode & 0x0F00)>>8);
                        break;
                case 0x00A1: // ExA1 SKPN Vx
                        dism=fmt::format("SKPN V[{}]", (opcode & 0x0F00)>>8);
                        break;
                default:
                        dism="UNKNOWN";
                        break;
                }

            case 0xF000: //F-page
                switch (opcode & 0x00FF) {
                case 0x0007: // Fx07 LD Vx, DT
                        dism=fmt::format("LD V[{}], DT", (opcode & 0x0F00)>>8);
                        break;
                case 0x000A: // Fx0A LD Vx, K
                        dism=fmt::format("LD V[{}], K", (opcode & 0x0F00)>>8);
                        break;
                case 0x0015: // Fx15 LD DT, Vx
                        dism=fmt::format("LD DT, V[{}]", (opcode & 0x0F00)>>8);
                        break;
                case 0x0018: // Fx18 LD ST, Vx
                        dism=fmt::format("LD ST, V[{}]", (opcode & 0x0F00)>>8);
                        break;
                case 0x001E: // Fx1E ADD I, Vx
                        dism=fmt::format("ADD I, V[{}]", (opcode & 0x0F00)>>8);
                        break;
                case 0x0029: // Fx29 LD F, Vx
                        dism=fmt::format("LD F, V[{}]", (opcode & 0x0F00)>>8);
                        break;
                case 0x0033: // Fx33 LD B, Vx
                        dism=fmt::format("LD B, V[{}]", (opcode & 0x0F00)>>8);
                        break;
                case 0x0055: // Fx55 LD [I], Vx
                        dism=fmt::format("LD [I], V[{}]", (opcode & 0x0F00)>>8);
                        break;
                case 0x0065: // Fx65 LD Vx, [I]
                        dism=fmt::format("LD V[{}], [I]", (opcode & 0x0F00)>>8);
                        break;
                }
                break;
            default:
                dism="UNKNOWN";
                break;
        }

        ret.emplace_back(std::pair<u16,u16>{0x200+i, opcode}, dism);

    }
    return ret;
}