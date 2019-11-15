//

#include "device.h"

#include <cfgmgr32.h>
#include <hidsdi.h>
#include <stdexcept>

EXTERN_C_START

Device::Device(PCWSTR deviceInterface) : deviceInterface_{ deviceInterface }{}

void
Device::initialize()
{
    if (isInitialized()) throw std::runtime_error{ "error: already initialized" };

    GUID guid;
    HidD_GetHidGuid(&guid);

    ULONG deviceInterfaceListLength = 0;
    CONFIGRET configRet = CM_Get_Device_Interface_List_Size(
        &deviceInterfaceListLength,
        &guid,
        nullptr,
        CM_GET_DEVICE_INTERFACE_LIST_PRESENT
    );

    if (configRet != CR_SUCCESS) throw std::runtime_error{ "error: getting device interface list size" };
    if (deviceInterfaceListLength <= 1) throw std::runtime_error{ "error: empty device interface list" };

    PWSTR deviceInterfaceList = (PWSTR)malloc(deviceInterfaceListLength * sizeof(WCHAR));
    if (deviceInterfaceList == nullptr) throw std::runtime_error{ "error: allocatting device interface list" };
    ZeroMemory(deviceInterfaceList, deviceInterfaceListLength * sizeof(WCHAR));

    configRet = CM_Get_Device_Interface_List(
        &guid,
        nullptr,
        deviceInterfaceList,
        deviceInterfaceListLength,
        CM_GET_DEVICE_INTERFACE_LIST_PRESENT
    );

    if (configRet != CR_SUCCESS) {
        free(deviceInterfaceList);
        throw std::runtime_error{ "error: getting device interface list" };
    }

    size_t deviceInterfaceLength = wcslen(deviceInterface_);
    HANDLE file = INVALID_HANDLE_VALUE;

    for (PWSTR currentInterface = deviceInterfaceList; *currentInterface; currentInterface += wcslen(currentInterface) + 1) {
        if (wcsncmp(currentInterface, deviceInterface_, deviceInterfaceLength) != 0) {
            continue;
        }
        file = CreateFile(currentInterface, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        if (file == INVALID_HANDLE_VALUE) {
            continue;
        }
        break;
    }
    free(deviceInterfaceList);

    if (file == INVALID_HANDLE_VALUE) throw std::runtime_error{ "error: invalid handle value" };

    deviceHandle_ = file;
    isInitialized_ = true;
}

void
Device::abort()
{
    if (deviceHandle_ != nullptr && deviceHandle_ != INVALID_HANDLE_VALUE) {
        CloseHandle(deviceHandle_);
    }
    isAborted_ = true;
}

void
Device::setOutputReport(PVOID data, DWORD size)
{
    if (!HidD_SetOutputReport(deviceHandle_, data, size)) {
        throw std::runtime_error{ "error: HidD_SetOutputReport" };
    }
}

bool
Device::isInitialized() const { return isInitialized_; }

bool
Device::isAborted() const { return isAborted_; }

EXTERN_C_END

//