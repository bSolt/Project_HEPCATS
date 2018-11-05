///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
// 
// Simulated IEU Telecommand Packet Processor
//
// -------------------------------------------------------------------------- /
//
// Input Arguments:
// - buffer (contains CCSDS telecommand packet)
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
// Created: November 1, 2018
// 
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library 
#include <string.h>  // String function definitions 
#include <unistd.h>  // UNIX standard function definitions 
#include <stdint.h>  // Integer types

void telecmd_pkt_proc(char* buffer)
{	
	// Initialize:
	uint16_t pkt_hdr_pkt_id;
	uint16_t pkt_hdr_pkt_seq_cnt;
	uint16_t pkt_hdr_pkt_len;

	uint64_t pkt_dat_fld_sec_hdr;
	uint32_t pkt_dat_fld_app_dat;
	uint16_t pkt_dat_fld_pkt_err_cnt;

	// Save input buffer to seperate fields:
	memcpy(&pkt_hdr_pkt_id,buffer,2);         // 2 bytes of buffer (offset 0)
	memcpy(&pkt_hdr_pkt_seq_cnt,buffer+2,2);  // 2 bytes of buffer (offset 2)
	memcpy(&pkt_hdr_pkt_len,buffer+4,2);      // 2 bytes of buffer (offset 4)

	memcpy(&pkt_dat_fld_sec_hdr,buffer+6,8);  // 8 bytes of buffer (offset 6)
	memcpy(&pkt_dat_fld_app_dat,buffer+14,4); // 4 bytes of buffer (offset 14)
	memcpy(&pkt_dat_fld_pkt_err_cnt,\
		buffer+18,2);                         // 2 bytes of buffer (offset 18)

	// Packet Header
	uint8_t pkt_id_vrs;
	uint8_t pkt_id_typ;
	uint8_t pkt_id_sec_hdr_flg;
	uint16_t pkt_id_apid;

	// Packet I.D. Version:
	pkt_id_vrs = pkt_hdr_pkt_id & 0x07; // Mask to keep bits 6-8

	// Packet I.D. Type:
	pkt_id_typ = pkt_hdr_pkt_id & 0x08; // Mask to keep bit 5 
	pkt_id_typ = pkt_id_typ >> 3; 	    // Shift right by 3 bits

	// Packet I.D. Secondary Header Flag:
	pkt_id_sec_hdr_flg = pkt_hdr_pkt_id & 0x10;   // Mask to keep bit 6
	pkt_id_sec_hdr_flg = pkt_id_sec_hdr_flg >> 4; // Shift right by 4 bits

	// Packet I.D. APID:
	pkt_id_apid = pkt_hdr_pkt_id & 0xFFE0; // Mask to keep bits 7-16
	pkt_id_apid = pkt_id_apid >> 5;        // Shift right by 5 bits

    // Print results:
    printf("Packet Header\n");
    printf("  Packet I.D.\n");
    printf("      Version               : %d\n",pkt_id_vrs);
    printf("      Type                  : %d\n",pkt_id_typ);
    printf("      Secondary Header Flag : %d\n",pkt_id_sec_hdr_flg);
    printf("      APID                  : %d\n",pkt_id_apid);

	/*unsigned int mask=1<<((sizeof(uint16_t)<<3)-1);
    while(mask) 
    {
        printf("%d", (pkt_hdr_pkt_id&mask ? 1 : 0));
        mask >>= 1;
    } printf("\n");
	*/

 	// Mask key:
	// (Bits are counted from Right --> Left)
	// Packet Header Packet I.D. Version:          0x07   = 0000000000000111
	// Packet Header Packet I.D. Type:             0x08   = 0000000000001000
	// Packet Header Packet Secondary Header Flag: 0x10   = 0000000000010000
	/// Packet Header Packet APID:                 0xFFE0 = 1111111111100000

	return;
}