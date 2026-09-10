#pragma once
#include <Windows.h>
#include <type_traits>
#include <stdint.h>
#include "utils.h"

#pragma region Macros
#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

#define CurrentTeb() ((TEB*)read_teb_member(Self))
#define CurrentPeb() ((PEB*)read_teb_member(ProcessEnvironmentBlock))

#define read_teb_member(member) read_teb_value<decltype(TEB::member),offsetof(TEB,member)>()
#define write_teb_member(member, data) write_teb_value<decltype(TEB::member),offsetof(TEB,member)>(data)

#define CurrentImageBase ((uintptr_t)CurrentPeb()->ImageBaseAddress)
#define CurrentModuleHandle ((HMODULE)CurrentImageBase)
#define CurrentProcessHandle ((HANDLE)(LONG_PTR)-1)

#define Kuser_Shared_Data ((KUSER_SHARED_DATA*)0x7FFE0000u)

#define UNICODE_STRING_PARAM(str) (str).Buffer, (str).Length / sizeof((str).Buffer[0])

#define InitializeObjectAttributes(p, n, a, r, s) do { \
    (p)->Length = sizeof(OBJECT_ATTRIBUTES);           \
    (p)->RootDirectory = r;                            \
    (p)->Attributes = a;                               \
    (p)->ObjectName = n;                               \
    (p)->SecurityDescriptor = s;                       \
    (p)->SecurityQualityOfService = NULL;              \
} while (0)
#pragma endregion

#pragma region Typedefs
typedef LONG NTSTATUS;
typedef LONG KPRIORITY;
#pragma endregion

#pragma region Enums
constexpr NTSTATUS STATUS_BUFFER_OVERFLOW = 0x80000005;
constexpr NTSTATUS STATUS_NO_MORE_FILES = 0x80000006;
constexpr NTSTATUS STATUS_BUFFER_TOO_SMALL = 0xC0000023;

enum PROCESSINFOCLASS {
    ProcessBasicInformation = 0,
    ProcessWow64Information = 26,
    ProcessImageFileName = 27
};
enum SYSTEM_INFORMATION_CLASS {
    SystemProcessInformation = 5,
};
enum NT_PRODUCT_TYPE {
    NtProductWinNt = 1,
    NtProductLanManNt = 2,
    NtProductServer = 3
};

enum ALTERNATIVE_ARCHITECTURE_TYPE {
    StandardDesign = 0,
    NEC98x86 = 1,
    EndAlternatives = 2
};

