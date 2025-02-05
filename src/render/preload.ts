import {
    ApplicationScannerChannel,
    ApplicationStatusChannel,
    RenderMessage,
    RenderRequestChannel,
    ApplicationInfo,
    ApplicationStatus
} from "../types";
import { contextBridge, ipcRenderer } from "electron";

export const ApplicationMonitorApi = {
    // Start monitoring applications
    StartApplicationScanner: (callback: ApplicationScannerCallback) => {
        ipcRenderer.on(ApplicationScannerChannel, (_, sa: ScannedApplications) => {
            callback(sa);
        });
        const message: RenderMessage = { request: "ApplicationScanner:StartListener" };
        ipcRenderer.send(RenderRequestChannel, message);
    },

    // Start monitoring a specific application's status
    StartApplicationStatusMonitor: (appInfo: ApplicationInfo, callback: (status: ApplicationStatus) => void) => {
        ipcRenderer.on(ApplicationStatusChannel, (_, status: ApplicationStatus) => {
            callback(status);
        });

        const message: RenderMessage = {
            request: "ApplicationStatus:ListenForStatus",
            payload: appInfo
        };
        ipcRenderer.send(RenderRequestChannel, message);
    },

    // Stop monitoring the application status
    StopApplicationStatusMonitor: () => {
        const message: RenderMessage = { request: "ApplicationStatus:StopListening" };
        ipcRenderer.send(RenderRequestChannel, message);
    }
};

export const ApplicationMonitorApiName = "ApplicationMonitorApi";

process.once("loaded", () => {
    contextBridge.exposeInMainWorld(ApplicationMonitorApiName, ApplicationMonitorApi);
});
