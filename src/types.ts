// Requests from renderer -> main
export const RenderRequestChannel = "render-request-channel";

export type RenderRequests =
    | "ApplicationScanner:StartListener"
    | "ApplicationScanner:StopListener"
    | "ApplicationStatus:ListenForStatus"
    | "ApplicationStatus:StopListening";

export type RenderMessage =
    | { request: "ApplicationScanner:StartListener" }
    | { request: "ApplicationScanner:StopListener" }
    | { request: "ApplicationStatus:ListenForStatus"; payload: ApplicationInfo }
    | { request: "ApplicationStatus:StopListening" };

// Responses / callbacks main -> renderer
export const ApplicationScannerChannel = "applicationScannerChannel";
export const ApplicationStatusChannel = "applicationStatusChannel";

// Application Info Type
export interface ApplicationInfo {
    applicationName: string;
    windowTitle: string;
}

// Application Status Type
export interface WindowDimensions {
    x: number;
    y: number;
    width: number;
    height: number;
}

export interface ApplicationStatus {
    status: "open" | "hidden" | "closed";
    dimensions?: WindowDimensions;
}

// Callback Type
export type ApplicationStatusCallback = (status: ApplicationStatus) => void;