enum FILE_INFORMATION_CLASS {
    FileDirectoryInformation = 1,
    FileFullDirectoryInformation = 2,
    FileBothDirectoryInformation = 3,
    FileBasicInformation = 4,
    FileStandardInformation = 5,
    FileInternalInformation = 6,
    FileEaInformation = 7,
    FileAccessInformation = 8,
    FileNameInformation = 9,
    FileRenameInformation = 10,
    FileLinkInformation = 11,
    FileNamesInformation = 12,
    FileDispositionInformation = 13,
    FilePositionInformation = 14,
    FileFullEaInformation = 15,
    FileModeInformation = 16,
    FileAlignmentInformation = 17,
    FileAllInformation = 18,
    FileAllocationInformation = 19,
    FileEndOfFileInformation = 20,
    FileAlternateNameInformation = 21,
    FileStreamInformation = 22,
    FilePipeInformation = 23,
    FilePipeLocalInformation = 24,
    FilePipeRemoteInformation = 25,
    FileMailslotQueryInformation = 26,
    FileMailslotSetInformation = 27,
    FileCompressionInformation = 28,
    FileObjectIdInformation = 29,
    FileCompletionInformation = 30,
    FileMoveClusterInformation = 31,
    FileQuotaInformation = 32,
    FileReparsePointInformation = 33,
    FileNetworkOpenInformation = 34,
    FileAttributeTagInformation = 35,
    FileTrackingInformation = 36,
    FileIdBothDirectoryInformation = 37,
    FileIdFullDirectoryInformation = 38,
    FileValidDataLengthInformation = 39,
    FileShortNameInformation = 40,
    FileIoCompletionNotificationInformation = 41,
    FileIoStatusBlockRangeInformation = 42,
    FileIoPriorityHintInformation = 43,
    FileSfioReserveInformation = 44,
    FileSfioVolumeInformation = 45,
    FileHardLinkInformation = 46,
    FileProcessIdsUsingFileInformation = 47,
    FileNormalizedNameInformation = 48,
    FileNetworkPhysicalNameInformation = 49,
    FileIdGlobalTxDirectoryInformation = 50,
    FileIsRemoteDeviceInformation = 51,
    FileUnusedInformation = 52,
    FileNumaNodeInformation = 53,
    FileStandardLinkInformation = 54,
    FileRemoteProtocolInformation = 55,
    FileRenameInformationBypassAccessCheck = 56,
    FileLinkInformationBypassAccessCheck = 57,
    FileVolumeNameInformation = 58,
    FileIdInformation = 59,
    FileIdExtdDirectoryInformation = 60,
    FileReplaceCompletionInformation = 61,
    FileHardLinkFullIdInformation = 62,
    FileIdExtdBothDirectoryInformation = 63,
    FileDispositionInformationEx = 64,
    FileRenameInformationEx = 65,
    FileRenameInformationExBypassAccessCheck = 66,
    FileDesiredStorageClassInformation = 67,
    FileStatInformation = 68,
    FileMemoryPartitionInformation = 69,
    FileStatLxInformation = 70,
    FileCaseSensitiveInformation = 71,
    FileLinkInformationEx = 72,
    FileLinkInformationExBypassAccessCheck = 73,
    FileStorageReserveIdInformation = 74,
    FileCaseSensitiveInformationForceAccessCheck = 75,
    FileKnownFolderInformation = 76,
    FileStatBasicInformation = 77,
    FileId64ExtdDirectoryInformation = 78,
    FileId64ExtdBothDirectoryInformation = 79,
    FileIdAllExtdDirectoryInformation = 80,
    FileIdAllExtdBothDirectoryInformation = 81,
    FileStreamReservationInformation,
    FileMupProviderInfo,
    FileMaximumInformation
};

enum KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation,
    KeyValueFullInformationAlign64,
    KeyValuePartialInformationAlign64,
    KeyValueLayerInformation,
    MaxKeyValueInfoClass
};
#pragma endregion

#pragma region Structs
struct UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
};
typedef UNICODE_STRING *PUNICODE_STRING;

struct CURDIR {
    UNICODE_STRING DosPath;
    HANDLE Handle;
};

struct CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
};

struct OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
};
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

struct IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };
    ULONG_PTR Information;
};
typedef IO_STATUS_BLOCK* PIO_STATUS_BLOCK;

struct RTL_USER_PROCESS_PARAMETERS {
    ULONG MaximumLength;
    ULONG Length;
    ULONG Flags;
    ULONG DebugFlags;
    HANDLE ConsoleHandle;
    ULONG ConsoleFlags;
    // 4 bytes of padding
    HANDLE StandardInput;
    HANDLE StandardOutput;
    HANDLE StandardError;
    CURDIR CurrentDirectory;
    UNICODE_STRING DllPath;
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
    PVOID Environment;
    ULONG StartingX;
    ULONG StartingY;
    ULONG CountX;
    ULONG CountY;
    ULONG CountCharsX;
    ULONG CountCharsY;
    ULONG FillAttribute;
    ULONG WindowFlags;
    ULONG ShowWindowFlags;
    // 4 bytes of padding
    UNICODE_STRING WindowTitle;
    UNICODE_STRING DesktopInfo;
    UNICODE_STRING ShellInfo;
    UNICODE_STRING RuntimeData;
};

