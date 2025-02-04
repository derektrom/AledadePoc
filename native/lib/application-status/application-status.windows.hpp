#ifndef WINDOWS_APPLICATION_STATUS_HPP
#define WINDOWS_APPLICATION_STATUS_HPP

#include "../utilities/classes.hpp"
#include "../utilities/classes.windows.hpp"
#include "../utilities/helpers.windows.hpp"

#include <map>
#include <windows.h>

#include "application-status.hpp"

class WindowsApplicationStatus : public NativeApplicationStatus {
public:
    WindowsApplicationStatus(Napi::Env env, Napi::Object exports) : Napi::Addon<WindowsApplicationStatus>(env, exports) {
        DefineAddon(
            exports,
            {
                InstanceMethod("ListenForStatus", &WindowsApplicationStatus::ListenForStatus),
                InstanceMethod("StopListener", &WindowsApplicationStatus::StopListener)
            }
        );
    }
    void ListenForStatus(const Napi::CallbackInfo& info);
    void StopListener(const Napi::CallbackInfo& info);
    void MonitorApplication();

    static void EnqueueWindowEvent(
        HWINEVENTHOOK hWinEventHook,
        DWORD event,
        HWND hwnd,
        LONG idObject,
        LONG idChild,
        DWORD idEventThread,
        DWORD dwmsEventTime
    );

private:
    void setupHooks();
    void teardownHooks();
    void runScan();
    std::map<HWND, Window> _windows;
    HWINEVENTHOOK _windows_callback_hook = NULL;
};

#endif // WINDOWS_APPLICATION_STATUS_HPP