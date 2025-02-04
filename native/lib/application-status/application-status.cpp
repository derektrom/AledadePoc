#include "application-status.hpp"
#include <iostream>
#include <windows.h>

NativeApplicationStatus::NativeApplicationStatus(Napi::Env env, Napi::Object exports){
    DefineAddon(
        exports,
        {
            InstanceMethod("ListenForStatus", &NativeApplicationStatus::ListenForStatus),
            InstanceMethod("StopListening", &NativeApplicationStatus::StopListening)
        }
    );
}

void NativeApplicationStatus::ListenForStatus(const Napi::CallbackInfo& info) {
    std::lock_guard<std::mutex> lock(mutex);

    if (running) {
        StopListening(info);
    }

    auto env = info.Env();
    if (info.Length() != 2 || !info[0].IsObject() || !info[1].IsFunction()) {
        Napi::TypeError::New(env, "Expected (Object, Function)").ThrowAsJavaScriptException();
        return;
    }

    auto input = info[0].As<Napi::Object>();
    currentApp.applicationName = input.Get("applicationName").As<Napi::String>().Utf8Value();
    currentApp.windowTitle = input.Get("windowTitle").As<Napi::String>().Utf8Value();
    callback = Napi::ThreadSafeFunction::New(env, info[1].As<Napi::Function>(), "AppStatusCallback", 0, 1);

    running = true;
    listenerThread = std::thread(&NativeApplicationStatus::MonitorApplication, this);
}

void NativeApplicationStatus::StopListening(const Napi::CallbackInfo& info) {
    std::lock_guard<std::mutex> lock(mutex);
    running = false;
    if (listenerThread.joinable()) {
        listenerThread.join();
    }
    callback = nullptr;
}


            
