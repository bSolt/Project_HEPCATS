///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
// 
// Simulated IEU Create Idle Telemetry Packet
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
// Created: November 10, 2018
// 
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library 
#include <string.h>  // String function definitions 
#include <unistd.h>  // UNIX standard function definitions 
#include <stdint.h>  // Integer types
#include "time.h"    // Standard time function definitions

#include "get_doy.h" // Function definition

// Packet header structure:
struct pkt_hdr
{
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

	// Packet User Data Field:
	uint8_t pkt_usr_dat[1064]; // 1064 bytes

	// Packet Error Control:
	uint16_t pkt_err_cnt: 16; // 16 bits
};

char* sim_ieu_crt_tlm_pkt(char* buffer)
{
	// Define packet structure:
	struct pkt_hdr pkt_hdr;
	struct pkt_dat_fld pkt_dat_fld;

	// Populate packet I.D. fields:
	pkt_hdr.pkt_id_vrs =         0; // "000"           (version 1)
	pkt_hdr.pkt_id_typ =         0; // "0"             (telemetry packet)
	pkt_hdr.pkt_id_sec_hdr_flg = 0; // "0"             (idle packet)
	pkt_hdr.pkt_id_apid =     2047; // "11111111111"   (idle packet APID)

	// Populate packet sequence control fields: 
	pkt_hdr.pkt_seq_cnt_grp_flg = 3; // "11" (unsegmented data)  
	pkt_hdr.pkt_seq_cnt_seq_cnt = 0; // "00000000000000" 
	                                 // (first packet in count)

	// Populate packet length field:
	pkt_hdr.pkt_len = 1073; // "C" (Octets in packet data field - 1)

	// Define time variables and get current time in UTC:
	struct tm *tm; time_t now;
	time(&now); tm = gmtime(&now);

	// Populate packet secondary header T fields:
	pkt_dat_fld.pkt_sec_hdr_t_year = tm->tm_year+1900;
	pkt_dat_fld.pkt_sec_hdr_t_doy  = \
		get_doy(tm->tm_mon+1,tm->tm_mday,tm->tm_year+1900);
	pkt_dat_fld.pkt_sec_hdr_t_hour = tm->tm_hour;
	pkt_dat_fld.pkt_sec_hdr_t_min  = tm->tm_min;
	pkt_dat_fld.pkt_sec_hdr_t_sec  = tm->tm_sec;

	// Populate packet secondary header P fields:
	pkt_dat_fld.pkt_sec_hdr_p_ext = 0; // "0"   (no extension)
	pkt_dat_fld.pkt_sec_hdr_p_id =  5; // "101" (time code I.D.)
	pkt_dat_fld.pkt_sec_hdr_p_cal = 1; // "1"   (DOY variation)
	pkt_dat_fld.pkt_sec_hdr_p_red = 1; // "001" (subsecond resolution)

	// Populate user data field:
	for (int i = 0; i < 1063; ++i)
		pkt_dat_fld.pkt_usr_dat[i] = 255; // "1s" (idle packet user data)

	// Populate packer error control field:
	pkt_dat_fld.pkt_err_cnt = 0; // "0000000000000000" (not researched yet)

	// Copy Packet Header to buffer:
	memcpy(buffer,&pkt_hdr,6);

	// Copy Packet Data Field to buffer:
	memcpy(buffer+6,&pkt_dat_fld,1074);

	return buffer;
}