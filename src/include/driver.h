/**
 * Copyright (C) 2009-2012, Shao Miller <sha0.miller@gmail.com>.
 * Copyright 2006-2008, V.
 * For WinAoE contact information, see http://winaoe.org/
 *
 * This file is part of WinVBlock, originally derived from WinAoE.
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
#ifndef WV_M_DRIVER_H_
#  define WV_M_DRIVER_H_

/**
 * @file
 *
 * Driver specifics.
 */

/* For testing and debugging */
#if 0
#  define RIS
#  define WVL_M_DEBUG_IRPS 1
#  define DEBUGMOSTPROTOCOLCALLS
#  define DEBUGALLPROTOCOLCALLS
#endif

#define POOLSIZE 2048

/** Object types */

typedef struct S_WVL_LOCKED_LIST S_WVL_LOCKED_LIST;

typedef struct S_WVL_RESOURCE_TRACKER S_WVL_RESOURCE_TRACKER;

typedef struct S_WVL_MINI_DRIVER S_WVL_MINI_DRIVER;

typedef struct WV_DEV_EXT WV_S_DEV_EXT, * WV_SP_DEV_EXT;

typedef struct S_WVL_DEVICE_THREAD_WORK_ITEM S_WVL_DEVICE_THREAD_WORK_ITEM;

/** Function types */

/**
 * The type of a function to be called within a device's thread context
 *
 * @param Device
 *   The device whose thread context the function will be called within
 *
 * @param Context
 *   Optional.  Additional context to pass to the function
 *
 * @return
 *   The status of the operation
 */
typedef NTSTATUS F_WVL_DEVICE_THREAD_FUNCTION(IN DEVICE_OBJECT *, IN VOID *);

/* Haven't found a better place for this, yet. */
extern NTSTATUS STDCALL WvDriverGetDevCapabilities(
    IN PDEVICE_OBJECT,
    IN PDEVICE_CAPABILITIES
  );

/** Function declarations */

/* From driver.c */
extern DRIVER_INITIALIZE DriverEntry;

/* From mainbus/mainbus.c */

/**
 * Return the main bus FDO.  This function is useful for
 * drivers/mini-drivers trying to communicate with this one
 *
 * @return
 *   A pointer to the main bus FDO
 */
extern WVL_M_LIB DEVICE_OBJECT * WvBusFdo(void);

/**
 * Add a child node to the main bus
 *
 * @param Device
 *   Points to the child device to add
 *
 * @retval TRUE
 *   Success
 * @retval FALSE
 *   Failure
 */
/* TODO: Fix WV_SP_DEV_T being unknown, here */
extern BOOLEAN STDCALL WvBusAddDev(IN WV_SP_DEV_T);

/** From disk.c */
extern NTSTATUS STDCALL WvDiskPnpQueryDevText(
    IN PDEVICE_OBJECT,
    IN PIRP,
    IN struct WVL_DISK_T *
  );

/**
 * Register a mini-driver
 *
 * @param MiniDriver
 *   Returns a mini-driver object for the registration.  This can later
 *   be used with WvlDeregisterMiniDriver().  This parameter is required
 *
 * @param DriverObject
 *   The driver object provided to the mini-driver by Windows.  This
 *   parameter is required
 *
 * @param AddDevice
 *   The mini-driver's AddDevice routine.  This parameter is optional
 *
 * @param Unload
 *   The mini-driver's Unload routine.  This routine must later call
 *   WvDeregisterMiniDriver with the mini-driver object received.
 *   This parameter is required
 *
 * @retval STATUS_SUCCESS
 * @retval STATUS_UNSUCCESSFUL
 *   An invalid parameter was passed or the library is not initialized
 * @retval STATUS_INSUFFICIENT_RESOURCES
 *   A mini-driver object could not be allocated
 */
extern WVL_M_LIB NTSTATUS WvlRegisterMiniDriver(
    OUT S_WVL_MINI_DRIVER ** MiniDriver,
    IN OUT DRIVER_OBJECT * DriverObject,
    IN DRIVER_ADD_DEVICE * AddDevice,
    IN DRIVER_UNLOAD * Unload
  );

/**
 * Deregister a mini-driver.  Called by a mini-driver's Unload routine
 *
 * @param MiniDriver
 *   The mini-driver to deregister.  After this function returns, the
 *   caller must not attempt to use the same pointer value
 */
extern WVL_M_LIB VOID WvlDeregisterMiniDriver(
    IN S_WVL_MINI_DRIVER * MiniDriver
  );