struct PEB {
    BOOLEAN InheritedAddressSpace;
    BOOLEAN ReadImageFileExecOptions;
    BOOLEAN BeingDebugged;
    BOOLEAN SpareBool;
#ifdef TH_X64
    UCHAR Padding0[4];
#endif
    HANDLE Mutant;
    PVOID ImageBaseAddress;
    PVOID Ldr; // PEB_LDR_DATA*
    RTL_USER_PROCESS_PARAMETERS* ProcessParameters;
    PVOID SubSystemData;
    HANDLE ProcessHeap;
    RTL_CRITICAL_SECTION* FastPebLock;
    PVOID unreliable_member_1;
    PVOID unreliable_member_2;
    ULONG unreliable_member_3;
#ifdef TH_X64
    UCHAR Padding1[4];
#endif
    PVOID KernelCallbackTable;
    ULONG SystemReserved[2];
    PVOID unreliable_member_4;
    ULONG TlsExpansionCounter;
#ifdef TH_X64
    UCHAR Padding2[4];
#endif
    PVOID TlsBitmap;
    ULONG TlsBitmapBits[2];
    PVOID ReadOnlySharedMemoryBase;
    PVOID unreliable_member_5;
    PVOID* ReadOnlyStaticServerData;
    PVOID AnsiCodePageData;
    PVOID OemCodePageData;
    PVOID UnicodeCaseTableData;
    ULONG NumberOfProcessors;
    ULONG NtGlobalFlag;
    LARGE_INTEGER CriticalSectionTimeout;
    ULONG_PTR HeapSegmentReserve;
    ULONG_PTR HeapSegmentCommit;
    ULONG_PTR HeapDeCommitTotalFreeThreshold;
    ULONG_PTR HeapDeCommitFreeBlockThreshold;
    ULONG NumberOfHeaps;
    ULONG MaximumNumberOfHeaps;
    PVOID* ProcessHeaps;
    PVOID GdiSharedHandleTable;
    PVOID ProcessStarterHelper;
    ULONG GdiDCAttributeList;
#ifdef TH_X64
    UCHAR Padding3[4];
#endif
    RTL_CRITICAL_SECTION* LoaderLock;
    ULONG OSMajorVersion;
    ULONG OSMinorVersion;
    USHORT OSBuildNumber;
    union {
        USHORT OSCSDVersion;
        struct {
            BYTE OSCSDMajorVersion;
            BYTE OSCSDMinorVersion;
        };
    };
    ULONG OSPlatformId;
    ULONG ImageSubsystem;
    ULONG ImageSubsystemMajorVersion;
    ULONG ImageSubsystemMinorVersion;
#ifdef TH_X64
    UCHAR Padding4[4];
#endif
    KAFFINITY unreliable_member_6;
#ifdef TH_X64
    ULONG GdiHandleBuffer[0x3C];
#else
    ULONG GdiHandleBuffer[0x22];
#endif
    VOID(*PostProcessInitRoutine)(VOID);
    PVOID TlsExpansionBitmap;
    ULONG TlsExpansionBitmapBits[0x20];
    ULONG SessionId;
#ifdef TH_X64
    UCHAR Padding5[4];
#endif
};
typedef struct _GDI_TEB_BATCH {
    ULONG Offset;
    ULONG_PTR HDC;
    ULONG Buffer[310];
} GDI_TEB_BATCH, *PGDI_TEB_BATCH;

