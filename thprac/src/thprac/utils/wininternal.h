#pragma once

#define read_fs_byte(offset) __readfsbyte(offset)
#define read_fs_word(offset) __readfsword(offset)
#define read_fs_dword(offset) __readfsdword(offset)
#define write_fs_byte(offset, data) __writefsbyte(offset, data)
#define write_fs_word(offset, data) __writefsword(offset, data)
#define write_fs_dword(offset, data) __writefsdword(offset, data)
#include "utils.h"
#include <Windows.h>
typedef LONG NTSTATUS;

constexpr NTSTATUS STATUS_BUFFER_OVERFLOW = 0x80000005;
constexpr NTSTATUS STATUS_NO_MORE_FILES = 0x80000006;
constexpr NTSTATUS STATUS_BUFFER_TOO_SMALL = 0xC0000023;

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

#pragma warning(disable : 4201)

/// Internal Windows Structs
/// Copied from thcrap
/*
Struct definitions based on the fields documented to have
consistent offsets in all Windows versions 5.0+
TEB: https://www.geoffchappell.com/studies/windows/km/ntoskrnl/inc/api/pebteb/teb/index.htm
PEB: https://www.geoffchappell.com/studies/windows/km/ntoskrnl/inc/api/pebteb/peb/index.htm

Rest in Peace:
Geoff Chappell
*/

struct UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
};

typedef UNICODE_STRING* PUNICODE_STRING;

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

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

#define InitializeObjectAttributes(p, n, a, r, s) do { \
        (p)->Length = sizeof(OBJECT_ATTRIBUTES);  \
        (p)->RootDirectory = r;                   \
        (p)->Attributes = a;                      \
        (p)->ObjectName = n;                      \
        (p)->SecurityDescriptor = s;              \
        (p)->SecurityQualityOfService = NULL;     \
    } while (0)

typedef struct _CURDIR {
    UNICODE_STRING DosPath;
    HANDLE Handle;
} CURDIR;

typedef struct _RTL_USER_PROCESS_PARAMETERS RTL_USER_PROCESS_PARAMETERS;
struct _RTL_USER_PROCESS_PARAMETERS {
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

typedef struct _PEB PEB;
struct _PEB {
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
    RTL_USER_PROCESS_PARAMETERS* ProcessParameters; // RTL_USER_PROCESS_PARAMETERS*
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
    VOID (*PostProcessInitRoutine)
    (VOID);
    PVOID TlsExpansionBitmap;
    ULONG TlsExpansionBitmapBits[0x20];
    ULONG SessionId;
#ifdef TH_X64
    UCHAR Padding5[4];
#endif
};
typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID;
typedef struct _GDI_TEB_BATCH {
    ULONG Offset;
    ULONG_PTR HDC;
    ULONG Buffer[310];
} GDI_TEB_BATCH, *PGDI_TEB_BATCH;
typedef struct _TEB TEB;
struct _TEB {
    // NT_TIB NtTib;
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
};

#define read_teb_member(member) (                                                                                                             \
    member_size(TEB, member) == 1 ? read_fs_byte(offsetof(TEB, member)) : member_size(TEB, member) == 2 ? read_fs_word(offsetof(TEB, member)) \
                                                                                                        : read_fs_dword(offsetof(TEB, member)))
#define write_teb_member(member, data) (\
member_size(TEB, member) == 1 ? write_fs_byte(offsetof(TEB, member), (data)) : \
member_size(TEB, member) == 2 ? write_fs_word(offsetof(TEB, member), (data)) : \
write_fs_dword(offsetof(TEB, member), (data))) \

#ifdef __INTELLISENSE__
#define CurrentPeb() ((PEB*)0)
#define CurrentTeb() ((TEB*)0)
#else
#define CurrentTeb() ((TEB*)read_teb_member(Self))
#define CurrentPeb() ((PEB*)read_teb_member(ProcessEnvironmentBlock))
#endif

#define KernelSharedDataAddr (0x7FFE0000u)

#define CurrentImageBase ((uintptr_t)CurrentPeb()->ImageBaseAddress)

#define CurrentModuleHandle ((HMODULE)CurrentImageBase)
#define CurrentProcessHandle ((HANDLE)(LONG_PTR)-1)


// Things from winternl.h.
// I can't include that header because it conflicts with this header

typedef struct _PROCESS_BASIC_INFORMATION {
    PVOID Reserved1;
    PVOID PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    PVOID Reserved3;
} PROCESS_BASIC_INFORMATION;
typedef PROCESS_BASIC_INFORMATION* PPROCESS_BASIC_INFORMATION;


typedef enum _PROCESSINFOCLASS {
    ProcessBasicInformation = 0,
    ProcessDebugPort = 7,
    ProcessWow64Information = 26,
    ProcessImageFileName = 27,
    ProcessBreakOnTermination = 29
} PROCESSINFOCLASS;

extern "C" NTSTATUS NTAPI NtQueryInformationProcess(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
);

