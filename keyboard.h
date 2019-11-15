//

#pragma once

#include "device.h"
#include "key.h"

#include <Windows.h>

class Keyboard : public Device
{
public:
    explicit Keyboard();
    Keyboard(const Keyboard&) = delete;
    void operator =(const Keyboard&) = delete;
    virtual ~Keyboard() override = default;
    virtual void initialize() override;
    virtual void abort() override;
    virtual void type(BYTE keyCode);

protected:
    BYTE modifiers_{ KEY_NONE };
    static const BYTE REPORT_ID{ 0x04 };

private:
#pragma pack(1)
    struct Report {
        BYTE reportId;
        BYTE modifiers;
        BYTE reserved;
        BYTE keyCodes[6];
    };
#pragma pack()
    virtual void sendKeyboardReport(BYTE* keyCodes);
};

//
