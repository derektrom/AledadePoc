#include "application-status.hpp"
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

class WindowsApplicationStatus : public NativeApplicationStatus {
public:
    WindowsApplicationStatus(Napi::Env env, Napi::Object exports) : NativeApplicationStatus(env, exports) {}
    void MonitorApplication();
};

void NativeApplicationStatus::MonitorApplication() {
    while (running) {
        HWND hwnd = FindWindow(NULL, currentApp.windowTitle.c_str());
        ApplicationStatus appStatus;

        if (!hwnd) {
            appStatus.status = "closed";
        }
        else {
            WINDOWPLACEMENT placement = { sizeof(WINDOWPLACEMENT) };
            GetWindowPlacement(hwnd, &placement);

            if (placement.showCmd == SW_SHOWMINIMIZED) {
                appStatus.status = "hidden";
            }
            else {
                RECT rect;
                GetWindowRect(hwnd, &rect);
                appStatus.status = "open";
                appStatus.dimensions = { rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top };
            }
        }

        callback.BlockingCall([&](Napi::Env env, Napi::Function jsCallback) {
            Napi::Object result = Napi::Object::New(env);
            result.Set("status", appStatus.status);
            if (appStatus.dimensions) {
                Napi::Object dim = Napi::Object::New(env);
                dim.Set("x", appStatus.dimensions->x);
                dim.Set("y", appStatus.dimensions->y);
                dim.Set("width", appStatus.dimensions->width);
                dim.Set("height", appStatus.dimensions->height);
                result.Set("dimensions", dim);
            }
            jsCallback.Call({ result });
            });

        std::this_thread::sleep_for(std::chrono::milliseconds(pollTime.load()));
    }
}
