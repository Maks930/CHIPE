#ifndef BADENGINE_EVENT_H
#define BADENGINE_EVENT_H

#include <variant>

namespace bde::system {
    class Event;
    class Window;
} // bde::system

namespace bde::core::Keyboard {
    enum class Key;
}

class GLFWwindow;

class bde::system::Event {

public:
    struct Closed{};
    struct Resized{int width; int height;};

    struct KeyPressed {int code;};
    struct KeyReleased {int code;};

    struct JustKeyPressed {int code;};
    struct JustKeyReleased {int code;};

    struct MouseMoved {int x, y; float dx;float dy;};

    template <typename TEventSubtype>
    explicit Event(const TEventSubtype &eventSubtype) {
        if constexpr (isEventSubtype<TEventSubtype>)
            m_events = eventSubtype;
    }


    template <typename TEventSubtype>
    [[nodiscard]] bool is() const {
        if constexpr (isEventSubtype<TEventSubtype>)
            return std::holds_alternative<TEventSubtype>(m_events);
        return false;
    }

    template <typename TEventSubtype>
    [[nodiscard]] const TEventSubtype* get_if() const {
        if constexpr (isEventSubtype<TEventSubtype>)
            return std::get_if<TEventSubtype>(&m_events);
        return nullptr;
    }

    template <typename TEventSubtype>
    [[nodiscard]] TEventSubtype* get_if() {
        if constexpr (isEventSubtype<TEventSubtype>)
            return std::get_if<TEventSubtype>(&m_events);
        return nullptr;
    }

private:
    using EventVariant = std::variant<Closed,
                 Resized,
                 KeyPressed,
                 KeyReleased,
                 JustKeyPressed,
                 JustKeyReleased,
                 MouseMoved
    >;
	EventVariant m_events;

    template <typename T, typename ...Args>
    [[nodiscard]] static constexpr bool isInParameterPack(const std::variant<Args...>*) {
        return std::disjunction_v<std::is_same<T, Args>...>;
    }

    //template <typename T>
    //static constexpr bool isEventSubtype = isInParameterPack<T>(static_cast<decltype (&m_events)>(nullptr));

    template <typename T>
    static constexpr bool isEventSubtype = std::is_constructible_v<EventVariant, T>;

    friend class bde::system::Window;
};

#endif //BADENGINE_EVENT_H