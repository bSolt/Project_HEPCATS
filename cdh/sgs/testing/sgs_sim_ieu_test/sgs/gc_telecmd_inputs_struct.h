///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
//
// Telecommand inputs structure declaration
//
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: November 1, 2018
//
///////////////////////////////////////////////////////////////////////////////

// Structure declaration:
struct telecmd_pkt_inputs {
    // Packet Identification:
    uint16_t pkt_apid;

    // Packet Sequence Control:
    uint16_t pkt_name;

    // Packet Secondary Header:
    uint32_t pkt_sec_hdr_t_sec;  //  32 bits
    uint16_t pkt_sec_hdr_t_msec; //  16 bits
    uint8_t  pkt_sec_hdr_t_void; //  8 bits

    // Packet Data Field:
    uint8_t pkt_app_dat_atc_flg;
    uint8_t pkt_app_dat_cmd_arg;
};