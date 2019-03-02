///////////////////////////////////////////////////////////////////////////////
//
// Create Telemetry Packet Transfer Frame
// 
// Telemetry packet transfer frames are created from source data, source
// origin APID, group flag, and sequence count. These parameters are placed
// into packet fields.
//
// Source data passed that is not TLM_PKT_USR_DAT_SIZE bytes in size is made
// to TLM_PKT_USR_DAT_SIZE bytes by populating the remaining space with NULL.
// This case would be for the last transfer frame of an image.
//
// -------------------------------------------------------------------------- /
//
// Input Arguments:
// - N/A
//
// Output Arguments:
// - N/A
//
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: February 2, 2018
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdlib.h>  // Standard library
#include <string.h>  // String function definitions
#include <stdint.h>  // Integer types
#include <time.h>    // Standard time function definitions

// Macro definitions:
#define TLM_PKT_USR_DAT_SIZE 1064 // Telemetry packet user data size in bytes

#define APID_IMG 0x64 // Image destination APID
#define APID_MDQ 0xC8 // Magnetometer DAQ destination APID

// Packet header structure:
struct pkt_hdr {
    // Packet I.D.:
    unsigned int pkt_id_vrs:          3;  //  3 bits
    unsigned int pkt_id_typ:          1;  //  1 bit
    unsigned int pkt_id_sec_hdr_flg:  1;  //  1 bit
    unsigned int pkt_id_apid:        11;  // 11 bits

    // Packet Sequence Control:
    unsigned int pkt_seq_cnt_grp_flg:  2;  //  2 bits
    unsigned int pkt_seq_cnt_seq_cnt: 14;  // 14 bits

    // Packet Length:
    uint16_t pkt_len; // 16 bits
};

// Packet data field structure:
struct pkt_dat_fld {
    // Packet Secondary Header:
    uint32_t pkt_sec_hdr_t_sec;          //  32 bits
    uint16_t pkt_sec_hdr_t_msec;         //  16 bits
    uint8_t  pkt_sec_hdr_t_void;         //  8 bits
    unsigned int pkt_sec_hdr_p_ext:   1; //  1 bit
    unsigned int pkt_sec_hdr_p_id:    3; //  3 bits
    unsigned int pkt_sec_hdr_p_bas:   2; //  2 bit
    unsigned int pkt_sec_hdr_p_frc:   2; //  2 bits

    // Packet User Data Field:
    char pkt_usr_dat[TLM_PKT_USR_DAT_SIZE];

    // Packet Error Control:
    uint16_t pkt_err_cnt; // 16 bits
};

void crt_tlm_pkt_xfr_frm(char* src_dat, size_t src_dat_size,
    char* tlm_pkt_xfr_frm_buf, uint16_t apid, uint8_t grp_flg,
    uint16_t seq_cnt) {
    // Definitions and initializations: 
    struct timeval tv; // Unix timestamp structure

    struct pkt_hdr pkt_hdr;         // Packet header structure
    struct pkt_dat_fld pkt_dat_fld; // Packet data field structure

    // Populate packet I.D. fields:
    pkt_hdr.pkt_id_vrs =         0; // (version 1)
    pkt_hdr.pkt_id_typ =         0; // (telemetry packet)
    pkt_hdr.pkt_id_sec_hdr_flg = 1; // (secondary header present)
    pkt_hdr.pkt_id_apid =     apid; // (packet APID)

    // Populate packet sequence control fields
    pkt_hdr.pkt_seq_cnt_grp_flg = grp_flg; // Grouping flag
    pkt_hdr.pkt_seq_cnt_seq_cnt = seq_cnt; // Sequence count

    // Populate packet length field:
    pkt_hdr.pkt_len = 1073; // "C" (Octets in packet data field - 1)

    // Get current Unix time stamp:
    gettimeofday(&tv,NULL);

    // Populate packet secondary header T fields:
    pkt_dat_fld.pkt_sec_hdr_t_sec  = tv.tv_sec;       // Seconds
    pkt_dat_fld.pkt_sec_hdr_t_msec = tv.tv_usec/1000; // Milliseconds  
    pkt_dat_fld.pkt_sec_hdr_t_void = 0xFF;            // "11111111"

    // Populate packet secondary header P fields:
    pkt_dat_fld.pkt_sec_hdr_p_ext = 0; // "0"   (no extension)
    pkt_dat_fld.pkt_sec_hdr_p_id =  2; // "010" (time code I.D.)
    pkt_dat_fld.pkt_sec_hdr_p_bas = 3; // "11"  (Number of octets of the basic
                                       //        time unit minus one)
    pkt_dat_fld.pkt_sec_hdr_p_frc = 2; // "10"  (Number of octets of the 
                                       //        fractional time unit)

    // Populate user data field. Check to see if source data is the same size
    // as the packet's user data size:
    if (src_dat_size == TLM_PKT_USR_DAT_SIZE) {
        // Set packet user data to source data:
        memcpy(pkt_dat_fld.pkt_usr_dat,src_dat,TLM_PKT_USR_DAT_SIZE);
    } else {
        // Populate packet user data with source data:
        memcpy(pkt_dat_fld.pkt_usr_dat+0,src_dat,src_dat_size);

        // Populate remaining space with "E":
        memset(pkt_dat_fld.pkt_usr_dat+src_dat_size,"E",\
            (TLM_PKT_USR_DAT_SIZE - src_dat_size));
    } 

    // Populate packer error control field:
    pkt_dat_fld.pkt_err_cnt = 0xFFFF; // "1"s (no error detection)

    // Copy APID, group flag, and creation time to transfer frame buffer:
    memcpy(tlm_pkt_xfr_frm_buf+0,&apid,2);
    memcpy(tlm_pkt_xfr_frm_buf+2,&grp_flg,1);
    memcpy(tlm_pkt_xfr_frm_buf+3,&pkt_dat_fld.pkt_sec_hdr_t_sec,4);
    memcpy(tlm_pkt_xfr_frm_buf+7,&pkt_dat_fld.pkt_sec_hdr_t_msec,2);

    // Copy Packet Header to transfer frame buffer:
    memcpy(tlm_pkt_xfr_frm_buf+9,&pkt_hdr,6);

    // Copy Packet Data Field to transfer frame buffer:
    memcpy(tlm_pkt_xfr_frm_buf+15,&pkt_dat_fld,1074);

    return;
}