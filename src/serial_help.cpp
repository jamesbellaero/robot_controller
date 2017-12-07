/*
 * Copyright (c) 2010-2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */

/*
	Common code used by POSIX samples to extract serial port settings from
	the command-line arguments passed into the program.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xbee/serial.h"
#include "serial_help.h"

/*
	Parse the command-line arguments, looking for "/dev/" to determine the
	serial port to use, and a bare number (assumed to be the baud rate).
	@param[in]	argc		argument count
	@param[in]	argv		array of \a argc arguments
	@param[out]	serial	serial port settings
*/
void parse_serial_arguments( int argc, char *argv[], xbee_serial_t *serial)
{
	int i;
	uint32_t baud;

	memset( serial, 0, sizeof *serial);

	// default baud rate
	serial->baudrate = 9600;
	//cycle through all arguments
	for (i = 1; i < argc; ++i)
	{
		//if any of the arguments have /dev in them, set device to dev
		if (strncmp( argv[i], "/dev", 4) == 0)
		{
			//copy the whole string to serial->device. max 40 characters
			strncpy( serial->device, argv[i], (sizeof serial->device) - 1);
	    //add null character to the end
			serial->device[(sizeof serial->device) - 1] = '\0';
		}
		//if an argument specifies a baudrate, set it here. To specify baud, just pass the number
		if ( (baud = (uint32_t) strtoul( argv[i], NULL, 0)) > 0)
		{
			serial->baudrate = baud;
		}
	}
	//if the device was not specified, prompt user input
	while (*serial->device == '\0')
	{
		printf( "Connect to which device? ");
		fgets( serial->device, sizeof serial->device, stdin);
		// strip any trailing newline characters (CR/LF)
		serial->device[strcspn(serial->device, "\r\n")] = '\0';
	}
}