struct TEB {
    //NT_TIB NtTib;
    struct _EXCEPTION_REGISTRATION_RECORD* ExceptionList;
    PVOID StackBase;
    PVOID StackLimit;
    PVOID SubSystemTib;
    PVOID FiberData;
    PVOID ArbitraryUserPointer;
    TEB* Self;
    PVOID EnvironmentPointer;
    CLIENT_ID ClientId;
    PVOID ActiveRpcHandle;
    PVOID ThreadLocalStoragePointer;
    PEB* ProcessEnvironmentBlock;
    ULONG LastErrorValue;
    ULONG CountOfOwnedCriticalSections;
    PVOID CsrClientThread;
    PVOID Win32ThreadInfo;
    ULONG User32Reserved[0x1A];
    ULONG UserReserved[5];
    PVOID WOW32Reserved;
    ULONG CurrentLocale;
    ULONG FpSoftwareStatusRegister;
    PVOID SystemReserved1[0x36];
    LONG ExceptionCode;
#ifdef TH_X64
    UCHAR Padding0[4];
#endif
    UCHAR SpareBytes1[0x2C];
    GDI_TEB_BATCH GdiTebBatch;
    CLIENT_ID RealClientId;
    PVOID GdiCachedProcessHandle;
    ULONG GdiClientPID;
    ULONG GdiClientTID;
    PVOID GdiThreadLocalInfo;
    ULONG_PTR Win32ClientInfo[0x3E];
    PVOID glDispatchTable[0xE9];
    ULONG_PTR glReserved1[0x1D];
    PVOID glReserved2;
    PVOID glSectionInfo;
    PVOID glSection;
    PVOID glTable;
    PVOID glCurrentRC;
    PVOID glContext;
    ULONG LastStatusValue;
#ifdef TH_X64
    UCHAR Padding2[4];
#endif
    UNICODE_STRING StaticUnicodeString;
    union {
        WCHAR StaticUnicodeBuffer[MAX_PATH + 1];
        char StaticUTF8Buffer[(MAX_PATH + 1) * sizeof(WCHAR)];
    };
#ifdef TH_X64
    UCHAR Padding3[6];
#endif
    PVOID DeallocationStack;
    PVOID TlsSlots[0x40];
    LIST_ENTRY TlsLinks;
    PVOID Vdm;
    PVOID ReservedForNtRpc;
    HANDLE DbgSsReserved[2];
    ULONG HardErrorMode;
    PVOID Instrumentation[9];
    GUID ActivityId;
    PVOID SubProcessTag;
    PVOID EtwLocalData;
    PVOID EtwTraceData;
    PVOID WinSockData;
    ULONG GdiBatchCount;
    ULONG IdealProcessorValue;
    ULONG GuaranteedStackBytes;
    PVOID ReservedForPerf;
    PVOID ReservedForOle;
    ULONG WaitingOnLoaderLock;
    PVOID SavedPriorityState;
    ULONG_PTR SoftPatchPtr1;
    PVOID ThreadPoolData;
    PVOID* TlsExpansionSlots;
};

struct KSYSTEM_TIME {
    ULONG LowPart;
    LONG High1Time;
    LONG High2Time;
};

struct KUSER_SHARED_DATA {
    volatile ULONG TickCountLowDeprecated; // 0x0
    ULONG TickCountMultiplier; // 0x4
    volatile KSYSTEM_TIME InterruptTime; // 0x8
    volatile KSYSTEM_TIME SystemTime; // 0x14
    volatile KSYSTEM_TIME TimeZoneBias; // 0x20
    USHORT ImageNumberLow; // 0x2C
    USHORT ImageNumberHigh; // 0x2E

    // This is the path to the windows directory
    WCHAR NtSystemRoot[MAX_PATH]; // 0x30
    ULONG MaxStackTraceDepth; // 0x238
    ULONG CryptoExponent; // 0x23C
    ULONG TimeZoneId; // 0x240

    // Begin Server 2003+
    ULONG LargePageMinimum; // 0x244
    // End Server 2003+

    ULONG unreliable_members_1[6]; // 0x248

    // Begin Windows 10+
    ULONG NtBuildNumber; // 0x260
    // End Windows 10+

    NT_PRODUCT_TYPE NtProductType; // 0x264
    BOOLEAN ProductTypeIsValid; // 0x268

    // Begin Windows 8+
    BOOLEAN Reserved0[1]; // 0x269
    USHORT NativeProcessorArchitecture; // 0x26A
    // End Windows 8+

    ULONG NtMajorVersion; // 0x26C
    ULONG NtMinorVersion; // 0x270
    BOOLEAN ProcessorFeatures[0x40]; // 0x274

    // Begin Windows NT 4 specific (Still have values in later versions, but are almost useless with only 32 bits each)
    ULONG MmHighestUserAddress; // 0x2B4
    ULONG MmSystemRangeStart; // 0x2B8
    // End Windows NT 4 specific

    volatile ULONG TimeSlip; // 0x2BC
    ALTERNATIVE_ARCHITECTURE_TYPE AlternativeArchitecture; // 0x2C0

    // Begin Windows 10+
    ULONG BootId; // 0x2C4
    // End Windows 10+

    LARGE_INTEGER SystemExpirationDate; // 0x2C8
    ULONG SuiteMask; // 0x2D0
    BOOLEAN KdDebuggerEnabled; // 0x2D4
    union {
        UCHAR MitigationPolicies; // 0x2D5
        struct {
            UCHAR NXSupportPolicy : 2;
            // Begin Windows 8+
            UCHAR SEHValidationPolicy : 2;
            UCHAR CurDirDevicesSkippedForDlls : 2;
            UCHAR Reserved : 2;
            // End Windows 8+
        };
    };

