
////

// Application Scanner

declare interface ScannedApplication {
  windowTitle: string;
  procedurePath: string;
  procedureName: string;
}

declare type ScannedApplications = Array<ScannedApplication>;
declare type ApplicationScannerCallback = (sa: ScannedApplications) => void;

declare interface NativeApplicationScannerModule {
  ListenForApplications: (callback: ApplicationScannerCallback) => void;
  StopListener: () => void;
}

declare interface ApplicationInfo {
    applicationName: string;
    windowTitle: string;
}

declare interface WindowDimensions {
    x: int;
    y: int;
    width: int;
    height: int;
}

declare interface ApplicationStatus {
    status: string;
    dimensions?: WindowDimensions;
}

declare type ApplicationStatusCallback = (sa: ApplicationInfo, sa: ApplicationStatus) => void;

declare interface NativeApplicationStatusModule {
    ListenForStatus: (callback: ApplicationStatusCallback) => void;
    StopListener: () => void;
}

////

// Native Modules

declare type NativeModuleMap = {
    "native-application-scanner": NativeApplicationScannerModule,
    "native-application-status": NativeApplicationStatusModule
};

declare type NativeModuleTypes = keyof NativeModuleMap;

