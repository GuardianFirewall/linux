/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/*
 * This file holds USB constants and structures that are needed for
 * USB's DFU (Device Firmware Update 1.1) descriptor and protocols.
 *
 * Authored by Rick Mark - 2020 from USB-IF DFU 1.1 spec
 *
 * This includes:
 *
 * - A DFU specific USB configuration descriptor
 * - DFU request and response structures and commands
 *
 * Note all descriptors are declared '__attribute__((packed))' so that:
 *
 * [a] they never get padded, either internally (USB spec writers
 *     probably handled that) or externally;
 *
 * [b] so that accessing bigger-than-a-bytes fields will never
 *     generate bus errors on any platform, even when the location of
 *     its descriptor inside a bundle isn't "naturally aligned", and
 *
 * [c] for consistency, removing all doubt even when it appears to
 *     someone that the two other points are non-issues for that
 *     particular descriptor type.
 */

#ifndef _UAPI__LINUX_USB_DFU_H
#define _UAPI__LINUX_USB_DFU_H

#include <linux/types.h>	/* __u8 etc */
#include <asm/byteorder.h>	/* le16_to_cpu */

#define USB_DT_DFU_FUNCTIONAL 0x21

/*-------------------------------------------------------------------------*/

/* USB_DT_CONFIG: Configuration descriptor information.
 *
 * USB_DT_OTHER_SPEED_CONFIG is the same descriptor, except that the
 * descriptor type is different.  Highspeed-capable devices can look
 * different depending on what speed they're currently running.  Only
 * devices with a USB_DT_DEVICE_QUALIFIER have any OTHER_SPEED_CONFIG
 * descriptors.
 */
struct usb_dfu_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;

	__u8  bmAttributes;
#define USB_DFU_WILL_DETACH             0x08
#define USB_DFU_MANIFESTATION_TOLERANT  0x04
#define USB_DFU_CAN_UPLOAD              0x02
#define USB_DFU_CAN_DOWNLOAD            0x01
	__u16 wDetachTimeOut;
	__u16 wTransferSize;
} __attribute__ ((packed));

#define USB_DT_DFU_FUNCTIONAL_SIZE		  7

struct usb_dfu1_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;

	__u8  bmAttributes;
#define USB_DFU_WILL_DETACH             0x08
#define USB_DFU_MANIFESTATION_TOLERANT  0x04
#define USB_DFU_CAN_UPLOAD              0x02
#define USB_DFU_CAN_DOWNLOAD            0x01
	__u16 wDetachTimeOut;
	__u16 wTransferSize;
#define USB_DFU_BCD_VERSION_1_0         0x0100
  __u16 bcdDFUVersion;
} __attribute__ ((packed));

#define USB_DT_DFU1_FUNCTIONAL_SIZE		  9

#define USB_DFU_REQUEST_DETACH          0x00
#define USB_DFU_REQUEST_DNLOAD          0x01
#define USB_DFU_REQUEST_UPLOAD          0x02
#define USB_DFU_REQUEST_GETSTATUS       0x03
#define USB_DFU_REQUEST_CLRSTATUS       0x04
#define USB_DFU_REQUEST_GETSTATE        0x05
#define USB_DFU_REQUEST_ABORT           0x06

struct usb_dfu_getstatus {
  __u8  bStatus;
  __u32 bwPollTimeout : 24;

  __u8  bState;
  __u8  iString;
} __attribute__ ((packed));

#define USB_DFU_GETSTATUS_SIZE		  6

// No error condition is present.
#define USB_DFU_STATUS_OK                 0x00
// File is not targeted for use by this device.
#define USB_DFU_STATUS_ERR_TARGET         0x01
// File is for this device but fails some vendor-specific verification test.
#define USB_DFU_STATUS_ERR_FILE           0x02
// Device is unable to write memory.
#define USB_DFU_STATUS_ERR_WRITE          0x03
// Memory erase function failed.
#define USB_DFU_STATUS_ERR_ERASE          0x04
// Memory erase check failed.
#define USB_DFU_STATUS_ERR_CHECK_ERASEED  0x05
// Program memory function failed.
#define USB_DFU_STATUS_ERR_PROG           0x06
// Programmed memory failed verification.
#define USB_DFU_STATUS_ERR_VERIFY         0x07
// Cannot program memory due to received address that is out of range.
#define USB_DFU_STATUS_ERR_ADDRESS        0x08
// Received DFU_DNLOAD with wLength = 0, but device does not think it has all
//  of the data yet.
#define USB_DFU_STATUS_ERR_NOTDONE        0x09
// Device’s firmware is corrupt. It cannot return to run-time (non-DFU)
//  operations.
#define USB_DFU_STATUS_ERR_FIRMWARE       0x0A
// iString indicates a vendor-specific error.
#define USB_DFU_STATUS_ERR_VENDOR         0x0B
// Device detected unexpected USB reset signaling.
#define USB_DFU_STATUS_ERR_USBR           0x0C
// Device detected unexpected power on reset.
#define USB_DFU_STATUS_ERR_POR            0x0D
// Something went wrong, but the device does not know what it was.
#define USB_DFU_STATUS_ERR_UNKNOWN        0x0E
// Device stalled an unexpected request.
#define USB_DFU_STATUS_ERR_STALLEDPKT     0x0F


// Device is running its normal application.
#define USB_DFU_STATE_APP_IDLE                    0x00
// Device is running its normal application, has received the DFU_DETACH
//  request, and is waiting for a USB reset.
#define USB_DFU_STATE_APP_DETACH                  0x01
// Device is operating in the DFU mode and is waiting for requests.
#define USB_DFU_STATE_DFU_IDLE                    0x02
// Device has received a block and is waiting for the host to solicit the
//  status via DFU_GETSTATUS.
#define USB_DFU_STATE_DFU_DNLOAD_SYNC             0x03
// Device is programming a control-write block into its nonvolatile memories.
#define USB_DFU_STATE_DFU_DNBUSY                  0x04
// Device is processing a download operation. Expecting DFU_DNLOAD requests.
#define USB_DFU_STATE_DFU_DNLOAD_IDLE             0x05
// Device has received the final block of firmware from the host and is waiting
//  for receipt of DFU_GETSTATUS to begin the Manifestation phase; or device
//  has completed the Manifestation phase and is waiting for receipt of
//  DFU_GETSTATUS. (Devices that can enter this state after the Manifestation
//  phase set bmAttributes bit bitManifestationTolerant to 1.)
#define USB_DFU_STATE_DFU_MANIFEST_SYNC           0x06
// Device is in the Manifestation phase. (Not all devices will be able to
//  respond to DFU_GETSTATUS when in this state.)
#define USB_DFU_STATE_DFU_MANIFEST                0x07
// Device has programmed its memories and is waiting for a USB reset or a power
//  on reset. (Devices that must enter this state clear
//  bitManifestationTolerant to 0.)
#define USB_DFU_STATE_DFU_MANIFEST_WAIT_RESET     0x08
// The device is processing an upload operation. Expecting DFU_UPLOAD requests.
#define USB_DFU_STATE_DFU_UPLOAD_IDLE             0x09
// An error has occurred. Awaiting the DFU_CLRSTATUS request.
#define USB_DFU_STATE_DFU_ERROR                   0x0A


struct usb_dfu_getstate {
  __u8  bState;
} __attribute__ ((packed));

#define USB_DFU_GETSTATE_SIZE		  1

#endif
