///////////////////////////////////////////////////////////////////////////////
//
// Filter Table (TO & DS)
//
// Filter Table (TO & DS) follow these formats:
//
//        |      Normal       |     Realtime      | 
// |------|-------------------|-------------------|
// | APID | Range | Frequency | Range | Frequency | ... 
// |------|-------|-----------|-------|-----------|
// | 0x## |   #   |     #     |   #   |     #     | ...
//
// The column order after the APId is: normal, realtime, playback, imaging,
// magnetometer. The APIDs currently are SW (HK), IMG, and MDQ.
//
// The range is the number of telemetry packets to consider while the frequency
// is how many of those telemetry packets will be downlinked or stored. For
// example, if range = 2 and frequency = 1, then 1 out of every 2 telemetry
// packets will be directed to either be downlinked or recorded.
//
// When selecting the filter table mode, use the filter table mode (number
// itself) to find the column indicies to read to get range and frequency
// values. This would be flt_tbl_mode*2+1 and flt_tbl_mode*2+2.
//
// Update the tables if a mode is changed, removed, or added. Also, check
// filter table task as this task makes assumptions about how the filter
// tables are set up.
//
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: March 11, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Macro definitions
#define FLT_TBL_ROW 3  // Filter table TO & DS row size
#define FLT_TBL_COL 11 // Filter table TO & DS column size

// Telemetry output (TO) table declaration:
extern uint16_t flt_tbl_to[3][11] = {
    {0x00,1,1,1,1,1,1,1,1,1,1} ,
    {0x64,1,1,1,1,0,0,1,1,0,0} ,
    {0xC8,1,1,1,1,0,0,0,0,1,1}
};

// Data storage (DS) table declaration:
extern uint16_t flt_tbl_ds[3][11] = {
    {0x00,1,1,0,0,1,1,1,1,1,1} ,
    {0x64,1,1,0,0,1,1,1,1,0,0} ,
    {0xC8,1,1,0,0,1,1,0,0,1,1}
};