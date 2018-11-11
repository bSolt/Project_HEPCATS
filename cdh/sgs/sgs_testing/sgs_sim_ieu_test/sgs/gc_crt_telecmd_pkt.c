///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
// 
// Ground Control Create Telecommand Packet
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

#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library 
#include <string.h>  // String function definitions 
#include <unistd.h>  // UNIX standard function definitions 
#include <stdint.h>  // Integer types

#include "gc_telecmd_inputs_struct.h" // Structure definition

// Packet header structure
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

// Packet data field structure
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

// Create telecommand packet function
char* gc_crt_telecmd_pkt(struct telecmd_pkt_inputs telecmd_pkt_inputs,char* buffer)
{
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
	pkt_hdr.pkt_seq_cnt_grp_flg =  3; // "11" (unsegmented data) 
	pkt_hdr.pkt_seq_cnt_pkt_name = \
		telecmd_pkt_inputs.pkt_name;

	// Populate packet length field:
	pkt_hdr.pkt_len = 13; // "C" (Octets in packet data field - 1)

	// Populate packet secondary header T fields:
	pkt_dat_fld.pkt_sec_hdr_t_year = telecmd_pkt_inputs.pkt_t_year;
	pkt_dat_fld.pkt_sec_hdr_t_doy =  telecmd_pkt_inputs.pkt_t_doy;
	pkt_dat_fld.pkt_sec_hdr_t_hour = telecmd_pkt_inputs.pkt_t_hour;
	pkt_dat_fld.pkt_sec_hdr_t_min =  telecmd_pkt_inputs.pkt_t_min;
	pkt_dat_fld.pkt_sec_hdr_t_sec =  telecmd_pkt_inputs.pkt_t_sec; 

	// Populate packet secondary header P fields:
	pkt_dat_fld.pkt_sec_hdr_p_ext = 0; // "0"   (no extension)
	pkt_dat_fld.pkt_sec_hdr_p_id =  5; // "101" (time code I.D.)
	pkt_dat_fld.pkt_sec_hdr_p_cal = 1; // "1"   (DOY variation)
	pkt_dat_fld.pkt_sec_hdr_p_red = 0; // "000" (second resolution)

	// Populate user data field:
	pkt_dat_fld.pkt_app_dat = telecmd_pkt_inputs.pkt_app_data;

	// Populate packer error control field:
	pkt_dat_fld.pkt_err_cnt = 0; // "0000000000000000" (no error detection)

	// Copy Packet Header and Data Field to buffer:
	memcpy(buffer+0,&pkt_hdr,6);
	memcpy(buffer+6,&pkt_dat_fld,14);

	return buffer;
}