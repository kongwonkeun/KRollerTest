//

#include "mouse.h"
#include "registry.h"

#include <stdexcept>

Mouse::Mouse() : Device{ L"\\\\?\\HID#KROLLER&Col03#1" }{}

void
Mouse::initialize()
{
    if (isInitialized()) throw std::runtime_error{ "error: already initialized" };
    populateRangeSpeedVector();
    Device::initialize();
}

void
Mouse::abort()
{
    Device::abort();
}

void
Mouse::leftButtonDown()
{
    if (!isInitialized()) throw std::runtime_error{ "error: not initialized" };
    buttons_ |= BUTTON_LEFT;
    sendMouseReport(0, 0);
}

void
Mouse::leftButtonUp()
{
    if (!isInitialized()) throw std::runtime_error{ "error: not initialized" };
    buttons_ &= ~BUTTON_LEFT;
    sendMouseReport(0, 0);
}

void
Mouse::leftButtonClick()
{
    if (!isInitialized()) throw std::runtime_error{ "error: not initialized" };
    leftButtonDown();
    leftButtonUp();
}

void
Mouse::rightButtonDown()
{
    if (!isInitialized()) throw std::runtime_error{ "error: not initialized" };
    buttons_ |= BUTTON_RIGHT;
    sendMouseReport(0, 0);
}

void
Mouse::rightButtonUp()
{
    if (!isInitialized()) throw std::runtime_error{ "error: not initialized" };
    buttons_ &= ~BUTTON_RIGHT;
    sendMouseReport(0, 0);
}

void
Mouse::rightButtonClick()
{
    if (!isInitialized()) throw std::runtime_error{ "error: not initialized" };
    rightButtonDown();
    rightButtonUp();
}

void
Mouse::middleButtonDown()
{
    if (!isInitialized()) throw std::runtime_error{ "error: not initialized" };
    buttons_ |= BUTTON_MIDDLE;
    sendMouseReport(0, 0);
}

void
Mouse::middleButtonUp()
{
    if (!isInitialized()) throw std::runtime_error{ "error: not initialized" };
    buttons_ &= ~BUTTON_MIDDLE;
    sendMouseReport(0, 0);
}

void
Mouse::middleButtonClick()
{
    if (!isInitialized()) throw std::runtime_error{ "error: not initialized" };
    middleButtonDown();
    middleButtonUp();
}

void
Mouse::moveCursor(POINT point)
{
    if (!isInitialized()) throw std::runtime_error{ "error: not initialized" };
    moveCursor(point.x, point.y);
}

void
Mouse::moveCursor(LONG x, LONG y)
{
    if (!isInitialized()) throw std::runtime_error{ "error: not initialized" };
    while (!isAborted())
    {
        DWORD getCurrentCursorPositionError = ERROR_SUCCESS;
        POINT currentPoint = getCurrentCursorPosition(&getCurrentCursorPositionError);
        if (getCurrentCursorPositionError != ERROR_SUCCESS) {
            throw std::runtime_error{ "error: cursor processing" };
        }

        LONG distance = (LONG)sqrt(pow(x - currentPoint.x, 2) + pow(y - currentPoint.y, 2));
        if (distance <= DISTANCE_TOLERANCE) {
            return;
        }

        CHAR xSpeed = static_cast<CHAR>(getSpeedByRange(abs(x - currentPoint.x)));
        xSpeed = (x > currentPoint.x ? xSpeed : -xSpeed);

        CHAR ySpeed = static_cast<CHAR>(getSpeedByRange(abs(y - currentPoint.y)));
        ySpeed = (y > currentPoint.y ? ySpeed : -ySpeed);

        sendMouseReport(xSpeed, ySpeed);
        Sleep(1);
    }
}

void
Mouse::populateRangeSpeedVector()
{
    rangeSpeedVector_.clear();
    int rangeIndex = 0;
    for (int speed = 0; speed < MAX_ABSOLUTE_SPEED + 1; ++speed) {
        int range = getRangeBySpeed(speed);
        while (rangeIndex <= range) {
            rangeSpeedVector_.push_back(speed);
            ++rangeIndex;
        }
    }
}

int
Mouse::getSpeedByRange(int range) const
{
    if (rangeSpeedVector_.size() == 0) {
        return static_cast<int>(sqrt(range));
    }
    if (range > rangeSpeedVector_.size() - 1) {
        range = static_cast<int>(rangeSpeedVector_.size()) - 1;
    }
    return rangeSpeedVector_.at(range);
}

int
Mouse::getRangeBySpeed(int speed)
{
    double sensitivityFactors[21] = { 0, 0.03125, 0.625, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875, 1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75, 3, 3.25, 3.5 };
    int sensitivity = Registry::get().getMouseSensitivity();
    int eppSpeed = Registry::get().getMouseSpeed();

    if (sensitivity < SENSITIVITY_MIN || sensitivity > SENSITIVITY_MAX || eppSpeed < EPP_DISABLED || eppSpeed > EPP_ENABLED) {
        return speed;
    }

    double sensitivityFactor = eppSpeed == EPP_ENABLED ? 1 : sensitivityFactors[sensitivity];
    double eppFactor = eppSpeed == EPP_ENABLED ? 0.25 * sensitivity : 1;
    double range = static_cast<double>(speed)* sensitivityFactor* eppFactor;
    return static_cast<int>(range);
}

POINT
Mouse::getCurrentCursorPosition(LPDWORD error)
{
    POINT currentPoint{ 0, 0 };
    BOOL getCursorPosResult = GetCursorPos(&currentPoint);
    if (!getCursorPosResult && nullptr != error) {
        *error = GetLastError();
    }
    return currentPoint;
}

void
Mouse::sendMouseReport(CHAR xSpeed, CHAR ySpeed)
{
    Report report;
    report.reportId = REPORT_ID;
    report.buttons = buttons_;
    report.x = xSpeed;
    report.y = ySpeed;

    setOutputReport(&report, static_cast<DWORD>(sizeof(Report)));
}

//