    // Begin Windows 10+
    USHORT CyclesPerYield; // 0x2D6
    // End Windows 10+

    volatile ULONG ActiveConsoleId; // 0x2D8
    volatile ULONG DismountCount; // 0x2DC
    ULONG ComPlusPackage; // 0x2E0
    ULONG LastSystemRITEventTickCount; // 0x2E4
    ULONG NumberOfPhysicalPages; // 0x2E8
    BOOLEAN SafeBootMode; // 0x2EC
    UCHAR unreliable_member_2; // 0x2ED
    UCHAR Reserved12[2]; // 0x2EE
    ULONG unreliable_member_3; // 0x2F0
    ULONG DataFlagsPad[1]; // 0x2F4

    ULONGLONG TestRetInstruction; // 0x2F8
    ULONG unreliable_members_4[4]; // 0x300
    ULONGLONG SystemCallPad[2]; // 0x310

    union {
        volatile KSYSTEM_TIME TickCount; // 0x320
        volatile ULONG64 TickCountQuad; // 0x320
        struct {
            ULONG ReservedTickCountOverlay[3]; // 0x320
            ULONG TickCountPad[1]; // 0x32C
        };
    };
    ULONG Cookie; // 0x330
    ULONG CookiePad[1]; // 0x334
};

struct SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    BYTE Reserved1[48];
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    UINT_PTR UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    PVOID Reserved3;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG Reserved4;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    PVOID Reserved5;
    SIZE_T QuotaPagedPoolUsage;
    PVOID Reserved6;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER Reserved7[6];
};

struct PROCESS_BASIC_INFORMATION {
    NTSTATUS ExitStatus;
#ifdef TH_X64
    UCHAR Padding0[4];
#endif
    PEB* PebBaseAddress;
    ULONG_PTR AffinityMask;
    KPRIORITY BasePriority;
#ifdef TH_X64
    UCHAR Padding1[4];
#endif
    ULONG_PTR UniqueProcessId;
    ULONG_PTR InheritedFromUniqueProcessId;
};

struct FILE_DIRECTORY_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    WCHAR FileName[1];
};

// Since we use our own header for NT API functions and structs anyways, might as well change these definitions to be better.
template <typename D = UCHAR[1]>
struct KEY_VALUE_PARTIAL_INFORMATION {
    ULONG TitleIndex;
    ULONG Type;
    ULONG DataLength;
    D Data;
};

template <typename D = UCHAR[1]>
struct KEY_VALUE_PARTIAL_INFORMATION_ALIGN64 {
    ULONG Type;
    ULONG DataLength;
    D Data;
};

template <typename D = WCHAR[1]>
struct KEY_VALUE_FULL_INFORMATION {
    ULONG TitleIndex;
    ULONG Type;
    ULONG DataOffset;
    ULONG DataLength;
    ULONG NameLength;
    D Name;
};

template <typename D = WCHAR[1]>
struct KEY_VALUE_BASIC_INFORMATION {
    ULONG TitleIndex;
    ULONG Type;
    ULONG NameLength;
    D Name;
};

#pragma endregion

#pragma region Helper functions
template<typename T, size_t offset, typename R = std::conditional_t<sizeof(T) == sizeof(uint8_t) || sizeof(T) == sizeof(uint16_t) || sizeof(T) == sizeof(uint32_t) || sizeof(T) == sizeof(uint64_t), T, T&>>
static inline R read_teb_value() {
    if constexpr (sizeof(T) == sizeof(uint8_t)) {
        uint8_t temp = read_teb_byte(offset);
        return *(T*)&temp;
    } else if constexpr (sizeof(T) == sizeof(uint16_t)) {
        uint16_t temp = read_teb_word(offset);
        return *(T*)&temp;
    } else if constexpr (sizeof(T) == sizeof(uint32_t)) {
        uint32_t temp = read_teb_dword(offset);
        return *(T*)&temp;
    } else if constexpr (sizeof(T) == sizeof(uint64_t)) {
        uint64_t temp = read_teb_qword(offset);
        return *(T*)&temp;
    } else {
        T& ret = *(T*)((uintptr_t)CurrentTeb() + offset);
        return ret;
    }
}

