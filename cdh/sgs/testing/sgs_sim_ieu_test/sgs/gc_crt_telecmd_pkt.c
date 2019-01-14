///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
// 
// Create telecommand packet
//
// -------------------------------------------------------------------------- /
//
// Input Arguments:
// - telecmd_pkt_inputs (APID, Packet Name, Packet Secondary Header, 
//                       and Application Data)
//
// Output Arguments:
// - buffer
// 
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: November 4, 2018
// 
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library
#include <string.h>  // String function definitions
#include <unistd.h>  // UNIX standard function definitions
#include <stdint.h>  // Integer types

// Header files:
#include "gc_telecmd_inputs_struct.h" // Structure declarations

// Packet header structure
struct pkt_hdr {
    // Packet I.D.:
    unsigned int pkt_id_vrs:          3;  //  3 bits
    unsigned int pkt_id_typ:          1;  //  1 bit
    unsigned int pkt_id_sec_hdr_flg:  1;  //  1 bit
    unsigned int pkt_id_apid:        11;  // 11 bits

    // Packet Sequence Control:
    unsigned int pkt_seq_cnt_grp_flg:   2;  //  2 bits
    unsigned int pkt_seq_cnt_pkt_name: 14;  // 14 bits

    // Packet Length:
    uint16_t pkt_len; // 16 bits   
};

// Packet data field structure
struct pkt_dat_fld {
    // Packet Secondary Header:
    uint32_t pkt_sec_hdr_t_sec;          //  32 bits
    uint16_t pkt_sec_hdr_t_msec;         //  16 bits
    uint8_t  pkt_sec_hdr_t_void;         //  8 bits
    unsigned int pkt_sec_hdr_p_ext:   1; //  1 bit
    unsigned int pkt_sec_hdr_p_id:    3; //  3 bits
    unsigned int pkt_sec_hdr_p_bas:   2; //  2 bit
    unsigned int pkt_sec_hdr_p_frc:   2; //  2 bits

    // Packet User (Application) Data Field:
    unsigned int pkt_app_dat_atc_flg:  1; // 1 bit
    unsigned int pkt_app_dat_cmd_arg: 31; // 31 bits

    // Packet Error Control:
    uint16_t pkt_err_cnt; // 16 bits
};

char* gc_crt_telecmd_pkt(struct telecmd_pkt_inputs telecmd_pkt_inputs,\
    char* buffer) {
    // Define packet structure:
    struct pkt_hdr pkt_hdr;
    struct pkt_dat_fld pkt_dat_fld;

    // Populate packet I.D. fields:
    pkt_hdr.pkt_id_vrs =         0; // "000" (always)
    pkt_hdr.pkt_id_typ =         1; // "0"   (telecommand packet)
    pkt_hdr.pkt_id_sec_hdr_flg = 1; // "1"   (not idle packet)
    pkt_hdr.pkt_id_apid = \
        telecmd_pkt_inputs.pkt_apid;

    // Populate packet sequence control fields: 
    pkt_hdr.pkt_seq_cnt_grp_flg  =  3; // "11" (unsegmented data) 
    pkt_hdr.pkt_seq_cnt_pkt_name = \
        telecmd_pkt_inputs.pkt_name;

    // Populate packet length field:
    pkt_hdr.pkt_len = 10; // "C" (Octets in packet data field - 1)

    // Populate packet secondary header T fields:
    pkt_dat_fld.pkt_sec_hdr_t_sec  = telecmd_pkt_inputs.pkt_sec_hdr_t_sec;
    pkt_dat_fld.pkt_sec_hdr_t_msec = telecmd_pkt_inputs.pkt_sec_hdr_t_msec;
    pkt_dat_fld.pkt_sec_hdr_t_void = telecmd_pkt_inputs.pkt_sec_hdr_t_void;

    // Populate packet secondary header P fields:
    pkt_dat_fld.pkt_sec_hdr_p_ext = 0; // "0"   (no extension)
    pkt_dat_fld.pkt_sec_hdr_p_id  = 2; // "010" (time code I.D.)
    pkt_dat_fld.pkt_sec_hdr_p_bas = 3; // "11"  (Number of octets of the basic
                                       //        time unit minus one)
    pkt_dat_fld.pkt_sec_hdr_p_frc = 2; // "10"  (Number of octets of the 
                                       //        fractional time unit)

    // Populate user data field:
    pkt_dat_fld.pkt_app_dat_atc_flg = telecmd_pkt_inputs.pkt_app_dat_atc_flg;
    pkt_dat_fld.pkt_app_dat_cmd_arg = telecmd_pkt_inputs.pkt_app_dat_cmd_arg;

    // Populate packer error control field:
    pkt_dat_fld.pkt_err_cnt = 0xFFFF; // "1"s (no error detection)

    // Copy Packet Header and Data Field to buffer:
    memcpy(buffer+0,&pkt_hdr,6);
    memcpy(buffer+6,&pkt_dat_fld,14);

    // Return:
    return buffer;
}