/**
 * Initialize an atomic list
 *
 * @param List
 *   The atomic list to initialize
 */
extern WVL_M_LIB VOID WvlInitializeLockedList(OUT S_WVL_LOCKED_LIST * List);

/**
 * Append an item to an atomic list
 *
 * @param List
 *   The atomic list to append a list item to
 *
 * @param Link
 *   The list item to append
 */
extern WVL_M_LIB VOID WvlAppendLockedListLink(
    IN OUT S_WVL_LOCKED_LIST * List,
    IN OUT LIST_ENTRY * Link
  );

/**
 * Remove an atomic list item from the list
 *
 * @param List
 *   The atomic list to remove the item from
 *
 * @param Link
 *   The item to remove
 *
 * @retval TRUE - The list is now empty
 * @retval FALSE - The list still has links
 */
extern WVL_M_LIB BOOLEAN WvlRemoveLockedListLink(
    IN OUT S_WVL_LOCKED_LIST * List,
    IN OUT LIST_ENTRY * Link
  );

/**
 * Initialize a resource tracker
 *
 * @param ResourceTracker
 *   The resource tracker to initialize
 */
extern WVL_M_LIB VOID WvlInitializeResourceTracker(
    S_WVL_RESOURCE_TRACKER * ResourceTracker
  );

/**
 * Wait for a resource to have zero usage
 *
 * @param ResourceTracker
 *   The tracker for the resource
 */
extern WVL_M_LIB VOID WvlWaitForResourceZeroUsage(
    S_WVL_RESOURCE_TRACKER * ResourceTracker
  );

/**
 * Increment the usage count for a resource
 *
 * @param ResourceTracker
 *   The tracker for the resource
 */
extern WVL_M_LIB VOID WvlIncrementResourceUsage(
    S_WVL_RESOURCE_TRACKER * ResourceTracker
  );

/**
 * Decrement the usage count for a resource
 *
 * @param ResourceTracker
 *   The tracker for the resource
 */
extern WVL_M_LIB VOID WvlDecrementResourceUsage(
    S_WVL_RESOURCE_TRACKER * ResourceTracker
  );

/**
 * Create a mini-driver device
 *
 * @param MiniDriver
 *   The mini-driver associated with the device
 *
 * @param DeviceExtensionSize
 *   The size of the device extension, in bytes.  This must be at least
 *   large enough to hold a WV_S_DEV_EXT, as the device extension for
 *   all mini-driver devices must point to a WV_S_DEV_EXT
 *
 * @param DeviceName
 *   Optional.  The name of the device
 *
 * @param DeviceType
 *   The device type
 *
 * @param DeviceCharacteristics
 *   Flags for the device characteristics
 *
 * @param Exclusive
 *   Determines whether or not to allow multiple handles to the device
 *
 * @param DeviceObject
 *   Points to the DEVICE_OBJECT pointer to populate with a pointer to
 *   the created mini-driver device
 *
 * @return
 *   The status of the operation
 *
 * Each mini-driver device has an associated thread
 */
extern WVL_M_LIB NTSTATUS STDCALL WvlCreateDevice(
    IN S_WVL_MINI_DRIVER * MiniDriver,
    IN ULONG DeviceExtensionSize,
    IN UNICODE_STRING * DeviceName,
    IN DEVICE_TYPE DeviceType,
    IN ULONG DeviceCharacteristics,
    IN BOOLEAN Exclusive,
    OUT DEVICE_OBJECT ** DeviceObject
  );

/**
 * Delete a mini-driver device
 *
 * @param Device
 *   The mini-driver device to be deleted
 */
extern WVL_M_LIB VOID STDCALL WvlDeleteDevice(IN DEVICE_OBJECT * Device);

/**
 * Call a function in a device's thread context
 *
 * @param Device
 *   The device whose thread context will be used
 *
 * @param Function
 *   The function to be called
 *
 * @param Context
 *   Optional.  Additional context to pass to the function
 *
 * @param Wait
 *   Specifies whether or not to wait for the called function to complete.
 *   If FALSE, a work item will be allocated from non-paged pool and will
 *   be freed once the called function has completed
 *
 * @retval STATUS_INSUFFICIENT_RESOURCES
 *   If 'Wait' was FALSE, this means that a work item could not be
 *   allocated from non-paged pool.  Otherwise, this is the status returned
 *   by the called function
 * @retval STATUS_SUCCESS
 *   If 'Wait' was FALSE, this means the the work item was enqueued
 * @retval STATUS_INVALID_PARAMETER
 *    An invalid parameter was passed to this function or, if 'Wait'
 *    is 'TRUE', this status could also have been returned by the called
 *    function
 * @retval STATUS_NO_SUCH_DEVICE
 *    Returned if the device is no longer available
 * @return
 *   If 'Wait' was TRUE, the status is that returned by the called function
 */
