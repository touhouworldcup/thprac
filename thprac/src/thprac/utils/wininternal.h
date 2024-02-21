#pragma once

#define read_fs_byte(offset) __readfsbyte(offset)
#define read_fs_word(offset) __readfsword(offset)
#define read_fs_dword(offset) __readfsdword(offset)
#define write_fs_byte(offset, data) __writefsbyte(offset, data)
#define write_fs_word(offset, data) __writefsword(offset, data)
#define write_fs_dword(offset, data) __writefsdword(offset, data)

#include <Windows.h>

/// Internal Windows Structs
/// Copied from thcrap
/*
Struct definitions based on the fields documented to have
consistent offsets in all Windoes versions 5.0+
TEB: https://www.geoffchappell.com/studies/windows/km/ntoskrnl/inc/api/pebteb/teb/index.htm
PEB: https://www.geoffchappell.com/studies/windows/km/ntoskrnl/inc/api/pebteb/peb/index.htm

Rest in Peace:
Geoff Chappell
*/

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
    PVOID ProcessParameters; // RTL_USER_PROCESS_PARAMETERS*
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
    union OSCSDVERSION {
        USHORT OSCSDVersion;
        struct OSCSDVERSION_BYTES {
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
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;
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
write_fs_dword(offsetof(TEB, member), (data)) \

#define CurrentTeb() ((TEB*)read_teb_member(Self))
#define CurrentPeb() ((PEB*)read_teb_member(ProcessEnvironmentBlock))

#define KernelSharedDataAddr (0x7FFE0000u)

#define CurrentImageBase ((uintptr_t)CurrentPeb()->ImageBaseAddress)

#define CurrentModuleHandle ((HMODULE)CurrentImageBase)

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

extern "C" __kernel_entry NTSTATUS
    NTAPI
    NtQueryInformationProcess(
        IN HANDLE ProcessHandle,
        IN PROCESSINFOCLASS ProcessInformationClass,
        OUT PVOID ProcessInformation,
        IN ULONG ProcessInformationLength,
        OUT PULONG ReturnLength OPTIONAL);