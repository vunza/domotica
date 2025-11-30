#pragma once

#include "ctrl_debug.h"

class Device {
public:
    void initialize();
    void run();
private:
    bool checkHardware();
};