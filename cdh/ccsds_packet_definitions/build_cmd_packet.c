///////////////////////////////////////////////////////////////////////////////
//
// Define Example CCSDS Telecommand Packet 
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

#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library 
#include <string.h>  // String function definitions 
#include <unistd.h>  // UNIX standard function definitions 
#include <stdint.h>  // Integer types

// Packet header structure:
struct pkt_hdr
{
	// Packet I.D.:
	unsigned int pkt_id_vrs:          3;  //  3 bits
	unsigned int pkt_id_typ:          1;  //  1 bit
	unsigned int pkt_id_sec_hdr_flg:  1;  //  1 bit
	unsigned int pkt_id_apid:        11;  // 11 bits

	// Packet Sequence Control:
	unsigned int pkt_seq_cnt_grp_flg:   2;  //  2 bits
	unsigned int pkt_seq_cnt_pkt_name: 14;  // 14 bits

	// Packet Length:
	unsigned int pkt_len: 16; // 16 bits	
};

// Packet data field structure:
struct pkt_dat_fld
{
	// Packet Secondary Header:
	uint16_t pkt_sec_hdr_t_year;         // 16 bits
	uint16_t pkt_sec_hdr_t_doy;          // 16 bits 
	uint8_t pkt_sec_hdr_t_hour;          //  8 bits  
	uint8_t pkt_sec_hdr_t_min;           //  8 bits
	uint8_t pkt_sec_hdr_t_sec;           //  8 bits
	unsigned int pkt_sec_hdr_p_ext:   1; //  1 bit
	unsigned int pkt_sec_hdr_p_id:    3; //  3 bits
	unsigned int pkt_sec_hdr_p_cal:   1; //  1 bit
	unsigned int pkt_sec_hdr_p_red:   3; //  3 bits

	// Packet User (Application) Data Field:
	uint32_t pkt_app_dat; // 4 bytes

	// Packet Error Control:
	unsigned int pkt_err_cnt: 16; // 16 bits
};

void main(int argc, char const *argv[])
{
	// Define packet structure:
	struct pkt_hdr pkt_hdr;
	struct pkt_dat_fld pkt_dat_fld;

	// Populate packet I.D. fields:
	pkt_hdr.pkt_id_vrs =         0; // "000"           (always)
	pkt_hdr.pkt_id_typ =         1; // "0"             (telecommand packet)
	pkt_hdr.pkt_id_sec_hdr_flg = 0; // "0"             (idle packet)
	pkt_hdr.pkt_id_apid =     2047; // "11111111111"   (idle packet APID)

	// Populate packet sequence control fields: 
	pkt_hdr.pkt_seq_cnt_grp_flg =      3; // "11" (unsegmented data)  
	pkt_hdr.pkt_seq_cnt_pkt_name = 16383; // "11111111111111" 
	                                      // (idle packet name)

	// Populate packet length field:
	pkt_hdr.pkt_len = 13; // "C" (Octets in packet data field - 1)

	// Populate packet secondary header T fields:
	pkt_dat_fld.pkt_sec_hdr_t_year = 2018; // "00000011111100010"
	pkt_dat_fld.pkt_sec_hdr_t_doy =   305; // "00000000100110001"
	pkt_dat_fld.pkt_sec_hdr_t_hour =    0; // "00000000"
	pkt_dat_fld.pkt_sec_hdr_t_min =    33; // "00100001"
	pkt_dat_fld.pkt_sec_hdr_t_sec =    27; // "00011011"

	// Populate packet secondary header P fields:
	pkt_dat_fld.pkt_sec_hdr_p_ext = 0; // "0"   (no extension)
	pkt_dat_fld.pkt_sec_hdr_p_id =  5; // "101" (time code I.D.)
	pkt_dat_fld.pkt_sec_hdr_p_cal = 1; // "1"   (DOY variation)
	pkt_dat_fld.pkt_sec_hdr_p_red = 1; // "001" (subsecond resolution)

	// Populate user data field:
	pkt_dat_fld.pkt_app_dat = 2147483647; // "1s" 
	                                      // (idle packet application data)

	// Populate packer error control field:
	pkt_dat_fld.pkt_err_cnt = 0; // "0000000000000000" (not researched yet)

	return;
}