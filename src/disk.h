/**
 * Copyright (C) 2009, Shao Miller <shao.miller@yrdsb.edu.on.ca>.
 * Copyright 2006-2008, V.
 * For WinAoE contact information, see http://winaoe.org/
 *
 * This file is part of WinVBlock, derived from WinAoE.
 *
 * WinVBlock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * WinVBlock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WinVBlock.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _DISK_H
#  define _DISK_H

/**
 * @file
 *
 * Disk specifics
 *
 */

#  include "aoedisk.h"
#  include "ramdisk.h"

enum DISK_DISKTYPE
{
	FloppyDisk,
	HardDisk,
	OpticalDisc
};

typedef struct _DISK_DISK
{
	PDEVICE_OBJECT Parent;
	driver__dev_ext_ptr Next;
	KEVENT SearchEvent;
	driver__search_state SearchState;
	KSPIN_LOCK SpinLock;
	winvblock__bool BootDrive;
	winvblock__bool Unmount;
	ULONG DiskNumber;
	winvblock__bool IsRamdisk;
	winvblock__uint32 DiskType;
	winvblock__bool STDCALL (
	*Initialize
	 ) (
	IN driver__dev_ext_ptr DeviceExtension
	 );
	union
	{
		AOEDISK_AOEDISK AoE;
		RAMDISK_RAMDISK RAMDisk;
	};
	LONGLONG LBADiskSize;
	LONGLONG Cylinders;
	ULONG Heads;
	ULONG Sectors;
	winvblock__uint32 SectorSize;
	ULONG SpecialFileCount;
} DISK_DISK,
*PDISK_DISK;

#endif													/* _DISK_H */
