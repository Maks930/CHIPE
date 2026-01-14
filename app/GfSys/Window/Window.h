#ifndef BADENGINE_WINDOW_H
#define BADENGINE_WINDOW_H

#include <bitset>
#include <string>
#include <cstdint>
#include <functional>
#include <memory>
#include <queue>
#include <optional>
#include <memory>

#ifndef GL_COLOR_BUFFER_BIT
#define GL_COLOR_BUFFER_BIT 0x00004000
#endif //GL_COLOR_BUFFER_BIT

namespace bde::system {
    class Event;
}

class GLFWwindow;
class GLFWmonitor;

namespace bde::system {
    class Window;
} // bde::system

class bde::system::Window {
public:
    using WinSize = std::pair<int32_t, int32_t>;

    explicit Window(const std::string &title, int32_t width, int32_t height, const bool& fullscreen = false);

    explicit Window(const std::string &title, const WinSize &size, const bool& fullscreen = false);
    explicit Window(const std::string &title, const WinSize &size,
            const WinSize &max_size, const bool& fullscreen = false);

    explicit Window(const std::string &title, const WinSize &size,
            const WinSize &max_size, const WinSize &min_size, const bool& fullscreen = false);

    explicit Window(const std::string &title, const WinSize& size,
            const std::pair<WinSize, WinSize> &size_limits, const bool& fullscreen = false);

    ~Window();

    void swapBuffers() const;
    void clear(const float& r = 0, const float& g = 0, const float& b = 0, const float& a = 1) const;
    void close() const;

    void setSize(int32_t width, int32_t height);

    void setClearMask(const uint32_t& mask);

    void setMaxSize(const int32_t& width, const int32_t& height);
    void setMinSize(const int32_t& width, const int32_t& height);
    void setMaxSize(const WinSize& size);
    void setMinSize(const WinSize& size);

    void setUpdateFunc(const std::function<void()> &func);


    void setTitle(const std::string& title) const;

    void setCursorMode(const uint32_t& mode) const;

    [[nodiscard]] bool isOpen() const;
    [[nodiscard]] WinSize getSize() const;

    void update() const;

    std::optional<Event> pollEvent();

    //TODO: delete me
    [[nodiscard]] GLFWwindow* getHandle() const {return m_pWindow;};

    void processAllEvents();

private:
    GLFWwindow* m_pWindow;
    std::string m_title;
    WinSize m_size;
    WinSize m_maxSize;
    WinSize m_minSize;
    bool m_bFullscreen;
    uint32_t m_clearMask{GL_COLOR_BUFFER_BIT};

    std::bitset<1032>               m_keyDown;
    std::unique_ptr<uint32_t[]>     m_pFrames;
    uint32_t                        m_frame{};

    std::queue<Event> m_pendingEvents;

    std::function<void()> m_updateFn = nullptr;

    std::mutex eventMutex;

    void _update() const;
    void precessEvents();

    void processInput();

    void pushEvent(const Event &event);

    friend class bde::system::Event;
};

#endif //BADENGINE_WINDOW_H