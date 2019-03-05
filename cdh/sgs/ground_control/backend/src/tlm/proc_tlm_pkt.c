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
#define APID_SW  0x00 // Software origin
#define APID_IMG 0x64 // Image origin
#define APID_MDQ 0xC8 // Magnetometer DAQ origin

#define MDQ_BUF_SIZE 768 // Magnetometer DAQ read size in bytes

// Telemetry processor function
void proc_tlm_pkt(char* buffer) {
    // Definitions:
    int empty_ind = 0; // Index where user data begins to be empty

    uint16_t pkt_hdr_pkt_id;
    uint16_t pkt_hdr_pkt_seq_cnt;
    uint16_t pkt_hdr_pkt_len;

    uint32_t pkt_dat_fld_sec_hdr_t_fld_sec;
    uint16_t pkt_dat_fld_sec_hdr_t_fld_msec;
    uint8_t  pkt_dat_fld_sec_hdr_p_fld;
    char     pkt_dat_fld_usr_data[1064];
    uint16_t pkt_dat_fld_pkt_err_cnt;

    uint8_t  val_telecmd_pkt_cnt = 0;     // Valid telecommand packet counter
    uint8_t  inv_telecmd_pkt_cnt = 0;     // Invalid telecommand packet counter
    uint8_t  rx_telecmd_pkt_cnt = 0;      // Received telecommand packet count
    uint8_t  val_cmd_cnt = 0;             // Valid command counter
    uint8_t  inv_cmd_cnt = 0;             // Invalid command counter
    uint8_t  cmd_exec_suc_cnt = 0;        // Commands executed successfully counter
    uint8_t  cmd_exec_err_cnt = 0;        // Commands not executed (error) counter
    uint16_t tlm_pkt_xfr_frm_seq_cnt = 0; // Packet sequence count
    uint16_t acq_img_cnt = 0;             // Acquired images count
    uint8_t  img_acq_prog_flag = 0;       // Image acquisition in progress flag
    uint8_t  mdq_scan_state = 0;          // Magnetometer DAQ scanning state
    uint8_t  ers_rly_swtch_state = 0;     // Electrical relay switch state
    uint8_t  flt_tbl_mode = 0;            // Filter table mode

    float mdq_conv_buf[MDQ_BUF_SIZE/2]; // Magnetometer DAQ converted data buffer

    float mdq_chnl0_sum = 0; // Channel 0 sum
    float mdq_chnl1_sum = 0; // Channel 1 sum
    float mdq_chnl2_sum = 0; // Channel 2 sum

    float mdq_chnl0_avg; // Channel 0 average
    float mdq_chnl1_avg; // Channel 1 average
    float mdq_chnl2_avg; // Channel 2 average

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

    // Packet Data Field Packet Error Control:
    uint16_t pkt_err_cnt = pkt_dat_fld_pkt_err_cnt;

    // Check the source data type:
    // (If housekeeping, parse and print to screen. If imaging, save to file.
    // If magnetometer, parse, print averaged magnetic field to screen, and
    // save to file)
    if (pkt_id_apid == APID_SW) {
        // Parse data:
        memcpy(&rx_telecmd_pkt_cnt,pkt_dat_fld_usr_data+0,1);
        memcpy(&val_telecmd_pkt_cnt,pkt_dat_fld_usr_data+1,1);
        memcpy(&inv_telecmd_pkt_cnt,pkt_dat_fld_usr_data+2,1);
        memcpy(&val_cmd_cnt,pkt_dat_fld_usr_data+3,1);
        memcpy(&inv_cmd_cnt,pkt_dat_fld_usr_data+4,1);
        memcpy(&cmd_exec_suc_cnt,pkt_dat_fld_usr_data+5,1);
        memcpy(&cmd_exec_err_cnt,pkt_dat_fld_usr_data+6,1);
        memcpy(&tlm_pkt_xfr_frm_seq_cnt,pkt_dat_fld_usr_data+7,2);
        memcpy(&acq_img_cnt,pkt_dat_fld_usr_data+9,2);
        memcpy(&img_acq_prog_flag,pkt_dat_fld_usr_data+11,1);
        memcpy(&ers_rly_swtch_state,pkt_dat_fld_usr_data+12,1);
        memcpy(&mdq_scan_state,pkt_dat_fld_usr_data+13,1);
        memcpy(&flt_tbl_mode,pkt_dat_fld_usr_data+14,1);

        // Print:
        printf("0x00:%u,%u,%u,%u,%u,%u,%u,%u,%u,%s,%s,%s,%s\n",\
            rx_telecmd_pkt_cnt,val_telecmd_pkt_cnt,inv_telecmd_pkt_cnt,\
            val_cmd_cnt,inv_cmd_cnt,cmd_exec_suc_cnt,\
            cmd_exec_err_cnt,tlm_pkt_xfr_frm_seq_cnt,acq_img_cnt,\
            img_acq_prog_flag ? "In progress" : "Idle",\
            ers_rly_swtch_state ? "On" : "Off",\
            mdq_scan_state ? "Scanning" : "Idle",\
            flt_tbl_mode == 0 ? "Normal" : flt_tbl_mode == 1 ? \
            "Playback" : flt_tbl_mode == 2 ? "IMG" : "MDQ");
    } else if (pkt_id_apid == APID_MDQ) {
        // Loop through channels 0, 1, and 2 raw data buffers to convert from
        // char to signed 16 bit numbers. Each char is 8 bits, so the
        // measurement in each channel is composed of two chars: the one at 2n
        // and the one at 2n+1. To combine, bit shift the char at 2n+1 and
        // combine with char at 2n. Once converted to 16 bit numbers, convert
        // from counts to nano Tesla. The conversion is:
        // [nT] = (10*(counts/32768))[V] * (10000[nT]/1[V])
        for (int i = 0; i < MDQ_BUF_SIZE/2; ++i) {
            // Convert two char to signed 16 bit number:
            mdq_conv_buf[i] = (pkt_dat_fld_usr_data[2*i+1] << 8) | \
                (pkt_dat_fld_usr_data[2*i] & 0xFF);

            // Convert from counts to nT:
            mdq_conv_buf[i] = \
                ((mdq_conv_buf[i]*10)/32768)*10000;
        }

        // Loop to find sum of channel 0, 1, and 2:
        for (int i = 0; i < (MDQ_BUF_SIZE/2-2); i+=3) {
            mdq_chnl0_sum = mdq_chnl0_sum + mdq_conv_buf[i];
            mdq_chnl1_sum = mdq_chnl1_sum + mdq_conv_buf[i+1];
            mdq_chnl2_sum = mdq_chnl2_sum + mdq_conv_buf[i+2];
        }

        // Find averages for channel 0, 1, and 2:
        mdq_chnl0_avg = mdq_chnl0_sum/128;
        mdq_chnl1_avg = mdq_chnl1_sum/128;
        mdq_chnl2_avg = mdq_chnl2_sum/128;

        // Print to screen:
        printf("0xC8:%0.3f,%0.3f,%0.3f\n",mdq_chnl0_avg,mdq_chnl1_avg,\
            mdq_chnl2_avg);

        // Set file path:
        strcpy(file_path,"../raw_record_files/mdq/"); // Relative to bin

        // Create file name:
        sprintf(file_name,"%s%u_%u.bin",file_path,pkt_t_fld_sec,pkt_t_fld_msec);

        // Open file:
        FILE* file_ptr = fopen(file_name,"wb");

        // Print converted user data for channel 0, 1, and 2:
        fwrite(&mdq_conv_buf,1,sizeof(mdq_conv_buf),file_ptr);

        // Close file:
        fclose(file_ptr);
    } else if (pkt_id_apid == APID_IMG) {
        // If first segment, create new file:
        if (pkt_seq_cnt_grp_flg == 1) {
            // Set filepath:
            strcpy(file_path,"../raw_record_files/img/"); // Relative to bin

            // Create file name:
            sprintf(file_name,"%s%u_%u.raw",file_path,pkt_t_fld_sec,\
                pkt_t_fld_msec);

            // Open file:
            FILE* file_ptr = fopen(file_name,"wb");

            // Print user data to file:
            fwrite(&pkt_dat_fld_usr_data,1,1064,file_ptr);

            // Close file:
            fclose(file_ptr);

            // Open file:
            file_ptr = fopen("../raw_record_files/img/current_file.txt","w");

            // Print file name to file:
            fprintf(file_ptr,"%s",file_name);

            // Close file:
            fclose(file_ptr);
        // Otherwise append to current file:
        } else {
            // Check if last segment:
            if (pkt_seq_cnt_grp_flg == 2) {
                // Loop to determine byte position where "E" begins
                // ("E" indicates that the character is empty)
                for (int i = 0; i < 1064; ++i) {
                    if (strcmp(pkt_dat_fld_usr_data[i],"E") == 0) {
                        // Check if the next position is also "E". If yes, then
                        // found the byte position:
                        if (strcmp(pkt_dat_fld_usr_data[i+1],"E") == 0) {
                            empty_ind = i;
                        }
                    }
                }
            }

            // Open file to get current file name:
            FILE* file_ptr = \
                fopen("../raw_record_files/img/current_file.txt","r");

            // Get first line:
            fgets(file_name,100,file_ptr);

            // Close file:
            fclose(file_ptr);

            // Open file:
            file_ptr = fopen(file_name,"ab");

             // Print user data to file:
            fwrite(&pkt_dat_fld_usr_data,1,1064-empty_ind,file_ptr);

            // Close file:
            fclose(file_ptr);
        }
    }

    // Will never reach this:
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
