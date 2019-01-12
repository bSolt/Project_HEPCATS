///////////////////////////////////////////////////////////////////////////////
//
// Define Example CCSDS Telemetry Packet
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
// Created: October 31, 2018
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library
#include <string.h>  // String function definitions
#include <unistd.h>  // UNIX standard function definitions
#include <stdint.h>  // Standard time function definitions

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
    uint8_t pkt_usr_dat[1064]; // 1064 bytes

    // Packet Error Control:
    uint16_t pkt_err_cnt: 16; // 16 bits
};

void main(int argc, char const *argv[]) {
    // Define packet structure:
    struct pkt_hdr pkt_hdr;
    struct pkt_dat_fld pkt_dat_fld;

    // Populate packet I.D. fields:
    pkt_hdr.pkt_id_vrs =         0;  // "000"          (version 1)
    pkt_hdr.pkt_id_typ =         0;  // "0"            (telemetry packet)
    pkt_hdr.pkt_id_sec_hdr_flg = 0;  // "0"            (idle packet)
    pkt_hdr.pkt_id_apid =     0x7FF; // "11111111111"  (idle packet APID)

    // Populate packet sequence control fields: 
    pkt_hdr.pkt_seq_cnt_grp_flg = 3; // "11" (unsegmented data)  
    pkt_hdr.pkt_seq_cnt_seq_cnt = 0; // "00000000000000" 
                                     // (first packet in count)

    // Populate packet length field:
    pkt_hdr.pkt_len = 1073; // "C" (Octets in packet data field - 1)

    // Populate packet secondary header T fields:
    pkt_dat_fld.pkt_sec_hdr_t_sec  = 1547261938; // "1011100001110010101011111110010"
    pkt_dat_fld.pkt_sec_hdr_t_msec =        824; // "1100111000"  
    pkt_dat_fld.pkt_sec_hdr_t_void =       0xFF; // "11111111"

    // Populate packet secondary header P fields:
    pkt_dat_fld.pkt_sec_hdr_p_ext = 0; // "0"   (no extension)
    pkt_dat_fld.pkt_sec_hdr_p_id =  2; // "010" (time code I.D.)
    pkt_dat_fld.pkt_sec_hdr_p_bas = 3; // "11"  (Number of octets of the basic
                                       //        time unit minus one)
    pkt_dat_fld.pkt_sec_hdr_p_frc = 2; // "10"  (Number of octets of the 
                                       //        fractional time unit)

    // Populate user data field:
    for (int i = 0; i < 1063; ++i)
        pkt_dat_fld.pkt_usr_dat[i] = 0xFF; // "1"s (idle packet user data)

    // Populate packer error control field:
    pkt_dat_fld.pkt_err_cnt = 0xFFFF; // "1"s (no error detection)

    // Return:
    return;
}
