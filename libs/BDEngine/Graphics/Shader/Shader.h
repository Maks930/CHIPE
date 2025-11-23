//
// Created by Maks930 on 08/25/2025.
//

#ifndef BADENGINE_SHADER_H
#define BADENGINE_SHADER_H

#include <cstdint>
#include <string>

namespace bde::graphics {
    class Shader;
} // bde::graphics

class bde::graphics::Shader {
private:
    uint32_t m_id;

public:
    explicit Shader(const uint32_t& id);
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    ~Shader();

    void use() const;

    static Shader* load(const std::string& vert, const std::string& frag);
    static Shader* load_t(const std::string& vert_data, const std::string& frag_data);
};

#endif //BADENGINE_SHADER_H