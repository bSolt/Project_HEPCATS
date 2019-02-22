///////////////////////////////////////////////////////////////////////////////
//
// Telecommand Inputs Structure
//
// Telecommand inputs structure declaration
//
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: Febraury 12, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Structure declaration:
struct telecmd_pkt_inputs {
    // Packet Identification:
    uint16_t pkt_apid;

    // Packet Sequence Control:
    uint16_t pkt_name;

    // Packet Secondary Header:
    uint32_t pkt_sec_hdr_t_sec;
    uint16_t pkt_sec_hdr_t_msec;
    uint8_t pkt_sec_hdr_t_void;

    // Packet Data Field:
    uint8_t pkt_app_dat_atc_flg;
    uint32_t pkt_app_dat_cmd_arg;
};