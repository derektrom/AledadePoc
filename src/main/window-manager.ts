import { BrowserWindow, globalShortcut, ipcMain } from "electron";
import { ApplicationScannerChannel, ApplicationStatusChannel, RenderMessage, RenderRequestChannel } from "../types";
import { LoadNativeModule } from "./utils";
import { ApplicationStatus } from "./application-status";

const NativeApplicationScanner = LoadNativeModule("native-application-scanner");

export class WindowManager {
    private window: BrowserWindow;
    private readonly windowSize = { width: 720, height: 420 };
    private applicationScannerIsRunning: boolean = false;
    private applicationStatusIsRunning: boolean = false;

    constructor(preloadEntry: string, staticEntry: string, devEntry: string) {
        this.window = new BrowserWindow({
            webPreferences: { preload: preloadEntry },
            height: this.windowSize.height,
            width: this.windowSize.width,
            skipTaskbar: true,
            show: true,
            frame: true,
            alwaysOnTop: true,
            transparent: false,
            resizable: true,
            autoHideMenuBar: true,
        });

        if (devEntry) {
            this.window.loadURL(devEntry);
        } else {
            this.window.loadFile(staticEntry);
        }

        globalShortcut.register("Control+Shift+I", () => {
            this.window.webContents.openDevTools({
                mode: "detach",
                activate: false,
            });
        });

        this.setupIpcHandlers();

        this.startApplicationStatusMonitor();
    }

    private setupIpcHandlers() {
        ipcMain.on(RenderRequestChannel, (_, message: RenderMessage) => {
            switch (message.request) {
                case "ApplicationScanner:StartListener":
                    if (this.applicationScannerIsRunning) {
                        NativeApplicationScanner.StopListener();
                    }
                    NativeApplicationScanner.ListenForApplications((scannedApplications) => {
                        this.window.webContents.send(ApplicationScannerChannel, scannedApplications);
                    });
                    this.applicationScannerIsRunning = true;
                    break;

                case "ApplicationScanner:StopListener":
                    NativeApplicationScanner.StopListener();
                    this.applicationScannerIsRunning = false;
                    break;

                case "ApplicationStatus:ListenForStatus":
                    if (this.applicationStatusIsRunning) {
                        ApplicationStatus.getInstance().StopListening();
                    }

                    if (!message.payload || !message.payload.applicationName || !message.payload.windowTitle) {
                        throw new Error("Invalid payload: Expected applicationName and windowTitle");
                    }

                    ApplicationStatus.getInstance().ListenForStatus(
                        message.payload.applicationName,
                        message.payload.windowTitle,
                        (status) => {
                            console.log("Application Status Update:", status);
                            this.window.webContents.send(ApplicationStatusChannel, status);
                        }
                    );

                    this.applicationStatusIsRunning = true;
                    break;

                case "ApplicationStatus:StopListener":
                    ApplicationStatus.getInstance().StopListening();
                    this.applicationStatusIsRunning = false;
                    break;

                default:
                    throw new Error(
                        `Main thread received unhandled request type on IPC channel ${RenderRequestChannel}: ${message.request}`
                    );
            }
        });
    }

    private startApplicationStatusMonitor() {
        const appName = process.platform === "win32" ? "Notepad" : "TextEdit";
        const windowTitle = process.platform === "win32" ? "Untitled - Notepad" : "Untitled";

        console.log(`Starting monitoring for ${appName}`);

        ApplicationStatus.getInstance().ListenForStatus(appName, windowTitle, (status) => {
            console.log("Application Status Update:", appName, windowTitle, status);
        });
    }
}
