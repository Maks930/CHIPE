#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <iomanip>
#include <algorithm>
#include <sstream>

using u8 = uint8_t;
using u16 = uint16_t;

// Основные опкоды (без учёта вариаций 8xy*)
const std::unordered_map<std::string, u16> OPCODES = {
    {"cls",   0x00E0},
    {"ret",   0x00EE},
    {"sys",   0x0000},  // 0nnn
    {"jp",    0x1000},  // 1nnn
    {"call",  0x2000},
    {"se",    0x3000},  // 3xkk
    {"sne",   0x4000},
    {"se",    0x5000},  // 5xy0
    {"ld",    0x6000},  // 6xkk   (также mov)
    {"add",   0x7000},
    {"mov",   0x8000},  // будет различаться по последней цифре
    {"or",    0x8001},
    {"and",   0x8002},
    {"xor",   0x8003},
    {"add",   0x8004},  // с флагом переноса
    {"sub",   0x8005},
    {"shr",   0x8006},
    {"subn",  0x8007},
    {"shl",   0x800E},
    {"sne",   0x9000},  // 9xy0
    {"ldi",   0xA000},  // ld I, addr
    {"jp",    0xB000},  // Bnnn  V0 + addr
    {"rnd",   0xC000},
    {"drw",   0xD000},
    {"skp",   0xE09E},
    {"skpn",  0xE0A1},
    {"ld",    0xF000}   // будет много вариантов Fx..
};

const std::unordered_map<std::string, u16> F_TIMERS_AND_SPECIAL = {
    {"ld",  0xF007},  // Vx = DT
    {"ld",  0xF00A},  // Vx = key (wait)
    {"ld",  0xF015},  // DT = Vx
    {"ld",  0xF018},  // ST = Vx
    {"add", 0xF01E},  // I += Vx
    {"ld",  0xF029},  // I = sprite Vx
    {"ld",  0xF033},  // BCD Vx → [I],[I+1],[I+2]
    {"ld",  0xF055},  // [I] ← V0..Vx
    {"ld",  0xF065},  // V0..Vx ← [I]
};

struct Instruction {
    std::string mnemonic;
    std::string arg1, arg2, arg3;
    int line = 0;
};

