//some proto's from Walker.h
// from http://forum.sysinternals.com/uploads/EPX0FF/2006-04-23_020246_proto_WalkObjects_debug.rar
// refer http://forum.sysinternals.com/forum_posts.asp?TID=3577&PN=2
#ifndef _WALKER_
#define _WALKER_

NTSTATUS (NTAPI *NtQuerySystemInformation)(UINT, PVOID, ULONG, PULONG);
NTSTATUS (NTAPI *NtQueryObject)(HANDLE, UINT, PVOID, ULONG, PULONG);
NTSTATUS (NTAPI *NtQueryInformationFile)(HANDLE, PIO_STATUS_BLOCK, PVOID, ULONG, FILE_INFORMATION_CLASS);
NTSTATUS (NTAPI *RtlAdjustPrivilege)(DWORD, BOOLEAN, ADJUST_PRIVILEGE_TYPE, PBOOLEAN);

typedef struct _HTT {
	HANDLE handle;
	char *txt;
} HTT, *PHTT;

//for enumerating handles
typedef struct _WALKER_HANDLE {
	ULONG ProcessId;
	UCHAR ObjectTypeNumber;
	UCHAR Flags;
	USHORT Handle;
	PVOID Object;
	ACCESS_MASK GrantedAccess;
} WALKER_HANDLE, *PWALKER_HANDLE;

//for enumerating objects
typedef struct _WALKER_OBJECT_INFORMATION {
	WALKER_HANDLE			 ohi;
	OBJECT_BASIC_INFORMATION obi;
    POBJECT_TYPE_INFORMATION oti;
	CHAR oni[1024];
} WALKER_OBJECT_INFORMATION, *PWALKER_OBJECT_INFORMATION;

typedef struct _HandlesEntry {
	ULONG HandlesCount;
	WALKER_HANDLE *Handles;
} HandlesEntry, *PHandlesEntry;

typedef struct _ObjectsEntry {
	ULONG ObjectsCount;
    WALKER_OBJECT_INFORMATION *Objects;
} ObjectsEntry, *PObjectsEntry;

#endif /*_WALKER_*/