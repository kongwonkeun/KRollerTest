//

#pragma once

#include <Windows.h>

class Device
{
public:
    explicit Device(PCWSTR deviceInterface);
    Device(const Device&) = delete;
    void operator =(const Device&) = delete;
    virtual ~Device() = default;
    virtual void initialize();
    virtual void abort();
    bool isInitialized() const;
    bool isAborted() const;

protected:
    void setOutputReport(PVOID data, DWORD size);

private:
    PCWSTR deviceInterface_{ nullptr };
    HANDLE deviceHandle_{ nullptr };
    bool isInitialized_{ false };
    bool isAborted_{ false };
};

//
