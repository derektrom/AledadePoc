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
};

#endif // WINDOWS_APPLICATION_STATUS_HPP