extern WVL_M_LIB NTSTATUS STDCALL WvlCallFunctionInDeviceThread(
    IN DEVICE_OBJECT * Device,
    IN F_WVL_DEVICE_THREAD_FUNCTION * Function,
    IN VOID * Context,
    IN BOOLEAN Wait
  );

/**
 * Miscellaneous: Grouped memory allocation functions.
 */

/* A group of memory allocations. */
typedef struct WVL_MEM_GROUP {
    PCHAR First;
    PCHAR Last;
    PCHAR Current;
  } WVL_S_MEM_GROUP, * WVL_SP_MEM_GROUP;
extern WVL_M_LIB VOID STDCALL WvlMemGroupInit(OUT WVL_SP_MEM_GROUP);
extern WVL_M_LIB PVOID STDCALL WvlMemGroupAlloc(
    IN OUT WVL_SP_MEM_GROUP,
    IN SIZE_T
  );
extern WVL_M_LIB VOID STDCALL WvlMemGroupFree(IN OUT WVL_SP_MEM_GROUP);
extern WVL_M_LIB PVOID STDCALL WvlMemGroupNextObj(IN OUT WVL_SP_MEM_GROUP);
extern WVL_M_LIB PVOID STDCALL WvlMemGroupBatchAlloc(
    IN OUT WVL_SP_MEM_GROUP,
    IN SIZE_T,
    IN UINT32
  );

/** Struct/union type definitions */

/** An atomic list */
struct S_WVL_LOCKED_LIST {
    LIST_ENTRY List[1];
    KSPIN_LOCK Lock;
  };

/** Track resource usage */
struct S_WVL_RESOURCE_TRACKER {
    /** The usage count */
    LONG UsageCount;

    /** An event that is set when the usage count is zero */
    KEVENT ZeroUsage;
  };

/** A mini-driver */
struct S_WVL_MINI_DRIVER {
    /** Position in the list of registered mini-drivers */
    LIST_ENTRY Link[1];

    /** The mini-driver's driver object, provided by Windows */
    DRIVER_OBJECT * DriverObject;

    /** Probe and potentially drive a PDO with an FDO */
    DRIVER_ADD_DEVICE * AddDevice;

    /** Perform mini-driver cleanup */
    DRIVER_UNLOAD * Unload;

    /** Track mini-driver usage */
    S_WVL_RESOURCE_TRACKER Usage[1];
  };

/** The common part of the device extension for all mini-driver devices */
struct WV_DEV_EXT {
    /** Points to the old common device extension.  TODO: Rework */
    struct WV_DEV_T * device;

    /** The IRP dispatch function */
    DRIVER_DISPATCH * IrpDispatch;

    /** The device thread's signal for IRP arrival */
    KEVENT IrpArrival;

    /** A handle to the device thread */
    HANDLE ThreadHandle;

    /** The device's IRP queue.  Requires lock for modification */
    LIST_ENTRY IrpQueue[1];

    /** Is the device available?  Requires lock for modification */
    BOOLEAN NotAvailable;

    /** The device lock */
    KSPIN_LOCK Lock;

    /** The device's mini-driver */
    S_WVL_MINI_DRIVER * MiniDriver;

    /** The device usage */
    S_WVL_RESOURCE_TRACKER Usage[1];
  };

/** A pseudo-IRP device thread work item */
struct S_WVL_DEVICE_THREAD_WORK_ITEM {
    /**
     * The device thread function to be called when this work
     * item is processed
     */
    F_WVL_DEVICE_THREAD_FUNCTION * Function;

    /** Signals when the function to be called has completed */
    KEVENT Complete;

    /** The status returned by the called function */
    NTSTATUS Status;

    /**
     * A dummy IRP that can be linked to the device's IRP queue.
     * (What a waste of space!)
     */
    IRP DummyIrp[1];
  };

/** Objects */
extern DRIVER_OBJECT * WvDriverObj;
extern UINT32 WvFindDisk;
extern KSPIN_LOCK WvFindDiskLock;
extern S_WVL_RESOURCE_TRACKER WvDriverUsage[1];

#endif	/* WV_M_DRIVER_H_ */
