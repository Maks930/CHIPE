//
// Created by Maks930 on 08/21/2025.
//

#include "Window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Event/Event.h>

using namespace bde::system;


Window::Window(const std::string &title, const int32_t width, const int32_t height, const bool& fullscreen) :
    m_title(title), m_size(width, height), m_bFullscreen(fullscreen)
{

    if (!glfwInit()) {
        // LOG_FATAL("Failed to initialize GLFW");
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);

    // WARNING: Maybe need refactor
    glfwWindowHint(GLFW_DECORATED, !m_bFullscreen);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    m_pWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!m_pWindow) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_pWindow);
    glfwSwapInterval(1);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, width, height);

    glfwSetWindowUserPointer(m_pWindow, this);

    m_pFrames = std::make_unique<uint32_t[]>(1032);
	std::fill(m_pFrames.get(), m_pFrames.get() + 1032, 0);


    glfwSetWindowCloseCallback(m_pWindow, [](GLFWwindow* window) {
        const auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        win->pushEvent(Event(Event::Closed()));
    });

    glfwSetKeyCallback(m_pWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        const auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
       if (action == GLFW_PRESS) {
           win->m_keyDown.set(key, true);
           win->m_pFrames[key] = win->m_frame;
       } else if (action == GLFW_RELEASE) {
           win->m_keyDown.set(key, false);
           win->m_pFrames[key] = win->m_frame;
       }
    });

}

Window::Window(const std::string &title, const WinSize &size, const bool& fullscreen) :
    Window(title, size.first, size.second, fullscreen)
{}

Window::Window(const std::string &title, const WinSize &size, const WinSize &max_size, const bool& fullscreen) :
    Window(title, size.first, size.second, fullscreen)
{
    glfwSetWindowSizeLimits(m_pWindow, max_size.first, max_size.second, GLFW_DONT_CARE, GLFW_DONT_CARE);
}

Window::Window(const std::string &title, const WinSize &size, const WinSize &max_size, const WinSize &min_size,
    const bool&  fullscreen) :
    Window(title, size.first, size.second, fullscreen)
{
    glfwSetWindowSizeLimits(m_pWindow, max_size.first, max_size.second, min_size.first, min_size.second);
}

Window::Window(const std::string &title, const WinSize &size, const std::pair<WinSize, WinSize> &size_limits,
    const bool &fullscreen) :
    Window(title, size, fullscreen) {
    glfwSetWindowSizeLimits(m_pWindow, size_limits.first.first, size_limits.first.second,
        size_limits.second.first, size_limits.second.second);
}

Window::~Window() {
    glfwDestroyWindow(m_pWindow);
    glfwTerminate();

}

void Window::swapBuffers() const {
    glfwSwapBuffers(m_pWindow);
}

void Window::clear(const float &r, const float &g, const float &b, const float &a) const {
    glClearColor(r, g, b, a);
    glClear(m_clearMask);
}


bool Window::isOpen() const {
    return !glfwWindowShouldClose(m_pWindow);
}

Window::WinSize Window::getSize() const {
    return m_size;
}

void Window::update() const {
    if (m_updateFn != nullptr) {
        m_updateFn();
    }
}


std::optional<Event> Window::pollEvent() {
    std::lock_guard<std::mutex> lock(eventMutex);
    std::optional<Event> event;
    if (!m_pendingEvents.empty()) {
        event = m_pendingEvents.front();
        m_pendingEvents.pop();
    }
    return event;
}

void Window::processAllEvents() {
    precessEvents();
    processInput();
}

void Window::_update() const {
    glClear(m_clearMask);
    update();
    swapBuffers();
}

void Window::precessEvents() {
    ++m_frame;

    glfwPollEvents();
}

void Window::processInput() {

    for (int i(0); i < 1024; ++i) {
        const bool flag = m_keyDown.test(i);
        if (flag && m_pFrames[i] == m_frame) {
            pushEvent(static_cast<Event>(Event::JustKeyPressed{i}));
        }
        else if (flag) {
            pushEvent(static_cast<Event>(Event::KeyPressed{i}));
        }
    }
}

void Window::pushEvent(const Event &event) {
    std::lock_guard<std::mutex> lock(eventMutex);
    m_pendingEvents.push(event);
}


void Window::close() const {
    glfwSetWindowShouldClose(m_pWindow, true);
}

void Window::setSize(const int32_t width, const int32_t height) {
    m_size.first = width; m_size.second = height;
    glfwSetWindowSize(m_pWindow, width, height);
}

void Window::setClearMask(const uint32_t &mask) {
    m_clearMask = mask;
}

void Window::setMaxSize(const int32_t &width, const int32_t &height) {
    m_maxSize.first = width; m_maxSize.second = height;
    glfwSetWindowSizeLimits(m_pWindow, GLFW_DONT_CARE, GLFW_DONT_CARE, width, height);
}

void Window::setMinSize(const int32_t &width, const int32_t &height) {
    m_minSize.first = width; m_minSize.second = height;
    glfwSetWindowSizeLimits(m_pWindow, width, height, GLFW_DONT_CARE, GLFW_DONT_CARE);
}

void Window::setMaxSize(const WinSize &size) {
    setMaxSize(size.first, size.second);
}

void Window::setMinSize(const WinSize &size) {
    setMinSize(size.first, size.second);
}

void Window::setUpdateFunc(const std::function<void()> &func) {
    m_updateFn = func;
}

void Window::setTitle(const std::string &title) const {
    glfwSetWindowTitle(m_pWindow, title.c_str());
}

void Window::setCursorMode(const uint32_t &mode) const {
    glfwSetInputMode(m_pWindow, GLFW_CURSOR, mode);
}
