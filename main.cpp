//

#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <cfgmgr32.h>
#include <hidsdi.h>
#include <time.h>
#include <iostream>
#include <string>
#include <conio.h>

EXTERN_C_START

USAGE G_myUsagePage = 0xFF00;
USAGE G_myUsage = 0x01;

INT __cdecl
main(_In_ ULONG argc, _In_reads_(argc) PCHAR argv[])
{
    if (!TestKRoller()) {
        return EXIT_FAILURE;
    }

    Mouse mouse;
    try {
        mouse.initialize();
    }
    catch (const std::runtime_error &e) {
        std::cout << std::string("mouse initialization: ") + e.what() << std::endl;
        return EXIT_FAILURE;
    }
    printf("----mouse initialized----\n");

    Keyboard keyboard;
    try {
        keyboard.initialize();
    }
    catch (const std::runtime_error &e) {
        std::cout << std::string("keyboard initialization: ") + e.what() << std::endl;
        return EXIT_FAILURE;
    }
    printf("----keyboard initialized----\n");

    Sleep(5000);

    try {
        mouse.moveCursor(136, 271);
        printf("----move  1----\n");
        mouse.rightButtonClick();
        printf("----click 1----\n");
        Sleep(500);
        mouse.moveCursor(56, 315);
        printf("----move  2----\n");
        mouse.leftButtonClick();
        printf("----click 2----\n");
        Sleep(500);
        mouse.moveCursor(283, 350);
        printf("----move  3----\n");
        mouse.rightButtonClick();
        printf("----click 3----\n");
        Sleep(500);
        mouse.moveCursor(275, 271);
        printf("----move  4----\n");
        mouse.leftButtonClick();
        printf("----click 4----\n");
        Sleep(500);
    }
    catch (const std::runtime_error &e) {
        std::cout << std::string("----") + e.what() + std::string("----") << std::endl;
        return EXIT_FAILURE;
    }

    try {
        _getch();
        keyboard.type(KEY_A);
        printf("----key 1=%c----\n", _getch());
        keyboard.type(KEY_B);
        printf("----key 2=%c----\n", _getch());
        keyboard.type(KEY_C);
        printf("----key 3=%c----\n", _getch());
        keyboard.type(KEY_D);
        printf("----key 4=%c----\n", _getch());
    }
    catch (const std::runtime_error &e) {
        std::cout << std::string("----") + e.what() + std::string("----") << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

BOOLEAN
TestKRoller()
{
    GUID hidguid;
    HANDLE file = INVALID_HANDLE_VALUE;
    BOOLEAN found = FALSE;
    BOOLEAN bSuccess = FALSE;

    srand((unsigned)time(NULL));
    HidD_GetHidGuid(&hidguid);
    found = FindMatchingDevice(&hidguid, &file);

    if (found)
    {
        printf("....sending control request to our device\n");

        // get/set feature loopback
        bSuccess = GetFeature(file);
        if (bSuccess == FALSE) {
            printf("----GetLastError=0x%x----\n", GetLastError());
            goto cleanup;
        }
        bSuccess = SetFeature(file);
        if (bSuccess == FALSE) {
            printf("----GetLastError=0x%x----\n", GetLastError());
            goto cleanup;
        }
        bSuccess = GetFeature(file);
        if (bSuccess == FALSE) {
            printf("----GetLastError=0x%x----\n", GetLastError());
            goto cleanup;
        }

        // get/set report loopback
        bSuccess = GetInputReport(file);
        if (bSuccess == FALSE) {
            printf("----GetLastError=0x%x----\n", GetLastError());
            goto cleanup;
        }
        bSuccess = SetOutputReport(file);
        if (bSuccess == FALSE) {
            printf("----GetLastError=0x%x----\n", GetLastError());
            goto cleanup;
        }
        bSuccess = GetInputReport(file);
        if (bSuccess == FALSE) {
            printf("----GetLastError=0x%x----\n", GetLastError());
            goto cleanup;
        }
        
        // read/write report loopback
        bSuccess = ReadInputData(file);
        if (bSuccess == FALSE) {
            printf("----GetLastError=0x%x----\n", GetLastError());
            goto cleanup;
        }
        bSuccess = WriteOutputData(file);
        if (bSuccess == FALSE) {
            printf("----GetLastError=0x%x----\n", GetLastError());
            goto cleanup;
        }
        bSuccess = ReadInputData(file);
        if (bSuccess == FALSE) {
            printf("----GetLastError=0x%x----\n", GetLastError());
            goto cleanup;
        }

        // get strings
        bSuccess = GetIndexedString(file);
        if (bSuccess == FALSE) {
            printf("----GetLastError=0x%x----\n", GetLastError());
            goto cleanup;
        }
        bSuccess = GetStrings(file);
        if (bSuccess == FALSE) {
            printf("----GetLastError=0x%x----\n", GetLastError());
            goto cleanup;
        }
    }
    else {
        printf("Failure: Could not find our HID device\n");
    }

cleanup:

    if (found && bSuccess == FALSE) {
        printf("****** Failure: one or more commands to device failed *******\n");
    }
    if (file != INVALID_HANDLE_VALUE) {
        CloseHandle(file);
    }
    return bSuccess;
}

BOOLEAN
FindMatchingDevice(_In_ LPGUID interfaceGuid, _Out_ HANDLE *handle)
{
    CONFIGRET cr = CR_SUCCESS;
    PWSTR deviceInterfaceList = NULL;
    ULONG deviceInterfaceListLength = 0;
    PWSTR currentInterface;
    BOOLEAN bRet = FALSE;
    HANDLE devHandle = INVALID_HANDLE_VALUE;

    if (NULL == handle) {
        printf("Error: Invalid device handle parameter\n");
        return FALSE;
    }

    *handle = INVALID_HANDLE_VALUE;

    cr = CM_Get_Device_Interface_List_Size(&deviceInterfaceListLength, interfaceGuid, NULL, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
    if (cr != CR_SUCCESS) {
        printf("Error 0x%x retrieving device interface list size.\n", cr);
        goto clean0;
    }

    if (deviceInterfaceListLength <= 1) {
        bRet = FALSE;
        printf("Error: No active device interfaces found. Is the sample driver loaded?\n");
        goto clean0;
    }

    deviceInterfaceList = (PWSTR)malloc(deviceInterfaceListLength * sizeof(WCHAR));
    if (deviceInterfaceList == NULL) {
        printf("Error allocating memory for device interface list.\n");
        goto clean0;
    }
    ZeroMemory(deviceInterfaceList, deviceInterfaceListLength * sizeof(WCHAR));

    cr = CM_Get_Device_Interface_List(
        interfaceGuid,
        NULL,
        deviceInterfaceList,
        deviceInterfaceListLength,
        CM_GET_DEVICE_INTERFACE_LIST_PRESENT
    );
    if (cr != CR_SUCCESS) {
        printf("Error 0x%x retrieving device interface list.\n", cr);
        goto clean0;
    }

    // enumerate devices of this interface class
    printf("....looking for our HID device (with UsagePage=0x%04X and Usage=0x%02X)\n", G_myUsagePage, G_myUsage);

    for (currentInterface = deviceInterfaceList; *currentInterface; currentInterface += wcslen(currentInterface) + 1)
    {
        printf("----%ls----\n", currentInterface);
        devHandle = CreateFile(
            currentInterface,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, // no SECURITY_ATTRIBUTES structure
            OPEN_EXISTING, // no special create flags
            0, // no special attributes
            NULL // no template file
        );
        if (INVALID_HANDLE_VALUE == devHandle) {
            //printf("Warning: CreateFile failed: %d\n", GetLastError());
            continue;
        }
        if (CheckIfOurDevice(devHandle)) {
            bRet = TRUE;
            *handle = devHandle;
        } else {
            CloseHandle(devHandle);
        }
    }

clean0:

    if (deviceInterfaceList != NULL) {
        free(deviceInterfaceList);
    }
    if (CR_SUCCESS != cr) {
        bRet = FALSE;
    }
    return bRet;
}

BOOLEAN
CheckIfOurDevice(HANDLE file)
{
    PHIDP_PREPARSED_DATA ppd; // opaque parser info describing this device
    HIDP_CAPS caps; // capabilities of this hid device.
    HIDD_ATTRIBUTES attr; // device attributes

    if (!HidD_GetAttributes(file, &attr)) {
        printf("Error: HidD_GetAttributes failed\n");
        return FALSE;
    }

    printf("Device Attributes - PID: 0x%x, VID: 0x%x\n", attr.ProductID, attr.VendorID);
    if ((attr.VendorID != HIDMINI_DEFAULT_VID) || (attr.ProductID != HIDMINI_DEFAULT_PID))
    {
        printf("Device attributes doesn't match the sample\n");
        return FALSE;
    }

    if (!HidD_GetPreparsedData(file, &ppd))
    {
        printf("Error: HidD_GetPreparsedData failed\n");
        return FALSE;
    }

    if (!HidP_GetCaps(ppd, &caps))
    {
        printf("Error: HidP_GetCaps failed\n");
        HidD_FreePreparsedData(ppd);
        return FALSE;
    }

    printf("----0xFF00=0x%04x:0x01=0x%02x----\n", caps.UsagePage, caps.Usage);
    if ((caps.UsagePage == G_myUsagePage) && (caps.Usage == G_myUsage)) {
        printf("Success: Found my device...\n");
        return TRUE;
    }
    return FALSE;
}

BOOLEAN
GetFeature(HANDLE file)
{
    PMY_DEVICE_ATTRIBUTES myDevAttributes = NULL;
    ULONG bufferSize;
    PUCHAR buffer;
    BOOLEAN bSuccess;

    bufferSize = MAXIMUM_STRING_LENGTH;
    buffer = (BYTE*)malloc(bufferSize);
    //bufferSize = FEATURE_REPORT_SIZE_CB + 1;
    //buffer = (PUCHAR)malloc(bufferSize);
    if (!buffer)
    {
        printf("malloc failed\n");
        return FALSE;
    }
    ZeroMemory(buffer, bufferSize);

    // fill the first byte with report ID of control collection
    buffer[0] = REPORT_ID_CONTROL;

    // send Hid control code thru HidD_GetFeature API
    bSuccess = HidD_GetFeature(file, buffer, bufferSize);
    if (!bSuccess)
    {
        printf("failed HidD_GetFeature\n");
    }
    else
    {
        myDevAttributes = (PMY_DEVICE_ATTRIBUTES)(buffer + 1); // +1 to skip report id
        printf("Received following feature attributes from device:\n"
            "    VendorID: 0x%x,\n"
            "    ProductID: 0x%x,\n"
            "    VersionNumber: 0x%x\n",
            myDevAttributes->vendorId,
            myDevAttributes->productId,
            myDevAttributes->versionNumber);
    }
    free(buffer);
    return bSuccess;
}

BOOLEAN
SetFeature(HANDLE file)
{
    PHIDMINI_CONTROL_INFO controlInfo = NULL;
    PMY_DEVICE_ATTRIBUTES myDevAttributes = NULL;
    ULONG bufferSize;
    BOOLEAN bSuccess;

    bufferSize = sizeof(HIDMINI_CONTROL_INFO);
    controlInfo = (PHIDMINI_CONTROL_INFO)malloc(bufferSize);
    if (!controlInfo)
    {
        printf("malloc failed\n");
        return FALSE;
    }
    ZeroMemory(controlInfo, bufferSize);

    // fill the control structure with the report ID of the control collection and the control code.
    controlInfo->reportId = REPORT_ID_CONTROL;
    controlInfo->controlCode = HIDMINI_CONTROL_ATTRIBUTES;
    myDevAttributes = (PMY_DEVICE_ATTRIBUTES)&controlInfo->u.attributes;
    myDevAttributes->vendorId = HIDMINI_TEST_VID;
    myDevAttributes->productId = HIDMINI_TEST_PID;
    myDevAttributes->versionNumber = HIDMINI_TEST_VERSION;

    // set feature
    bSuccess = HidD_SetFeature(file, controlInfo, bufferSize);
    if (!bSuccess)
    {
        printf("failed HidD_SetFeature\n");
    }
    else
    {
        printf("Set following feature attributes on device:\n"
            "    VendorID: 0x%x,\n"
            "    ProductID: 0x%x,\n"
            "    VersionNumber: 0x%x\n",
            myDevAttributes->vendorId,
            myDevAttributes->productId,
            myDevAttributes->versionNumber);
    }
    free(controlInfo);
    return bSuccess;
}

BOOLEAN
GetInputReport(HANDLE file)
{
    ULONG bufferSize;
    PUCHAR buffer;
    BOOLEAN bSuccess;

    bufferSize = sizeof(HIDMINI_INPUT_REPORT);
    buffer = (PUCHAR)malloc(bufferSize);
    if (!buffer)
    {
        printf("malloc failed\n");
        return FALSE;
    }
    ZeroMemory(buffer, bufferSize);

    // fill the first byte with report ID of collection
    buffer[0] = REPORT_ID_CONTROL;

    // send Hid control code
    bSuccess = HidD_GetInputReport(file, buffer, bufferSize);
    if (!bSuccess)
    {
        printf("failed HidD_GetInputReport\n");
    }
    else {
        printf("Received following data in input report: %d\n", ((PHIDMINI_INPUT_REPORT)buffer)->data);
    }
    free(buffer);
    return bSuccess;
}


BOOLEAN
SetOutputReport(HANDLE file)
{
    ULONG bufferSize;
    PHIDMINI_OUTPUT_REPORT buffer;
    BOOLEAN bSuccess;

    bufferSize = sizeof(HIDMINI_OUTPUT_REPORT);
    buffer = (PHIDMINI_OUTPUT_REPORT)malloc(bufferSize);
    if (!buffer)
    {
        printf("malloc failed\n");
        return FALSE;
    }
    ZeroMemory(buffer, bufferSize);

    // fill the report
    buffer->reportId = REPORT_ID_CONTROL;
    buffer->data = (UCHAR)(rand() % UCHAR_MAX);

    // send Hid control code
    bSuccess = HidD_SetOutputReport(file, buffer, bufferSize);
    if (!bSuccess)
    {
        printf("failed HidD_SetOutputReport\n");
    }
    else {
        printf("Set following data in output report: %d\n", ((PHIDMINI_OUTPUT_REPORT)buffer)->data);
    }
    free(buffer);
    return bSuccess;
}

BOOLEAN
ReadInputData(_In_ HANDLE file)
{
    PHIDMINI_INPUT_REPORT report;
    ULONG bufferSize;
    BOOL bSuccess;
    DWORD bytesRead;

    bufferSize = sizeof(HIDMINI_INPUT_REPORT);
    report = (PHIDMINI_INPUT_REPORT)malloc(bufferSize);
    if (!report)
    {
        printf("malloc failed\n");
        return FALSE;
    }
    ZeroMemory(report, bufferSize);

    report->reportId = REPORT_ID_CONTROL;

    // get input data.
    bSuccess = ReadFile(
        file,        // HANDLE hFile,
        report,      // LPVOID lpBuffer,
        bufferSize,  // DWORD nNumberOfBytesToRead,
        &bytesRead,  // LPDWORD lpNumberOfBytesRead,
        NULL         // LPOVERLAPPED lpOverlapped
    );
    if (!bSuccess)
    {
        printf("failed ReadFile \n");
    }
    else
    {
        printf("Read following byte from device: %d\n", report->data);
    }
    free(report);
    return (BOOLEAN)bSuccess;
}

BOOLEAN
WriteOutputData(_In_ HANDLE file)
{
    PHIDMINI_OUTPUT_REPORT outputReport;
    ULONG outputReportSize;
    BOOL bSuccess;
    DWORD bytesWritten;

    outputReportSize = sizeof(HIDMINI_OUTPUT_REPORT);
    outputReport = (PHIDMINI_OUTPUT_REPORT)malloc(outputReportSize);
    if (!outputReport)
    {
        printf("malloc failed\n");
        return FALSE;
    }
    ZeroMemory(outputReport, outputReportSize);

    outputReport->reportId = REPORT_ID_CONTROL;
    outputReport->data = (UCHAR)(rand() % UCHAR_MAX);

    // write output data.
    bSuccess = WriteFile(
        file,                   // HANDLE hFile,
        (PVOID)outputReport,    // LPVOID lpBuffer,
        outputReportSize,       // DWORD nNumberOfBytesToRead,
        &bytesWritten,          // LPDWORD lpNumberOfBytesRead,
        NULL                    // LPOVERLAPPED lpOverlapped
    );
    if (!bSuccess)
    {
        printf("failed WriteFile\n");
    }
    else {
        printf("Wrote following byte to device: %d\n", outputReport->data);
    }
    free(outputReport);
    return (BOOLEAN)bSuccess;
}

BOOLEAN
GetIndexedString(HANDLE file)
{
    BOOLEAN bSuccess;
    BYTE* buffer;
    ULONG bufferLength;

    bufferLength = MAXIMUM_STRING_LENGTH;
    buffer = (BYTE*)malloc(bufferLength);
    if (!buffer)
    {
        printf("malloc failed\n");
        return FALSE;
    }
    ZeroMemory(buffer, bufferLength);

    bSuccess = HidD_GetIndexedString(
        file,
        HIDMINI_DEVICE_STRING_INDEX,    // IN ULONG StringIndex,
        (PVOID)buffer,                  // OUT PVOID Buffer,
        bufferLength                    // IN ULONG BufferLength
    );
    if (!bSuccess)
    {
        printf("failed WriteFile\n");
    }
    else {
        printf("Indexed string: %S\n", (PWSTR)buffer);
    }
    free(buffer);
    return bSuccess;
}

BOOLEAN
GetStrings(HANDLE file)
{
    BOOLEAN bSuccess;
    BYTE* buffer;
    ULONG bufferLength;

    bufferLength = MAXIMUM_STRING_LENGTH;
    buffer = (BYTE*)malloc(bufferLength);
    if (!buffer)
    {
        printf("malloc failed\n");
        return FALSE;
    }
    ZeroMemory(buffer, bufferLength);

    bSuccess = HidD_GetProductString(
        file,
        (PVOID)buffer,  // OUT PVOID  Buffer,
        bufferLength    // IN ULONG  BufferLength
    );
    if (!bSuccess)
    {
        printf("failed HidD_GetProductString\n");
        goto exit;
    }
    else {
        printf("Product string: %S\n", (PWSTR)buffer);
    }
    ZeroMemory(buffer, bufferLength);

    bSuccess = HidD_GetSerialNumberString(
        file,
        (PVOID)buffer,  // OUT PVOID  Buffer,
        bufferLength    // IN ULONG  BufferLength
    );
    if (!bSuccess)
    {
        printf("failed HidD_GetSerialNumberString\n");
        goto exit;
    }
    else
    {
        printf("Serial number string: %S\n", (PWSTR)buffer);
    }
    ZeroMemory(buffer, bufferLength);

    bSuccess = HidD_GetManufacturerString(
        file,
        (PVOID)buffer,  // OUT PVOID  Buffer,
        bufferLength    // IN ULONG  BufferLength
    );
    if (!bSuccess)
    {
        printf("failed HidD_GetManufacturerString\n");
        goto exit;
    }
    else {
        printf("Manufacturer string: %S\n", (PWSTR)buffer);
    }

exit:

    free(buffer);
    return bSuccess;
}

EXTERN_C_END

//