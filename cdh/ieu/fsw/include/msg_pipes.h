///////////////////////////////////////////////////////////////////////////////
//
// Message Pipes Header
//
// Xenomai message pipe variable declarations
//
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: February 18, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Message pipe declarations:
extern RT_PIPE ips_msg_pipe; // For raw and processed images
                             // (read_usb_img_task <--> ips)