// Completely undocumented data structure. Am I cooking too nasty?
struct SYSTEM_HANDLE_TABLE_ENTRY_INFO {
    // Geoff Chapell documents this as being a USHORT with another USHORT after it
    // Wine uses a ULONG for this. Which one is correct? Which one does my NT kernel use?
    // Maybe I should call NtQuerySystemInformation with SystemExtendedHandleInformation 
    // instead and not care about possible Windows 2000 support.
    ULONG UniqueProcessId;
    UCHAR ObjectTypeIndex;
    UCHAR HandleAttributes;
    USHORT HandleValue;
    PVOID Object;
    ULONG GrantedAccess;
};

struct SYSTEM_HANDLE_INFORMATION {
    ULONG NumberOfHandles;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
};

enum SYSTEM_INFORMATION_CLASS {
    SystemHandleInformation = 16,
};

extern "C" NTSTATUS NTAPI NtQuerySystemInformation(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
);


// KUSER_SHARED_DATA struct definition so that we don't have to depend on the WDK just for ntddk.h
// KUSER_SHARED_DATA itself comes from MSDN https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-kuser_shared_data
// The structs and enums used inside are not on MSDN (for some reason) do I got them from NirSoft
// https://www.nirsoft.net/kernel_struct/vista/KSYSTEM_TIME.html
// https://www.nirsoft.net/kernel_struct/vista/NT_PRODUCT_TYPE.html
// https://www.nirsoft.net/kernel_struct/vista/ALTERNATIVE_ARCHITECTURE_TYPE.html

typedef struct _KSYSTEM_TIME {
    ULONG LowPart;
    LONG High1Time;
    LONG High2Time;
} KSYSTEM_TIME, *PKSYSTEM_TIME;

typedef enum _NT_PRODUCT_TYPE {
    NtProductWinNt = 1,
    NtProductLanManNt = 2,
    NtProductServer = 3
} NT_PRODUCT_TYPE;

typedef enum _ALTERNATIVE_ARCHITECTURE_TYPE {
    StandardDesign = 0,
    NEC98x86 = 1,
    EndAlternatives = 2
} ALTERNATIVE_ARCHITECTURE_TYPE;

#define PROCESSOR_FEATURE_MAX 64

