///////////////////////////////////////////////////////////////////////////////
//
// Create Telemetry Packet Transfer Frame header
//
// Create telemetry packet transfer function declaration
//
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: February 2, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Function declaration:
void crt_tlm_pkt_xfr_frm(char* src_dat, size_t src_dat_size,
    char* tlm_pkt_xfr_frm_buf, uint16_t apid, uint8_t grp_flg,
    uint16_t seq_cnt);