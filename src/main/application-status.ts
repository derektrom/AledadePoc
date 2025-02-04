import { LoadNativeModule } from "./utils";
import { ApplicationInfo, ApplicationStatus as ApplicationStatusType, ApplicationStatusCallback } from "../types";

const NativeApplicationStatus = LoadNativeModule("native-application-status");

export class ApplicationStatus {
    private static instance: ApplicationStatus;
    private running: boolean = false;

    public static getInstance(): ApplicationStatus {
        if (!ApplicationStatus.instance) {
            ApplicationStatus.instance = new ApplicationStatus();
        }
        return ApplicationStatus.instance;
    }

    public ListenForStatus(applicationName: string, windowTitle: string, callback: ApplicationStatusCallback): void {
        if (this.running) {
            this.StopListening();
        }

        this.running = true;

        const appInfo: ApplicationInfo = { applicationName, windowTitle };

        NativeApplicationStatus.ListenForStatus(
            appInfo,
            (status: unknown) => {
                callback(status as ApplicationStatusType);
            }
        );
    }

    public StopListening(): void {
        if (!this.running) return;
        NativeApplicationStatus.StopListener();
        this.running = false;
    }
}
