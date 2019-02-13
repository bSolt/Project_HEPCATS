///////////////////////////////////////////////////////////////////////////////
//
// Process Telemetry Packet
// 
// Telemetry packet processor
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
// Created: February 12, 2018
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library
#include <string.h>  // String function definitions
#include <unistd.h>  // UNIX standard function definitions
#include <stdint.h>  // Integer types

// Macro definitions:
#define APID_SW 0x00  // Software origin
#define APID_IMG 0x64 // Image origin
#define APID_MDQ 0xC8 // Magnetometer DAQ origin

// Telemetry processor function
void proc_tlm_pkt(char* buffer) {   
    // Definitions:
    uint16_t pkt_hdr_pkt_id;
    uint16_t pkt_hdr_pkt_seq_cnt;
    uint16_t pkt_hdr_pkt_len;

    uint32_t pkt_dat_fld_sec_hdr_t_fld_sec;
    uint16_t pkt_dat_fld_sec_hdr_t_fld_msec;
    uint8_t pkt_dat_fld_sec_hdr_p_fld;
    uint32_t pkt_dat_fld_usr_data;
    uint16_t pkt_dat_fld_pkt_err_cnt;

    uint8_t val_telecmd_pkt_cnt; // Valid telecommand packet counter
    uint8_t inv_telecmd_pkt_cnt; // Invalid telecommand packet counter
    uint8_t rx_telecmd_pkt_cnt;  // Received telecommand packet count
    uint8_t val_cmd_apid_cnt;    // Valid command counter
    uint8_t inv_cmd_apid_cnt;    // Invalid command counter
    uint8_t cmd_exec_suc_cnt;    // Commands executed successfully counter
    uint8_t cmd_exec_err_cnt;    // Commands not executed (error) counter

    char file_path[50];
    char file_name[100];

    // Save input buffer to seperate fields:
    memcpy(&pkt_hdr_pkt_id,buffer,2);
    memcpy(&pkt_hdr_pkt_seq_cnt,buffer+2,2);
    memcpy(&pkt_hdr_pkt_len,buffer+4,2);

    memcpy(&pkt_dat_fld_sec_hdr_t_fld_sec,buffer+6,4);
    memcpy(&pkt_dat_fld_sec_hdr_t_fld_msec,buffer+10,2);
    memcpy(&pkt_dat_fld_sec_hdr_p_fld,buffer+13,1);
    memcpy(&pkt_dat_fld_usr_data,buffer+14,1064);
    memcpy(&pkt_dat_fld_pkt_err_cnt,buffer+1078,2);

    // Packet Header Identification:
    uint8_t pkt_id_vrs;
    uint8_t pkt_id_typ;
    uint8_t pkt_id_sec_hdr_flg;
    uint16_t pkt_id_apid;

    // Packet I.D. Version:
    pkt_id_vrs = pkt_hdr_pkt_id & 0x07; // Mask to keep bits 0-2

    // Packet I.D. Type:
    pkt_id_typ = pkt_hdr_pkt_id & 0x08; // Mask to keep bit 3 
    pkt_id_typ = pkt_id_typ >> 3;       // Shift right by 3 bits

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
    uint32_t pkt_t_fld_sec;
    uint16_t pkt_t_fld_msec;

    uint8_t pkt_p_fld_ext;
    uint8_t pkt_p_fld_id;
    uint8_t pkt_p_fld_bas;
    uint8_t pkt_p_fld_frc;

    // Packet Data Field Packet Secondary Header T-Fields:
    pkt_t_fld_sec = pkt_dat_fld_sec_hdr_t_fld_sec;
    pkt_t_fld_msec = pkt_dat_fld_sec_hdr_t_fld_msec;

    // Packet Secondary Header P-Field Extension:
    pkt_p_fld_ext = pkt_dat_fld_sec_hdr_p_fld & 0x0; // Mask to keep bit 0

    // Packet Secondary Header P-Field Identification:
    pkt_p_fld_id = pkt_dat_fld_sec_hdr_p_fld & 0xE; // Mask to keep bits 1-3
    pkt_p_fld_id = pkt_p_fld_id >> 1;               // Shift right by 1 bit

    // Packet Secondary Header P-Field Variation:
    pkt_p_fld_bas = pkt_dat_fld_sec_hdr_p_fld & 0x30; // Mask to keep bit 4-5
    pkt_p_fld_bas = pkt_p_fld_bas >> 4;               // Shift right by 4 bits

    // Packet Secondary Header P-Field Variation:
    pkt_p_fld_frc = pkt_dat_fld_sec_hdr_p_fld & 0xC0; // Mask to keep bit 6-7
    pkt_p_fld_frc = pkt_p_fld_frc >> 6;               // Shift right by 5 bits

    // Packet Data Field Application Data
    int pkt_usr_data[1064]; 
    memcpy(pkt_usr_data,&pkt_dat_fld_usr_data,1064);

    // Packet Data Field Packet Error Control:
    uint16_t pkt_err_cnt = pkt_dat_fld_pkt_err_cnt;

    // If housekeeping telemetry, parse data and print. If image or
    // magnetometer data, save file:
    if (pkt_id_apid == APID_SW) {
        // Parse data:
        memcpy(&rx_telecmd_pkt_cnt,pkt_usr_data+0,1);
        memcpy(&val_telecmd_pkt_cnt,pkt_usr_data+1,1);
        memcpy(&inv_telecmd_pkt_cnt,pkt_usr_data+2,1);
        memcpy(&val_cmd_apid_cnt,pkt_usr_data+3,1);
        memcpy(&inv_cmd_apid_cnt,pkt_usr_data+4,1);
        memcpy(&cmd_exec_suc_cnt,pkt_usr_data+5,1);
        memcpy(&cmd_exec_err_cnt,pkt_usr_data+6,1);

        // Print:
        printf("%u,%u,%u,%u,%u,%u,%u\n",rx_telecmd_pkt_cnt,\
            val_telecmd_pkt_cnt,inv_telecmd_pkt_cnt,val_cmd_apid_cnt,\
            inv_cmd_apid_cnt,cmd_exec_suc_cnt,cmd_exec_err_cnt);
    } else if (pkt_id_apid == APID_MDQ) {
        // Set file path:
        strcpy(file_path,"/tmp/mag/");

        // Create file name:
        sprintf(file_name,"%s%u_%u.bin",file_path,pkt_t_fld_sec,pkt_t_fld_msec);

        // Open file:
        FILE* file_ptr = fopen(file_name,"wb");

        // Print user data to file:
        fwrite(&pkt_usr_data,1,1064,file_ptr);

        // Close file:
        fclose(file_ptr);
    } else if (pkt_id_apid == APID_IMG) {
        // If first segment, create new file:
        if (pkt_seq_cnt_grp_flg == 1) {
            // Set filepath:
            strcpy(file_path,"/tmp/img/");

            // Create file name:
            sprintf(file_name,"%s%u_%u.bin",file_path,pkt_t_fld_sec,\
                pkt_t_fld_msec);
            
            // Open file:
            FILE* file_ptr = fopen(file_name,"wb");

            // Print user data to file:
            fwrite(&pkt_usr_data,1,1064,file_ptr);

            // Close file:
            fclose(file_ptr);

            // Open file:
            file_ptr = fopen("current_file.txt","w");

            // Print file name to file:
            fprintf(file_ptr,"%s",file_name);

            // Close file:
            fclose(file_ptr);
        // Otherwise append to current file:
        } else {
            // Open file to get current file name:
            FILE* file_ptr = fopen("current_file.txt","r");

            // Get first line:
            fgets(file_name,100,file_ptr);

            // Close file:
            fclose(file_ptr);

            // Open file:
            file_ptr = fopen(file_name,"ab");

             // Print user data to file:
            fwrite(&pkt_usr_data,1,1064,file_ptr);

            // Close file:
            fclose(file_ptr);
        }
    }

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
    printf("      T-Field Sec             : %u\n",pkt_t_fld_sec);
    printf("      T-Field mSec            : %u\n",pkt_t_fld_msec);
    printf("      P-Field Ext             : %u\n",pkt_p_fld_ext);
    printf("      P-Field I.D.            : %u\n",pkt_p_fld_id);
    printf("      P-Field Basic Octets    : %u\n",pkt_p_fld_bas);
    printf("      P-Field Fraction Octets : %u\n",pkt_p_fld_frc);
    printf("  Packet Error Control\n");
    printf("      Packet Error Control  : %u\n",pkt_err_cnt);
    printf("\n");

    return;
}

/*
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
printf("      T-Field Sec             : %u\n",pkt_t_fld_sec);
printf("      T-Field mSec            : %u\n",pkt_t_fld_msec);
printf("      P-Field Ext             : %u\n",pkt_p_fld_ext);
printf("      P-Field I.D.            : %u\n",pkt_p_fld_id);
printf("      P-Field Basic Octets    : %u\n",pkt_p_fld_bas);
printf("      P-Field Fraction Octets : %u\n",pkt_p_fld_frc);
printf("  Packet Error Control\n");
printf("      Packet Error Control  : %u\n",pkt_err_cnt);
printf("\n");
*/