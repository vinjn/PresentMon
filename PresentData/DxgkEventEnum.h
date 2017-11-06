#pragma once

namespace dxgk {
    enum {
        HighPagingLevel = 1,
        ExcessiveMemoryTransfer = 2,
        task_0 = 3,
        VSyncDPC = 17,
        WorkerThreadStart = 18,
        WorkerThreadStop = 19,
        UpdateContextStatus = 20,
        ChangePriority = 21,
        AttemptPreemption = 22,
        SelectContext = 23,
        AdapterStart = 24,
        AdapterStop = 25,
        AdapterDCStart = 26,
        DeviceStart_V1 = 27,
        DeviceStop_V1 = 28,
        DeviceDCStart_V1 = 29,
        ContextStart = 30,
        ContextStop = 31,
        ContextDCStart = 32,
        AdapterAllocationStart_V2 = 33,
        AdapterAllocationStop_V2 = 34,
        AdapterAllocationDCStart_V2 = 35,
        DeviceAllocationStart_V1 = 36,
        DeviceAllocationStop_V1 = 37,
        DeviceAllocationDCStart_V1 = 38,
        TerminateAllocation = 39,
        ProcessTerminateAllocation = 40,
        Lock = 41,
        Unlock = 42,
        ReferenceAllocations = 43,
        PatchLocationList = 45,
        SetAllocationPriority = 46,
        MigrateAllocationStart = 47,
        MigrateAllocation = 48,
        MigrateAllocationStop = 49,
        MemoryTransfer = 50,
        ApertureMapping = 51,
        ApertureUnmapping = 52,
        PagingOpTransfer = 53,
        PagingOpFill = 54,
        PagingOpDiscard = 55,
        PagingOpReadPhysical = 56,
        PagingOpWritePhysical = 57,
        PagingOpMapApertureSegment = 58,
        PagingOpUnmapApertureSegment = 59,
        PagingOpSpecialLockTransfer = 60,
        PreparationStart = 61,
        Preparation = 62,
        PreparationStop = 63,
        RenameAllocationStart = 64,
        RenameAllocationStop = 65,
        SkipSegment = 66,
        ReserveResourceStart = 67,
        ReserveResourceStop = 68,
        InnerIterationStart = 69,
        InnerIterationStop = 70,
        AllocationFault = 71,
        MarkAllocation = 72,
        PageInAllocation = 73,
        EvictAllocation = 74,
        Unreset = 75,
        AddDmaBufferStart = 76,
        AddDmaBufferStop = 77,
        ReportSegment_V1 = 78,
        ReportCommittedAllocation = 80,
        ProcessPolicyChange = 81,
        DmaPoolTrimmingPolicy = 82,
        SetProcessWorkingSet = 83,
        SynchronizationMutexStart = 84,
        SynchronizationMutex = 85,
        SynchronizationMutexStop = 86,
        SynchronizationMutexDCStart = 87,
        SemaphoreStart = 88,
        Semaphore = 89,
        SemaphoreStop = 90,
        SemaphoreDCStart = 91,
        FenceStart = 92,
        Fence = 93,
        FenceStop = 94,
        FenceDCStart = 95,
        CPUNotificationStart = 96,
        CPUNotification = 97,
        CPUNotificationStop = 98,
        CPUNotificationDCStart = 99,
        WaitForSynchronizationObject = 100,
        SignalSynchronizationObject = 101,
        SetDisplayMode = 102,
        BlockThread = 103,
        PerformanceWarning = 104,
        ProfilerStart = 105,
        ProfilerStop = 106,
        ExtendedProfilerStart = 107,
        ExtendedProfilerStop = 108,
        SetPointerPosition = 109,
        DpiReportAdapter_V1 = 110,
        InvalidateModeCache = 111,
        RebuildModeCacheStart = 112,
        UpdateContextRunningTime = 113,
        RebuildModeCacheStop = 115,
        MMIOFlip = 116,
        SegmentPolicyChange = 117,
        DpiAddDeviceStart = 118,
        DpiAddDeviceStop = 119,
        DpiDriverUnloadStart = 120,
        DpiDriverUnloadStop = 121,
        DpiDispatchCreateStart = 122,
        DpiDispatchCreateStop = 123,
        DpiDispatchCloseStart = 124,
        DpiDispatchCloseStop = 125,
        DpiDispatchPnpStart = 126,
        DpiDispatchPnpStop = 127,
        DpiDispatchPowerStart = 128,
        DpiDispatchPowerStop = 129,
        DpiDispatchIoctlStart = 130,
        DpiDispatchIoctlStop = 131,
        DpiDispatchInternalIoctlStart = 132,
        DpiDispatchInternalIoctlStop = 133,
        DpiDispatchSystemControlStart = 134,
        DpiDispatchSystemControlStop = 135,
        DdiAddDeviceStart = 136,
        DdiAddDeviceStop = 137,
        DdiStartDeviceStart = 138,
        DdiStartDeviceStop = 139,
        DdiStopDeviceStart = 140,
        DdiStopDeviceStop = 141,
        DdiRemoveDeviceStart = 142,
        DdiRemoveDeviceStop = 143,
        DdiDispatchIoRequestStart = 144,
        DdiDispatchIoRequestStop = 145,
        DdiProtectedCallbackStart = 146,
        DdiProtectedCallbackStop = 147,
        DdiQueryChildRelationsStart = 148,
        DdiQueryChildRelationsStop = 149,
        DdiQueryChildStatusStart = 150,
        DdiQueryChildStatusStop = 151,
        DdiQueryDeviceDescriptorStart = 152,
        DdiQueryDeviceDescriptorStop = 153,
        DdiSetPowerStateStart = 154,
        DdiSetPowerStateStop = 155,
        DdiNotifyAcpiEventStart = 156,
        DdiNotifyAcpiEventStop = 157,
        DdiUnloadStart = 158,
        DdiUnloadStop = 159,
        DdiQueryInterfaceStart = 160,
        DdiQueryInterfaceStop = 161,
        DdiLinkDeviceStart = 162,
        DdiLinkDeviceStop = 163,
        EtwVersionStop = 164,
        ExternalEventStop = 165,
        Blit = 166,
        BlitRect = 167,
        Flip = 168,
        Render = 169,
        RenderKm = 170,
        PresentHistoryStart = 171,
        PresentHistory = 172,
        PresentHistoryStop = 173,
        ACPIEvent = 174,
        DmaPacketStart_V1 = 175,
        DmaPacketStop = 176,
        DmaPacket_V1 = 177,
        QueuePacketStart = 178,
        QueuePacket = 179,
        QueuePacketStop = 180,
        VSyncInterrupt = 181,
        GetDeviceState = 182,
        GetDevicePresentState = 183,
        Present = 184,
        RecordQueuePacketTimestamps = 185,
        OfferAllocationStart = 186,
        OfferAllocation = 187,
        OfferAllocationStop = 188,
        ReportOfferAllocation = 189,
        ReclaimAllocation = 190,
        DiscardAllocation = 191,
        Dxgk_Diag_OutputDupl_Connect = 192,
        Dxgk_Diag_OutputDupl_Destroy = 193,
        Dxgk_Diag_OutputDupl_Process_Present = 194,
        Dxgk_Diag_OutputDupl_Process_Pointer_Shape = 195,
        Dxgk_Diag_OutputDupl_Process_Pointer_Position = 196,
        Dxgk_Diag_OutputDupl_High_Level_Update_Start = 197,
        Dxgk_Diag_OutputDupl_High_Level_Update_Stop = 198,
        Dxgk_Diag_OutputDupl_Get_Frame_Data = 199,
        Dxgk_Diag_OutputDupl_Get_Meta_Data = 200,
        Dxgk_Diag_OutputDupl_Get_Meta_Data201 = 201,
        Dxgk_Diag_OutputDupl_Get_Pointer_Shape_Data = 202,
        RuntimePowerManagementStart = 203,
        RuntimePowerManagementStop = 204,
        RuntimePowerManagementStart205 = 205,
        RuntimePowerManagementStop206 = 206,
        RuntimePowerManagementStart207 = 207,
        RuntimePowerManagementStop208 = 208,
        RuntimePowerManagement = 209,
        DdiStopDeviceAndReleasePostDisplayOwnershipStart = 210,
        DdiStopDeviceAndReleasePostDisplayOwnershipStop = 211,
        Dxgk_Diag_OutputDupl_Release_Frame = 212,
        RuntimePowerManagement214_V1 = 214,
        PresentHistoryDetailedStart = 215,
        BlockGpuAccess = 216,
        DelayContextScheduling = 217,
        DdiGetChildContainerIdStart = 218,
        DdiGetChildContainerIdStop = 219,
        RuntimePowerManagement220 = 220,
        RuntimePowerManagement221 = 221,
        RuntimePowerManagement222 = 222,
        RuntimePowerManagement223 = 223,
        ProcessAllocationStart_V1 = 225,
        ProcessAllocationStop_V1 = 226,
        ReportCommittedGlobalAllocationDCStart = 227,
        RuntimePowerManagement228 = 228,
        RuntimePowerManagement229 = 229,
        SignalSynchronizationObject2 = 230,
        WaitForSynchronizationObject2 = 231,
        RuntimePowerManagementStart232 = 232,
        RuntimePowerManagementStop233 = 233,
        RuntimePowerManagement234 = 234,
        RuntimePowerManagement235 = 235,
        RuntimePowerManagement236 = 236,
        RuntimePowerManagement237 = 237,
        UnwaitQueuePacket = 238,
        ChangeInProcessPriority = 239,
        DdiNotifySurpriseRemovalStart = 240,
        DdiNotifySurpriseRemovalStop = 241,
        ResetBackingStore = 242,
        RuntimePowerManagement243 = 243,
        QueuePacketStart244 = 244,
        QueuePacketStart245_V1 = 245,
        GpuWork = 246,
        VSyncStart = 247,
        VSyncStop = 248,
        AllocationTerminationListTimer = 249,
        NodeMetadata = 250,
        PresentMultiPlaneOverlay = 251,
        FlipMultiPlaneOverlay = 252,
        RuntimePowerManagement253 = 253,
        RuntimePowerManagement254 = 254,
        RuntimePowerManagement255 = 255,
        RuntimePowerManagement256 = 256,
        RuntimePowerManagement257 = 257,
        RuntimePowerManagement258 = 258,
        MMIOFlipMultiPlaneOverlay = 259,
        DpiFdoThermalActiveCooling = 260,
        DpiFdoThermalPassiveCooling = 261,
        CalibrateGpuClockTask = 262,
        HistoryBuffer = 263,
        YieldDueToStarvationThresholdExceeded = 264,
        ScheduleHighestReadyPriorityLevel = 265,
        IndependentFlip_V1 = 266,
        IndependentFlipTransition_V1 = 267,
        CancelFlip = 268,
        RuntimePowerManagement269 = 269,
        RuntimePowerManagement270 = 270,
        DxgkSetPresenterViewMode = 271,
        DxgkCbIndicateChildStatus = 272,
        VSyncDPCMultiPlane = 273,
        TotalBytesResidentInSegment = 274,
        Brightness = 275,
        BacklightOptimizationLevel = 276,
        VidMmDereferenceObjectAsyncStart = 277,
        VidMmDereferenceObjectAsyncStop = 278,
        VidMmUnmapViewAsyncStart = 279,
        VidMmUnmapViewAsyncStop = 280,
        PagingPreparationStart = 281,
        PagingPreparationStop = 282,
        RuntimePowerManagement283 = 283,
        CddStandardAllocation = 284,
        CddStandardAllocation285 = 285,
        CddStandardAllocation286 = 286,
        CddStandardAllocation287 = 287,
        ProcessAllocationDetailsStart = 288,
        ProcessAllocationDetailsStop = 289,
        MonitoredFenceStart = 290,
        MonitoredFence = 291,
        MonitoredFenceStop = 292,
        MonitoredFenceDCStart = 293,
        SignalSynchronizationObjectFromGpu = 294,
        WaitForSynchronizationObjectFromGpu = 295,
        SignalSynchronizationObjectFromCpu = 296,
        WaitForSynchronizationObjectFromCpu = 297,
        CreateSyncPoint = 298,
        RetireSyncPoint = 299,
        UnwaitCpuWaiter = 300,
        RecycleRangeTracking = 301,
        RecycleRangeTracking302 = 302,
        RecycleRangeTracking303 = 303,
        UpdateVPR = 304,
        ReferenceWrittenPrimaries = 305,
        PagingOpVirtualTransfer = 306,
        PagingOpVirtualFill = 307,
        PagingOpInitContextResource = 308,
        PagingOpUpdatePageTable = 309,
        PagingOpFlushTlb = 310,
        PagingOpUpdateContextAllocation = 311,
        PagingOpNotifyResidency = 312,
        PagingOpSysmemCommit = 313,
        PagingOpSysmemUncommit = 314,
        CheckMultiPlaneOverlaySuccess = 315,
        CheckMultiPlaneOverlayFail = 316,
        CheckMultiPlaneOverlaySupportPlane = 317,
        DWMVsyncCountWait = 318,
        DWMVsyncSignal = 319,
        VidMmMakeResident = 320,
        VidMmEvict = 321,
        PagingQueuePacketStart_V1 = 322,
        PagingQueuePacketStart323_V1 = 323,
        PagingQueuePacket = 324,
        PagingQueuePacketStop = 325,
        ClearFlipDevice = 326,
        CommitVirtualAddressStart = 327,
        CommitVirtualAddressStop = 328,
        SetProcessStatus = 329,
        GpuVirtualAddressRangeDCStart_V1 = 330,
        GpuVirtualAddressRangeStart_V1 = 331,
        GpuVirtualAddressRangeStop_V1 = 332,
        GpuVirtualAddressRangeMappingDCStart_V1 = 333,
        GpuVirtualAddressRangeMapping_V1 = 334,
        GpuVirtualAddressAllocatorDCStart = 335,
        GpuVirtualAddressAllocatorStart = 336,
        GpuVirtualAddressAllocatorStop = 337,
        MakeResidentStart = 338,
        MakeResidentStop = 339,
        Lock340 = 340,
        Unlock341 = 341,
        HardwareContentProtectionTeardown = 342,
        Profiler = 343,
        ExtendedProfiler = 344,
        VidMmSelectOperation = 346,
        VidMmTryOperation = 347,
        VidMmProfilerStart = 348,
        VidMmProfilerStop = 349,
        VidMmSuspendDevice = 350,
        VidMmDemoteDevice = 351,
        VidMmYieldDevice = 352,
        VidMmResumeDevice = 353,
        YieldStartAdapter = 354,
        YieldStartNode = 355,
        YieldStopNode = 356,
        YieldSetExpirationTimer = 357,
        YieldCancelExpirationTimer = 358,
        YieldConditionEvaluation = 359,
        FlushScheduler = 360,
        AbortQueuePacket = 361,
        VidMmWorkerThreadWake = 362,
        YieldAccumulateRunningTime = 363,
        LockAllocationBackingStore = 364,
        UnlockAllocationBackingStore = 365,
        VidMmProcessBudgetChange = 366,
        VidMmProcessUsageChange = 367,
        PowerRequestDuringD3 = 368,
        GdiRenderDuringCS = 369,
        VidMmProcessDemotedCommitmentChange = 370,
        VidMmProcessCommitmentChange = 371,
        DxgkDdiMiracastQueryCapsStart = 1000,
        DxgkDdiMiracastQueryCapsStop = 1001,
        DxgkDdiMiracastCreateContextStart = 1002,
        DxgkDdiMiracastCreateContextStop = 1003,
        DxgkDdiMiracastIoControlStart = 1004,
        DxgkDdiMiracastIoControlStop = 1005,
        DxgkDdiMiracastDestroyContextStart = 1006,
        DxgkDdiMiracastDestroyContextStop = 1007,
        DxgkCbSendUserModeMessageStart = 1008,
        DxgkCbSendUserModeMessageStop = 1009,
        MiracastStartMiracastSession = 1010,
        MiracastStopMiracastSession = 1011,
        MiracastSendUserModeRequestStart = 1012,
        MiracastSendUserModeRequestStop = 1013,
        MiracastSendUserModeRequest = 1014,
        MiracastStartMiracastSessionStage = 1015,
        MiracastStopMiracastSessionStage = 1016,
        MiracastDeviceStateChange = 1017,
        MiracastInterrupt = 1018,
        MiracastDPC = 1019,
        MiracastUmdDriverCreateMiracastContextStart = 1020,
        MiracastUmdDriverCreateMiracastContextStop = 1021,
        MiracastUmdDriverDestroyMiracastContextStart = 1022,
        MiracastUmdDriverDestroyMiracastContextStop = 1023,
        MiracastUmdDriverStartMiracastSessionStart = 1024,
        MiracastUmdDriverStartMiracastSessionStop = 1025,
        MiracastUmdDriverStopMiracastSessionStart = 1026,
        MiracastUmdDriverStopMiracastSessionStop = 1027,
        MiracastUmdDriverHandleKernelModeMessageStart = 1028,
        MiracastUmdDriverHandleKernelModeMessageStop = 1029,
        MiracastUmdDriverCbReportSessionStatus = 1030,
        MiracastUmdDriverCbMiracastIoControl = 1031,
        MiracastUmdDriverCbGetNextChunkDataStart = 1032,
        MiracastUmdDriverCbGetNextChunkDataStop = 1033,
        MiracastUmdDriverCbRegisterForDataRateNotifications = 1034,
        MiracastUmdReportEncodeComplete = 1035,
        MiracastUmdReportChunkSent = 1036,
        MiracastUmdReportProtocolEvent = 1037,
        MiracastUmdSendMiracastEscapeStart = 1038,
        MiracastUmdSendMiracastEscapeStop = 1039,
        DxgkMiracastCbReportChunkInfo = 1040,
        MiracastUmdQoSDataRate = 1041,
        MiracastUmdOpenMiracastSessionDataRate = 1042,
        MiracastStartMiracastSessionFail = 1043,
        MiracastDetectedUmdDriverWatchdogTimeOut = 1044,
        MiracastStopMiracastSessionAbnormal = 1045,
        MiracastPerfTrackStartMiracastSessionStart = 1046,
        MiracastPerfTrackStartMiracastSessionDoneStop = 1047,
        MiracastPerfTrackStartMiracastSessionDoneNoMonitorStop = 1048,
        MiracastPerfTrackStartMiracastSessionFailedStop = 1049,
        MiracastPerfTrackSourceDroppedFrames = 1050,
        MiracastPerfTrackGraphicsLatency = 1051,
        MiracastPerfTrackIFrameRequest = 1052,
        MiracastGlobalConfiguration = 1053,
        MiracastChunkReportViolation = 1054,
        MiracastPerfTrackStartMiracastSessionNoSendStop = 1055,
        IndirectSwapChainCreate = 1056,
        IndirectSwapChainOpen = 1057,
        IndirectSwapChainSurfaceList = 1058,
        IndirectSwapChainCloseHandle = 1059,
        IndirectSwapChainDestroy = 1060,
        IndirectSwapChainMarkAbandoned = 1061,
        IndirectSwapChainAcquire = 1062,
        IndirectSwapChainRelease = 1063,
        IndirectSwapChainGetMetaData = 1064,
        IndirectSwapChainSetMetaData = 1065,
        BltQueueUpdatePresentStats = 1066,
        BltQueueUpdateVSyncState = 1067,
        BltQueueCompleteIndirectPresent = 1068,
        ReportSyncObject = 1069,
        ReportSyncObjectStart = 1070,
        BltQueueEntryStart = 1071,
        BltQueueEntryStop = 1072,
        OutputDuplicationCreate = 1073,
        OutputDuplicationGetFrameInfo = 1074,
        OutputDuplicationReleaseFrame = 1075,
        OutputDuplicationProcessHighLevel = 1076,
        OutputDuplicationBlit = 1077,
        OutputDuplicationDestroy = 1078,
        OpmGetCertificate = 1079,
        OpmGetCertificateSize = 1080,
        OpmCreateProtectedOutput = 1081,
        OPMGetRandomNumber = 1082,
        OPMSetSigningKeyAndSequenceNumbers = 1083,
        OPMGetInformation = 1084,
        OPMGetCOPPCompatibleInformation = 1085,
        OPMConfigureProtectedOutput = 1086,
        OPMDestroyProtectedOutput = 1087,
        OPMSetProtectionLevel = 1088,
        OPMGetConnectorType = 1089,
        OPMGetSupportedProtectionTypes = 1090,
        OPMGetActualProtectionLevel = 1091,
        SetVidPnSourceVisibility = 1092,
        IndirectDisableRenderD3 = 1093,
        PerfTrack_DMM_Hotplug_Monitor_Set_VideoModeStart = 10000,
        PerfTrack_DMM_Hotplug_Monitor_Set_VideoModeStop = 10001,
        PerfTrack_DMM_Hotplug_Monitor_Set_VideoModeStop10002 = 10002,
        DMM_SnapPerfCounters = 10010,
        PerfTrack_Dxgkrnl_StateChangeNotify = 10011,
    };
}