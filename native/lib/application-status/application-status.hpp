#ifndef APPLICATION_STATUS_HPP
#define APPLICATION_STATUS_HPP

#include "../utilities/helpers.hpp"
#include <cstddef>
#include <napi.h>
#include <thread>
#include <utility>
#include <mutex>

class PlatformNativeApplicationStatus {
public:
    virtual ~PlatformNativeApplicationStatus() {
        StopListener();
    }
    virtual void ListenForStatus(ApplicationInfo appInfo, Napi::ThreadSafeFunction&& callback) {
        _callback = std::move(callback);
        _thread = std::thread(&PlatformNativeApplicationStatus::MonitorApplication, this, appInfo);
    }
    virtual void StopListener() {
        _callback = NULL;
    }
protected:
    PlatformNativeApplicationStatus() = default;
    Napi::ThreadSafeFunction _callback = NULL;
    std::thread _thread;
    virtual void MonitorApplication(ApplicationInfo appInfo) = 0;
};

// Declare NativeApplicationStatus
class NativeApplicationStatus : public Napi::Addon<NativeApplicationStatus> {
public:
    NativeApplicationStatus(Napi::Env env, Napi::Object exports);

protected:
    std::atomic<bool> running{ false };
    ApplicationInfo currentApp;
    Napi::ThreadSafeFunction callback;
private:
    void ListenForStatus(const Napi::CallbackInfo& info);
    void StopListening(const Napi::CallbackInfo& info);

    std::thread listenerThread;
    std::mutex mutex;

    void MonitorApplication();
};

#endif // APPLICATION_STATUS_HPP
