///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
// 
// Ground Control Telemetry Packet Processor
//
// -------------------------------------------------------------------------- /
//
// Input Arguments:
// - buffer (contains CCSDS telemetry packet)
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

// Telemetry processor function
void gc_proc_tlm_pkt(char* buffer)
{	
	// Initialize:
	uint16_t pkt_hdr_pkt_id;
	uint16_t pkt_hdr_pkt_seq_cnt;
	uint16_t pkt_hdr_pkt_len;

	uint16_t pkt_dat_fld_sec_hdr_t_fld_year;
	uint16_t pkt_dat_fld_sec_hdr_t_fld_doy;
	uint8_t pkt_dat_fld_sec_hdr_t_fld_hour;
	uint8_t pkt_dat_fld_sec_hdr_t_fld_min;
	uint8_t pkt_dat_fld_sec_hdr_t_fld_sec;
	uint8_t pkt_dat_fld_sec_hdr_p_fld;
	uint32_t pkt_dat_fld_usr_data;
	uint16_t pkt_dat_fld_pkt_err_cnt;

	// Save input buffer to seperate fields:
	memcpy(&pkt_hdr_pkt_id,buffer,2);         // 2 bytes of buffer (offset 0)
	memcpy(&pkt_hdr_pkt_seq_cnt,buffer+2,2);  // 2 bytes of buffer (offset 2)
	memcpy(&pkt_hdr_pkt_len,buffer+4,2);      // 2 bytes of buffer (offset 4)

	memcpy(&pkt_dat_fld_sec_hdr_t_fld_year,buffer+6,2);  // 2 bytes of buffer (offset 6)
	memcpy(&pkt_dat_fld_sec_hdr_t_fld_doy,buffer+8,2);   // 2 bytes of buffer (offset 8)
	memcpy(&pkt_dat_fld_sec_hdr_t_fld_hour,buffer+10,1); // 1 byte of buffer (offset 10)
	memcpy(&pkt_dat_fld_sec_hdr_t_fld_min,buffer+11,1);  // 1 byte of buffer (offset 11)
	memcpy(&pkt_dat_fld_sec_hdr_t_fld_sec,buffer+12,1);  // 1 byte of buffer (offset 12)
	memcpy(&pkt_dat_fld_sec_hdr_p_fld,buffer+13,1);      // 1 byte of buffer (offset 13)
	memcpy(&pkt_dat_fld_usr_data,buffer+14,1064);        // 1067 bytes of buffer (offset 14)
	memcpy(&pkt_dat_fld_pkt_err_cnt,buffer+1078,2);      // 2 bytes of buffer (offset 1078)

	// Packet Header Identification:
	uint8_t pkt_id_vrs;
	uint8_t pkt_id_typ;
	uint8_t pkt_id_sec_hdr_flg;
	uint16_t pkt_id_apid;

	// Packet I.D. Version:
	pkt_id_vrs = pkt_hdr_pkt_id & 0x07; // Mask to keep bits 0-2

	// Packet I.D. Type:
	pkt_id_typ = pkt_hdr_pkt_id & 0x08; // Mask to keep bit 3 
	pkt_id_typ = pkt_id_typ >> 3; 	    // Shift right by 3 bits

	// Packet I.D. Secondary Header Flag:
	pkt_id_sec_hdr_flg = pkt_hdr_pkt_id & 0x10;   // Mask to keep bit 4
	pkt_id_sec_hdr_flg = pkt_id_sec_hdr_flg >> 4; // Shift right by 4 bits

	// Packet I.D. APID:
	pkt_id_apid = pkt_hdr_pkt_id & 0xFFE0; // Mask to keep bits 5-15
	pkt_id_apid = pkt_id_apid >> 5;        // Shift right by 5 bits

	// Packet Header Packet Sequence Control
	uint8_t pkt_seq_cnt_grp_flg;
	uint16_t pkt_seq_cnt_pkt_name;

	// Packet Sequence Control Grouping Flag:
	pkt_seq_cnt_grp_flg = pkt_hdr_pkt_seq_cnt & 0x3; // Mask to keep bits 0-1

	// Packet Sequence Control Packet Name:
	pkt_seq_cnt_pkt_name = pkt_hdr_pkt_seq_cnt \
		& 0xFFFC; // Mask to keep bits 2-15 
	pkt_seq_cnt_pkt_name = pkt_hdr_pkt_seq_cnt >> 2; // Shift right by 2 bits

	// Packet Header Packet Length
	uint16_t pkt_len = pkt_hdr_pkt_len;

	// Packet Data Field Packet Secondary Header
	uint16_t pkt_t_fld_year;
	uint16_t pkt_t_fld_doy;
	uint8_t pkt_t_fld_hour;
	uint8_t pkt_t_fld_min;
	uint8_t pkt_t_fld_sec;
	
	uint8_t pkt_p_fld_ext;
	uint8_t pkt_p_fld_id;
	uint8_t pkt_p_fld_var;
	uint8_t pkt_p_fld_res;

	// Packet Data Field Packet Secondary Header T-Fields:
	pkt_t_fld_year = pkt_dat_fld_sec_hdr_t_fld_year;
	pkt_t_fld_doy = pkt_dat_fld_sec_hdr_t_fld_doy;
	pkt_t_fld_hour = pkt_dat_fld_sec_hdr_t_fld_hour;
	pkt_t_fld_min = pkt_dat_fld_sec_hdr_t_fld_min;
	pkt_t_fld_sec = pkt_dat_fld_sec_hdr_t_fld_sec;

	// Packet Secondary Header P-Field Extension:
	pkt_p_fld_ext = pkt_dat_fld_sec_hdr_p_fld & 0x0; // Mask to keep bit 0

	// Packet Secondary Header P-Field Identification:
	pkt_p_fld_id = pkt_dat_fld_sec_hdr_p_fld & 0xE; // Mask to keep bits 1-3 
	pkt_p_fld_id = pkt_p_fld_id >> 1;               // Shift right by 1 bit 

	// Packet Secondary Header P-Field Variation:
	pkt_p_fld_var = pkt_dat_fld_sec_hdr_p_fld & 0x10; // Mask to keep bit 4
	pkt_p_fld_var = pkt_p_fld_var >> 4;               // Shift right by 4 bits

	// Packet Secondary Header P-Field Variation:
	pkt_p_fld_res = pkt_dat_fld_sec_hdr_p_fld & 0xE0; // Mask to keep bit 5-7
	pkt_p_fld_res = pkt_p_fld_res >> 5;               // Shift right by 5 bits

	// Packet Data Field Application Data
	int pkt_usr_data[1064]; 
	memcpy(pkt_usr_data,&pkt_dat_fld_usr_data,1064);

	// Packet Data Field Packet Error Control:
	uint16_t pkt_err_cnt = pkt_dat_fld_pkt_err_cnt;

    // Print results:
    printf("Packet Header\n");
    printf("  Packet I.D.\n");
    printf("      Version               : %u\n",pkt_id_vrs);
    printf("      Type                  : %u\n",pkt_id_typ);
    printf("      Secondary Header Flag : %u\n",pkt_id_sec_hdr_flg);
    printf("      APID                  : %u\n",pkt_id_apid);
    printf("  Packet Sequence Control\n");
    printf("      Grouping Flags        : %u\n",pkt_seq_cnt_grp_flg);
    printf("      Sequence Count        : %u\n",pkt_seq_cnt_pkt_name);
	printf("  Packet Length\n");
	printf("      Packet Length         : %u\n",pkt_len);
	printf("Packet Data Field\n");
	printf("  Packet Secondary Header\n");
    printf("      T-Field Year          : %u\n",pkt_t_fld_year);
    printf("      T-Field DOY           : %u\n",pkt_t_fld_doy);
    printf("      T-Field Hour          : %u\n",pkt_t_fld_hour);
    printf("      T-Field Min           : %u\n",pkt_t_fld_min);
    printf("      T-Field Sec           : %u\n",pkt_t_fld_sec);
    printf("      P-Field Ext           : %u\n",pkt_p_fld_ext);
    printf("      P-Field I.D.          : %u\n",pkt_p_fld_id);
    printf("      P-Field Variation     : %u\n",pkt_p_fld_var);
    printf("      P-Field Resolution    : %u\n",pkt_p_fld_res);
    printf("  Packet Error Control\n");
    printf("      Packet Error Control  : %u\n",pkt_err_cnt);
    printf("\n");

	return;
}