struct KUSER_SHARED_DATA {
    ULONG TickCountLowDeprecated;
    ULONG TickCountMultiplier;
    KSYSTEM_TIME InterruptTime;
    KSYSTEM_TIME SystemTime;
    KSYSTEM_TIME TimeZoneBias;
    USHORT ImageNumberLow;
    USHORT ImageNumberHigh;
    WCHAR NtSystemRoot[260];
    ULONG MaxStackTraceDepth;
    ULONG CryptoExponent;
    ULONG TimeZoneId;
    ULONG LargePageMinimum;
    ULONG AitSamplingValue;
    ULONG AppCompatFlag;
    ULONGLONG RNGSeedVersion;
    ULONG GlobalValidationRunlevel;
    LONG TimeZoneBiasStamp;
    ULONG NtBuildNumber;
    NT_PRODUCT_TYPE NtProductType;
    BOOLEAN ProductTypeIsValid;
    BOOLEAN Reserved0[1];
    USHORT NativeProcessorArchitecture;
    ULONG NtMajorVersion;
    ULONG NtMinorVersion;
    BOOLEAN ProcessorFeatures[PROCESSOR_FEATURE_MAX];
    ULONG Reserved1;
    ULONG Reserved3;
    ULONG TimeSlip;
    ALTERNATIVE_ARCHITECTURE_TYPE AlternativeArchitecture;
    ULONG BootId;
    LARGE_INTEGER SystemExpirationDate;
    ULONG SuiteMask;
    BOOLEAN KdDebuggerEnabled;
    union {
        UCHAR MitigationPolicies;
        struct {
            UCHAR NXSupportPolicy : 2;
            UCHAR SEHValidationPolicy : 2;
            UCHAR CurDirDevicesSkippedForDlls : 2;
            UCHAR Reserved : 2;
        };
    };
    USHORT CyclesPerYield;
    ULONG ActiveConsoleId;
    ULONG DismountCount;
    ULONG ComPlusPackage;
    ULONG LastSystemRITEventTickCount;
    ULONG NumberOfPhysicalPages;
    BOOLEAN SafeBootMode;
    union {
        UCHAR VirtualizationFlags;
        struct {
            UCHAR ArchStartedInEl2 : 1;
            UCHAR QcSlIsSupported : 1;
        };
    };
    UCHAR Reserved12[2];
    union {
        ULONG SharedDataFlags;
        struct {
            ULONG DbgErrorPortPresent : 1;
            ULONG DbgElevationEnabled : 1;
            ULONG DbgVirtEnabled : 1;
            ULONG DbgInstallerDetectEnabled : 1;
            ULONG DbgLkgEnabled : 1;
            ULONG DbgDynProcessorEnabled : 1;
            ULONG DbgConsoleBrokerEnabled : 1;
            ULONG DbgSecureBootEnabled : 1;
            ULONG DbgMultiSessionSku : 1;
            ULONG DbgMultiUsersInSessionSku : 1;
            ULONG DbgStateSeparationEnabled : 1;
            ULONG SpareBits : 21;
        } DUMMYSTRUCTNAME2;
    } DUMMYUNIONNAME2;
    ULONG DataFlagsPad[1];
    ULONGLONG TestRetInstruction;
    LONGLONG QpcFrequency;
    ULONG SystemCall;
    ULONG Reserved2;
    ULONGLONG FullNumberOfPhysicalPages;
    ULONGLONG SystemCallPad[1];
    union {
        KSYSTEM_TIME TickCount;
        ULONG64 TickCountQuad;
        struct {
            ULONG ReservedTickCountOverlay[3];
            ULONG TickCountPad[1];
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME3;
    ULONG Cookie;
    ULONG CookiePad[1];
    LONGLONG ConsoleSessionForegroundProcessId;
    ULONGLONG TimeUpdateLock;
    ULONGLONG BaselineSystemTimeQpc;
    ULONGLONG BaselineInterruptTimeQpc;
    ULONGLONG QpcSystemTimeIncrement;
    ULONGLONG QpcInterruptTimeIncrement;
    UCHAR QpcSystemTimeIncrementShift;
    UCHAR QpcInterruptTimeIncrementShift;
    USHORT UnparkedProcessorCount;
    ULONG EnclaveFeatureMask[4];
    ULONG TelemetryCoverageRound;
    USHORT UserModeGlobalLogger[16];
    ULONG ImageFileExecutionOptions;
    ULONG LangGenerationCount;
    ULONGLONG Reserved4;
    ULONGLONG InterruptTimeBias;
    ULONGLONG QpcBias;
    ULONG ActiveProcessorCount;
    UCHAR ActiveGroupCount;
    UCHAR Reserved9;
    union {
        USHORT QpcData;
        struct {
            UCHAR QpcBypassEnabled;
            UCHAR QpcReserved;
        };
    };
    LARGE_INTEGER TimeZoneBiasEffectiveStart;
    LARGE_INTEGER TimeZoneBiasEffectiveEnd;
    XSTATE_CONFIGURATION XState;
    KSYSTEM_TIME FeatureConfigurationChangeStamp;
    ULONG Spare;
    ULONG64 UserPointerAuthMask;
    XSTATE_CONFIGURATION XStateArm64;
    ULONG Reserved10[210];
};

#define Kuser_Shared_Data ((KUSER_SHARED_DATA*)0x7FFE0000)

extern "C" ULONG NTAPI RtlNtStatusToDosError(NTSTATUS Status);
extern "C" NTSTATUS NTAPI RtlSetCurrentDirectory_U(const UNICODE_STRING* Path);

extern "C" NTSTATUS NTAPI NtOpenProcessToken(HANDLE ProcessHandle, ACCESS_MASK DesiredAccess, PHANDLE TokenHandle);
extern "C" NTSTATUS NTAPI NtQueryInformationToken(HANDLE TokenHandle, TOKEN_INFORMATION_CLASS TokenInformationClass, PVOID TokenInformation, ULONG TokenInformationLength, PULONG ReturnLength);

typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation,
    KeyValueFullInformationAlign64,
    KeyValuePartialInformationAlign64,
    KeyValueLayerInformation,
    MaxKeyValueInfoClass
} KEY_VALUE_INFORMATION_CLASS;

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

extern "C" NTSYSAPI NTSTATUS NTAPI NtOpenKey(
    PHANDLE KeyHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes);

extern "C" NTSYSAPI NTSTATUS NTAPI NtQueryValueKey(
    HANDLE KeyHandle,
    UNICODE_STRING* ValueName,
    KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID KeyValueInformation,
    ULONG Length,
    PULONG ResultLength);

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
typedef FILE_INFORMATION_CLASS* PFILE_INFORMATION_CLASS;

struct FILE_NAMES_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    ULONG FileNameLength;
    WCHAR FileName[1];
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

struct FILE_FULL_DIR_INFORMATION {
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
    ULONG EaSize;
    WCHAR FileName[1];
};

struct IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };
    ULONG_PTR Information;
};

typedef IO_STATUS_BLOCK* PIO_STATUS_BLOCK;

typedef VOID NTAPI IO_APC_ROUTINE(
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG Reserved);

typedef IO_APC_ROUTINE PIO_APC_ROUTINE;

#define SL_RESTART_SCAN 0x00000001
#define SL_RETURN_SINGLE_ENTRY 0x00000002
#define SL_INDEX_SPECIFIED 0x00000004
#define SL_RETURN_ON_DISK_ENTRIES_ONLY 0x00000008
#define SL_NO_CURSOR_UPDATE_QUERY 0x00000010

extern "C" NTSTATUS NTAPI NtQueryDirectoryFileEx(
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    PVOID FileInformation,
    ULONG Length,
    FILE_INFORMATION_CLASS FileInformationClass,
    ULONG QueryFlags,
    PUNICODE_STRING FileName);