bool is_hex_char(char c) {
    return (c >= '0' && c <= '9') ||
        (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F');
}

bool is_valid_hex(const std::string& s, size_t expected_len = 0) {
    if (expected_len > 0 && s.length() != expected_len) return false;
    return std::all_of(s.begin(), s.end(), is_hex_char);
}

u16 parse_hex(const std::string& s) {
    u16 val = 0;
    std::stringstream ss;
    ss << std::hex << s;
    ss >> val;
    return val;
}

bool parse_register(const std::string& s, u8& reg) {
    if (s.length() != 2) return false;
    if (s[0] != 'v' && s[0] != 'V') return false;

    std::string num = s.substr(1);
    if (num.length() == 1 && is_hex_char(num[0])) {
        reg = parse_hex(num);
        return reg <= 0xF;
    }
    return false;
}

bool parse_nibble(const std::string& s, u8& n) {
    if (s.length() != 1) return false;
    if (!is_hex_char(s[0])) return false;
    n = parse_hex(s);
    return true;
}

std::vector<u16> assemble(const std::vector<Instruction>& instructions,
    std::vector<std::string>& errors)
{
    std::vector<u16> program;
    std::unordered_map<std::string, u16> labels;
    size_t pc = 0x200;

    // Первый проход
    for (size_t i = 0; i < instructions.size(); ++i) {
        const auto& ins = instructions[i];

        // Метка?
        if (!ins.mnemonic.empty() && ins.arg1.empty() && ins.arg2.empty()) {
            std::string label = ins.mnemonic;
            std::transform(label.begin(), label.end(), label.begin(), ::tolower);
            if (labels.count(label)) {
                errors.push_back("Line " + std::to_string(ins.line) +
                    ": duplicate label: " + label);
            }
            labels[label] = static_cast<u16>(pc);
            continue;
        }

        if (ins.mnemonic == "db" || ins.mnemonic == "byte") {
            if (!is_valid_hex(ins.arg1, 2)) {
                errors.push_back("Line " + std::to_string(ins.line) +
                    ": invalid byte value: " + ins.arg1);
            }
            pc += 1;
            continue;
        }

        pc += 2;
    }

    // Второй проход — генерация кода
    pc = 0x200;

    for (const auto& ins : instructions) {
        std::string mnem = ins.mnemonic;
        std::transform(mnem.begin(), mnem.end(), mnem.begin(), ::tolower);

        if (mnem.empty()) continue; // пустая строка / только метка

        // Метка — пропускаем, уже обработали
        if (ins.arg1.empty() && ins.arg2.empty()) {
            continue;
        }

        u16 opcode = 0;

        if (mnem == "cls") {
            opcode = 0x00E0;
        }
        else if (mnem == "ret") {
            opcode = 0x00EE;
        }
        else if (mnem == "sys" || mnem == "call" || mnem == "jp") {
            u16 base = OPCODES.at(mnem);
            if (ins.arg1[0] == ':') { // метка
                std::string lbl = ins.arg1.substr(1);
                std::transform(lbl.begin(), lbl.end(), lbl.begin(), ::tolower);
                if (!labels.count(lbl)) {
                    errors.push_back("Line " + std::to_string(ins.line) +
                        ": undefined label: " + lbl);
                    continue;
                }
                u16 addr = labels[lbl];
                opcode = base | (addr & 0x0FFF);
            }
            else if (is_valid_hex(ins.arg1, 3)) {
                u16 addr = parse_hex(ins.arg1);
                opcode = base | (addr & 0x0FFF);
            }
            else {
                errors.push_back("Line " + std::to_string(ins.line) +
                    ": bad address format");
            }
        }
        // 3xkk, 4xkk, 6xkk, 7xkk, Cxkk
        else if (mnem == "se" || mnem == "sne" || mnem == "ld" ||
            mnem == "add" || mnem == "rnd")
        {
            u16 base = OPCODES.at(mnem);

            u8 vx;
            if (!parse_register(ins.arg1, vx)) {
                errors.push_back("Line " + std::to_string(ins.line) +
                    ": bad register: " + ins.arg1);
                continue;
            }

            if (is_valid_hex(ins.arg2, 2)) {
                u8 byte = static_cast<u8>(parse_hex(ins.arg2));
                opcode = base | (vx << 8) | byte;
            }
            else {
                errors.push_back("Line " + std::to_string(ins.line) +
                    ": expected 2-digit hex byte");
            }
        }
        // 5xy0 и 9xy0
        else if ((mnem == "se" || mnem == "sne") && ins.arg3.empty()) {
            u16 base = (mnem == "se") ? 0x5000 : 0x9000;

            u8 vx, vy;
            if (!parse_register(ins.arg1, vx) || !parse_register(ins.arg2, vy)) {
                errors.push_back("Line " + std::to_string(ins.line) +
                    ": expected two registers");
                continue;
            }
            opcode = base | (vx << 8) | (vy << 4);
        }
        // 8xyN группа
        else if (OPCODES.count(mnem) && mnem != "ld" && mnem != "add") {
            u16 base = OPCODES.at(mnem);

            u8 vx, vy;
            if (!parse_register(ins.arg1, vx) || !parse_register(ins.arg2, vy)) {
                errors.push_back("Line " + std::to_string(ins.line) +
                    ": expected two registers");
                continue;
            }
            opcode = base | (vx << 8) | (vy << 4);
        }
        // DxyN — draw
        else if (mnem == "drw") {
            u8 vx, vy, n;
            if (!parse_register(ins.arg1, vx) ||
                !parse_register(ins.arg2, vy) ||
                !parse_nibble(ins.arg3, n))
            {
                errors.push_back("Line " + std::to_string(ins.line) +
                    ": drw Vx, Vy, nibble");
                continue;
            }
            opcode = 0xD000 | (vx << 8) | (vy << 4) | n;
        }
        // Ex9E / ExA1
        else if (mnem == "skp" || mnem == "skpn") {
            u16 base = (mnem == "skp") ? 0xE09E : 0xE0A1;
            u8 vx;
            if (!parse_register(ins.arg1, vx)) {
                errors.push_back("Line " + std::to_string(ins.line) +
                    ": skp/skpn needs register");
                continue;
            }
            opcode = base | (vx << 8);
        }
        // Fx.. группа
        else if (mnem == "ld" || mnem == "add") {
            if (ins.arg1 == "i" || ins.arg1 == "I") {
                // LD I, addr
                if (is_valid_hex(ins.arg2, 3)) {
                    u16 addr = parse_hex(ins.arg2);
                    opcode = 0xA000 | (addr & 0x0FFF);
                }
                else if (ins.arg2[0] == ':') {
                    std::string lbl = ins.arg2.substr(1);
                    std::transform(lbl.begin(), lbl.end(), lbl.begin(), ::tolower);
                    if (labels.count(lbl)) {
                        opcode = 0xA000 | (labels[lbl] & 0x0FFF);
                    }
                    else {
                        errors.push_back("Undefined label: " + lbl);
                    }
                }
                else {
                    errors.push_back("Bad I address");
                }
            }
            else if (ins.arg2 == "dt" || ins.arg2 == "DT") {
                u8 vx;
                if (!parse_register(ins.arg1, vx)) goto bad_fx;
                opcode = 0xF007 | (vx << 8);
            }
            else if (ins.arg2 == "k" || ins.arg2 == "K") {
                u8 vx;
                if (!parse_register(ins.arg1, vx)) goto bad_fx;
                opcode = 0xF00A | (vx << 8);
            }
            else if (ins.arg1 == "dt" || ins.arg1 == "DT") {
                u8 vx;
                if (!parse_register(ins.arg2, vx)) goto bad_fx;
                opcode = 0xF015 | (vx << 8);
            }
            else if (ins.arg1 == "st" || ins.arg1 == "ST") {
                u8 vx;
                if (!parse_register(ins.arg2, vx)) goto bad_fx;
                opcode = 0xF018 | (vx << 8);
            }
            else if (ins.arg1 == "f" || ins.arg1 == "F") {
                u8 vx;
                if (!parse_register(ins.arg2, vx)) goto bad_fx;
                opcode = 0xF029 | (vx << 8);
            }
            else if (ins.arg1 == "b" || ins.arg1 == "B") {
                u8 vx;
                if (!parse_register(ins.arg2, vx)) goto bad_fx;
                opcode = 0xF033 | (vx << 8);
            }
            else if (ins.arg2 == "[i]" || ins.arg2 == "[I]") {
                u8 vx;
                if (!parse_register(ins.arg1, vx)) goto bad_fx;
                opcode = 0xF055 | (vx << 8);
            }
            else if (ins.arg1 == "[i]" || ins.arg1 == "[I]") {
                u8 vx;
                if (!parse_register(ins.arg2, vx)) goto bad_fx;
                opcode = 0xF065 | (vx << 8);
            }
            else {
                goto bad_fx;
            }
        }
        else if (mnem == "add" && ins.arg1 == "i") {
            u8 vx;
            if (!parse_register(ins.arg2, vx)) goto bad_fx;
            opcode = 0xF01E | (vx << 8);
        }
        else if (mnem == "db" || mnem == "byte") {
            if (is_valid_hex(ins.arg1, 2)) {
                u8 val = static_cast<u8>(parse_hex(ins.arg1));
                program.push_back(val << 8); // как big-endian слово
                pc += 1;
                continue;
            }
        }
        else {
            errors.push_back("Line " + std::to_string(ins.line) +
                ": unknown instruction: " + mnem);
            continue;
        }

        program.push_back(opcode);
        pc += 2;
        continue;

    bad_fx:
        errors.push_back("Line " + std::to_string(ins.line) +
            ": bad Fx.. instruction format");
    }

    return program;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: cas input.asm output.ch8\n";
        return 1;
    }

    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "Cannot open input file\n";
        return 2;
    }

    std::vector<Instruction> instructions;
    std::string line;
    int line_num = 0;

    while (std::getline(in, line)) {
        ++line_num;

        // Убираем комментарии
        size_t comment_pos = line.find(';');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }

        // trim
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty()) continue;

        Instruction ins;
        ins.line = line_num;

        std::stringstream ss(line);
        std::string token;

        // Первое слово — либо метка, либо мнемоника
        ss >> token;

        // Метка?
        if (token.back() == ':') {
            ins.mnemonic = token.substr(0, token.size() - 1);
            if (!(ss >> token)) {
                // только метка на строке
                instructions.push_back(ins);
                continue;
            }
        }

        ins.mnemonic = token;

        // Аргументы
        std::vector<std::string> args;
        while (ss >> token) {
            // убираем запятые
            token.erase(std::remove(token.begin(), token.end(), ','), token.end());
            if (!token.empty()) {
                args.push_back(token);
            }
        }

        if (args.size() >= 1) ins.arg1 = args[0];
        if (args.size() >= 2) ins.arg2 = args[1];
        if (args.size() >= 3) ins.arg3 = args[2];

        instructions.push_back(ins);
    }

    std::vector<std::string> errors;
    auto program = assemble(instructions, errors);

    if (!errors.empty()) {
        std::cerr << "=== Compilation errors ===\n";
        for (const auto& err : errors) {
            std::cerr << err << "\n";
        }
        return 3;
    }

    std::ofstream out(argv[2], std::ios::binary);
    if (!out) {
        std::cerr << "Cannot create output file\n";
        return 4;
    }

    //big-endian
    for (u16 word : program) {
        u8 high = word >> 8;
        u8 low = word & 0xFF;
        out.put(static_cast<char>(high));
        out.put(static_cast<char>(low));
    }

    std::cout << "Successfully compiled " << program.size() * 2
        << " bytes → " << argv[2] << "\n";

    return 0;
}