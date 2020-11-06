#ifndef MODULES_HPP_
#define MODULES_HPP_

#include <vector>

namespace mod {

const std::string ZID = "z5206032\n";

/**
 * Module struct to hold important module information
 */
struct ModuleInfo {
    const std::wstring name;
    const std::wstring ip;
    const int port;
    const int heartbeat;
};


/* Constant module definitions */
const ModuleInfo CAMERA = { L"CameraModule.exe", L"192.168.1.200", 26000, 0 };
const ModuleInfo DISPLAY = { L"DisplayModule.exe", L"", -1, 1 };
const ModuleInfo GPS = { L"GNSSModule.exe", L"192.168.1.200", 24000, 2 };
const ModuleInfo LASER = { L"LaserModule.exe", L"192.168.1.200", 23000, 3 };
const ModuleInfo MANAGE = { L"ProcessManagementModule.exe", L"", -1, 4 };
const ModuleInfo TELEOP = { L"VehicleControlModule.exe", L"192.168.1.200", 25000, 5 };

/**
 * Modules to startup when process management starts
 */
const ModuleInfo STARTUP[] = {
    DISPLAY,
    LASER,
    GPS,
    CAMERA,
    TELEOP,
};

}  // namespace mod

# endif // MODULES_HPP_
