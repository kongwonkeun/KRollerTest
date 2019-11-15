//

#include "keyboard.h"

#include <stdexcept>

EXTERN_C_START

Keyboard::Keyboard() : Device{ L"\\\\?\\HID#KROLLER&Col05#1" }{}

void
Keyboard::initialize()
{
    if (isInitialized()) throw std::runtime_error{ "error: already initialized" };
    Device::initialize();
}

void
Keyboard::abort()
{
    Device::abort();
}

void
Keyboard::type(BYTE keyCode)
{
    BYTE keyCodes[6] = { KEY_NONE };

    keyCodes[0] = keyCode;
    sendKeyboardReport(keyCodes);

    keyCodes[0] = KEY_NONE;
    sendKeyboardReport(keyCodes);
}

void
Keyboard::sendKeyboardReport(BYTE* keyCodes)
{
    Report report;
    report.reportId = REPORT_ID;
    report.modifiers = modifiers_;
    report.reserved = 0x00;
    std::memcpy(report.keyCodes, keyCodes, 6);

    setOutputReport(&report, static_cast<DWORD>(sizeof(Report)));
}

EXTERN_C_END

//