template<typename T, size_t offset>
static inline void write_teb_value(const T& value) {
    if constexpr (sizeof(T) == sizeof(uint8_t)) {
        write_teb_byte(offset, *(uint8_t*)&value);
    } else if constexpr (sizeof(T) == sizeof(uint16_t)) {
        write_teb_word(offset, *(uint16_t*)&value);
    } else if constexpr (sizeof(T) == sizeof(uint32_t)) {
        write_teb_dword(offset, *(uint32_t*)&value);
    } else if constexpr (sizeof(T) == sizeof(uint64_t)) {
        write_teb_qword(offset, *(uint64_t*)&value);
    } else {
        *(T*)((uintptr_t)CurrentTeb() + offset) = value;
    }
}

static inline constexpr UNICODE_STRING MakeUnicodeString(const wchar_t* str, size_t length) {
    if (length > 0x7FFEu) {
        length = 0x7FFEu;
    }
    length *= sizeof(wchar_t);
    return {
        (USHORT)length,
        (USHORT)(length + sizeof(wchar_t)),
        (PWSTR)str
    };
}

static inline constexpr UNICODE_STRING MakeUnicodeString(const wchar_t* str) {
    return MakeUnicodeString(str, t_strlen(str) + 1);
}

// Constexpr reimplementation of RtlInitUnicodeString
static inline constexpr void RtlInitUnicodeString(UNICODE_STRING* out, const wchar_t* str) {
    *out = MakeUnicodeString(str);
}

// %wZ is a printf format for UNICODE_STRING
static inline constexpr UNICODE_STRING operator""_wZ(const wchar_t* str, size_t length) {
    return MakeUnicodeString(str, length);
}

#pragma endregion

#pragma region Functions
typedef VOID NTAPI IO_APC_ROUTINE(
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG Reserved
);
extern "C" {
    NTSYSAPI ULONG NTAPI RtlNtStatusToDosError(
        NTSTATUS Status
    );

    NTSYSAPI NTSTATUS NTAPI RtlSetCurrentDirectory_U(
        const UNICODE_STRING* Path
    );

    NTSYSAPI NTSTATUS NTAPI NtOpenKey(
        PHANDLE KeyHandle,
        ACCESS_MASK DesiredAccess,
        POBJECT_ATTRIBUTES ObjectAttributes
    );

    NTSYSAPI NTSTATUS NTAPI NtQueryValueKey(
        HANDLE KeyHandle,
        UNICODE_STRING* ValueName,
        KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
        PVOID KeyValueInformation,
        ULONG Length,
        PULONG ResultLength
    );

    NTSYSAPI NTSTATUS NTAPI NtOpenProcessToken(
        HANDLE ProcessHandle, 
        ACCESS_MASK DesiredAccess, 
        PHANDLE TokenHandle
    );

    NTSYSAPI NTSTATUS NTAPI NtQueryInformationToken(
        HANDLE TokenHandle, 
        TOKEN_INFORMATION_CLASS TokenInformationClass, 
        PVOID TokenInformation, 
        ULONG TokenInformationLength, 
        PULONG ReturnLength
    );
    
    NTSYSAPI NTSTATUS NTAPI NtQueryInformationProcess(
        HANDLE ProcessHandle,
        PROCESSINFOCLASS ProcessInformationClass,
        PVOID ProcessInformation,
        ULONG ProcessInformationLength,
        PULONG ReturnLength
    );

    NTSYSAPI NTSTATUS NTAPI NtQuerySystemInformation(
        SYSTEM_INFORMATION_CLASS SystemInformationClass,
        PVOID SystemInformation,
        ULONG SystemInformationLength,
        PULONG ReturnLength
    );
    NTSYSAPI NTSTATUS NTAPI NtQueryDirectoryFile(
        HANDLE                 FileHandle,
        HANDLE                 Event,
        IO_APC_ROUTINE* ApcRoutine,
        PVOID                  ApcContext,
        PIO_STATUS_BLOCK       IoStatusBlock,
        PVOID                  FileInformation,
        ULONG                  Length,
        FILE_INFORMATION_CLASS FileInformationClass,
        BOOLEAN                ReturnSingleEntry,
        PUNICODE_STRING        FileName,
        BOOLEAN                RestartScan
    );
}
#pragma endregion
