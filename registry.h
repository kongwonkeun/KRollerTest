//

#pragma once

#include <Windows.h>

class Registry
{
public:
    static Registry& get();
    int getMouseSensitivity() const;
    int getMouseSpeed() const;

private:
    explicit Registry() = default;
    Registry(const Registry&) = delete;
    void operator =(const Registry&) = delete;
    virtual ~Registry() = default;
};

//
