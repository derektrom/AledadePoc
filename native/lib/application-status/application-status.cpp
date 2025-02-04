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

    auto env = info.Env();

    if (info.Length() != 2 || !info[0].IsObject() || !info[1].IsFunction()) {
        Napi::TypeError::New(env, "Expected (ApplicationInfo, callback)").ThrowAsJavaScriptException();
        return;
    }

    Napi::Object input = info[0].As<Napi::Object>();

    // Extract applicationName & windowTitle
    if (!input.Has("applicationName") || !input.Has("windowTitle")) {
        Napi::TypeError::New(env, "Expected properties applicationName and windowTitle").ThrowAsJavaScriptException();
        return;
    }

    currentApp.applicationName = input.Get("applicationName").As<Napi::String>().Utf8Value();
    currentApp.windowTitle = input.Get("windowTitle").As<Napi::String>().Utf8Value();

    // Extract callback function
    Napi::Function jsCallback = info[1].As<Napi::Function>();
    callback = Napi::ThreadSafeFunction::New(
        env,
        jsCallback,
        "AppStatusCallback",
        0,
        1,
        [](Napi::Env) {} // Finalizer
    );

    running = true;
    listenerThread = std::thread(&NativeApplicationStatus::MonitorApplication, this);

    // Log the start of the monitoring process
    Napi::Function consoleLog = env.Global().Get("console").As<Napi::Object>().Get("log").As<Napi::Function>();
    consoleLog.Call({ Napi::String::New(env, "Successfully started NativeApplicationStatus") });
}


void NativeApplicationStatus::StopListening(const Napi::CallbackInfo& info) {
    std::lock_guard<std::mutex> lock(mutex);
    running = false;
    if (listenerThread.joinable()) {
        listenerThread.join();
    }
    callback = nullptr;
}


            
