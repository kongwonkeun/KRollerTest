//

#pragma once

#include <Windows.h>

// report id
#define REPORT_ID_MOUSE_INPUT       0x01
#define REPORT_ID_MOUSE_OUTPUT      0x02
#define REPORT_ID_KEYBOARD_INPUT    0x03
#define REPORT_ID_KEYBOARD_OUTPUT   0x04

#define REPORT_ID_CONTROL           0x11
#define REPORT_ID_FEATURE           0x12

//#include <pshpack1.h>
#pragma pack(1)

typedef struct _HID_MOUSE_INPUT_REPORT {
    BYTE    reportId;
    BYTE    buttons;
    CHAR    x;
    CHAR    y;
}
HID_MOUSE_INPUT_REPORT,
*PHID_MOUSE_INPUT_REPORT;

typedef struct _HID_MOUSE_OUTPUT_REPORT {
    BYTE    reportId;
    BYTE    buttons;
    CHAR    x;
    CHAR    y;
}
HID_MOUSE_OUTPUT_REPORT,
*PHID_MOUSE_OUTPUT_REPORT;

typedef struct _HID_KEYBOARD_INPUT_REPORT {
    BYTE    reportId;
    BYTE    modifiers;
    BYTE    reserved;
    BYTE    keyCodes[6];
}
HID_KEYBOARD_INPUT_REPORT,
*PHID_KEYBOARD_INPUT_REPORT;

typedef struct _HID_KEYBOARD_OUTPUT_REPORT {
    BYTE    reportId;
    BYTE    modifiers;
    BYTE    reserved;
    BYTE    keyCodes[6];
}
HID_KEYBOARD_OUTPUT_REPORT,
*PHID_KEYBOARD_OUTPUT_REPORT;

// input from hidmini
typedef struct _HIDMINI_INPUT_REPORT
{
    UCHAR   reportId;
    UCHAR   data;
}
HIDMINI_INPUT_REPORT,
*PHIDMINI_INPUT_REPORT;

// output to hidmini
typedef struct _HIDMINI_OUTPUT_REPORT
{
    UCHAR   reportId;
    UCHAR   data;
    USHORT  pad1;
    ULONG   pad2;
}
HIDMINI_OUTPUT_REPORT,
*PHIDMINI_OUTPUT_REPORT;

typedef struct _MY_DEVICE_ATTRIBUTES
{
    USHORT  vendorId;
    USHORT  productId;
    USHORT  versionNumber;
}
MY_DEVICE_ATTRIBUTES,
*PMY_DEVICE_ATTRIBUTES;

typedef struct _HIDMINI_CONTROL_INFO
{
    UCHAR   reportId;
    UCHAR   controlCode;
    union {
        MY_DEVICE_ATTRIBUTES attributes;
        struct {
            ULONG dummy1;
            ULONG dummy2;
        } Dummy;
    } u;
}
HIDMINI_CONTROL_INFO,
*PHIDMINI_CONTROL_INFO;

//#include <poppack.h>
#pragma pack()

#define HIDMINI_CONTROL_ATTRIBUTES     0x00
#define HIDMINI_DEVICE_STRING          L"UMDF Virtual Hidmini Device"
#define HIDMINI_MANUFACTURER_STRING    L"UMDF Virtual Hidmini Device Manufacturer"
#define HIDMINI_PRODUCT_STRING         L"UMDF Virtual Hidmini Device Product"
#define HIDMINI_SERIAL_NUMBER_STRING   L"UMDF Virtual Hidmini Device Serial Number"
#define HIDMINI_DEVICE_STRING_INDEX    5

#define MAXIMUM_STRING_LENGTH          (126 * sizeof(WCHAR))

#define FEATURE_REPORT_SIZE_CB  ((USHORT)(sizeof(HIDMINI_CONTROL_INFO)  - 1))
#define INPUT_REPORT_SIZE_CB    ((USHORT)(sizeof(HIDMINI_INPUT_REPORT)  - 1))
#define OUTPUT_REPORT_SIZE_CB   ((USHORT)(sizeof(HIDMINI_OUTPUT_REPORT) - 1))

//
