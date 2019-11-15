//

#include "registry.h"

EXTERN_C_START

Registry& 
Registry::get()
{
    static Registry instance;
    return instance;
}

int
Registry::getMouseSensitivity() const
{
    DWORD size = 3;
    char buffer[3];

    LONG getValueResult = RegGetValueA(
        HKEY_CURRENT_USER,
        "Control Panel\\Mouse",
        "MouseSensitivity",
        RRF_RT_REG_SZ,
        nullptr,
        &buffer,
        &size
    );
    if (getValueResult != ERROR_SUCCESS) {
        return -1;
    }
    return atoi(buffer);
}

int
Registry::getMouseSpeed() const
{
    DWORD size = 2;
    char buffer[2];

    LONG getValueResult = RegGetValueA(
        HKEY_CURRENT_USER,
        "Control Panel\\Mouse",
        "MouseSpeed",
        RRF_RT_REG_SZ,
        nullptr,
        &buffer,
        &size
    );
    if (getValueResult != ERROR_SUCCESS) {
        return -1;
    }
    return atoi(buffer);
}

EXTERN_C_END

//