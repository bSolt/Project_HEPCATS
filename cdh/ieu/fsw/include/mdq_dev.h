///////////////////////////////////////////////////////////////////////////////
//
// (Magnetometer) DAQ Device
//
// DAQ device variable and libusb session declaration
//
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: February 24, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Variable declarations:
extern libusb_device_handle* dev_hdl; // Device handle
extern libusb_context* ctx;           // libusb session