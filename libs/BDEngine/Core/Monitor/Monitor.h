//
// Created by Maks930 on 08/24/2025.
//

#ifndef BADENGINE_MONITOR_H
#define BADENGINE_MONITOR_H

#include <cstdint>
#include <utility>

using WinSize = std::pair<uint32_t, uint32_t>;
namespace bde::core::monitor {
    WinSize getPrimaryMonitorSize();
} // bde::core::monitor

#endif //BADENGINE_MONITOR_H