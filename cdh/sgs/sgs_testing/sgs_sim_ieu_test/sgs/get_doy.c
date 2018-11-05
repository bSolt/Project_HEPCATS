///////////////////////////////////////////////////////////////////////////////
//
// Determines Day of Year given Month, Day, and Year
//
// -------------------------------------------------------------------------- /
//
// Input Arguments:
// - mon
// - day
// - year
//
// Output Arguments:
// - doy
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

// Determines year:
uint16_t is_leap_year(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Determines DOY:
uint16_t get_doy(int mon, int day, int year)
{
    static const int days[2][13] = {
        {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
        {0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}
    };

    int leap = is_leap_year(year);

    return days[leap][mon] + day;
}