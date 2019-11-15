//

#pragma once

#include "common.h"
#include "mouse.h"
#include "keyboard.h"

#define HIDMINI_DEFAULT_PID     0xFEED
#define HIDMINI_DEFAULT_VID     0xDEED

// these are the device attributes returned by the mini driver
// in response to IOCTL_HID_GET_DEVICE_ATTRIBUTES
#define HIDMINI_TEST_PID        0xDEEF
#define HIDMINI_TEST_VID        0xFEED
#define HIDMINI_TEST_VERSION    0x0505

#define USE_ALT_USAGE_PAGE  "UseAltUsagePage"

EXTERN_C_START

// function prototypes
BOOLEAN TestKRoller(void);
BOOLEAN FindMatchingDevice(_In_ LPGUID guid, _Out_ HANDLE *handle);
BOOLEAN CheckIfOurDevice(HANDLE file);

BOOLEAN GetFeature(HANDLE file);
BOOLEAN SetFeature(HANDLE file);
BOOLEAN GetInputReport(HANDLE file);
BOOLEAN SetOutputReport(HANDLE file);
BOOLEAN GetStrings(HANDLE file);
BOOLEAN GetIndexedString(HANDLE file);
BOOLEAN ReadInputData(_In_ HANDLE file);
BOOLEAN WriteOutputData(_In_ HANDLE file);

EXTERN_C_END

//
