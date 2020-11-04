#ifndef MODULES_HPP_
#define MODULES_HPP_

namespace mod {

const char ZID[] = "z5206032\n";

struct Module {
    const WCHAR* name;
    const WCHAR* ip;
    const int port;
};

const Module CAMERA = { L"CameraModule.exe", L"192.168.1.200", 26000 };
const Module DISPLAY = { L"DisplayModule.exe" };
const Module GPS = { L"GNSSModule.exe", L"192.168.1.200", 24000 };
const Module LASER = { L"LaserModule.exe", L"192.168.1.200", 23000 };
const Module MANAGE = { L"ProcessManagementModule.exe" };
const Module TELEOP = { L"VehicleControlModule.exe", L"192.168.1.200", 25000 };

const Module MODULES[] = {
    DISPLAY,
    LASER,
    GPS,
    CAMERA,
    TELEOP,
};

}  // namespace mod

# endif // MODULES_HPP_