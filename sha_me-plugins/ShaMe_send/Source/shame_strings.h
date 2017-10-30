/*
  ==============================================================================

    shame_strings.h
    Created: 9 Oct 2017 11:52:04am
    Author:  Antonio Ortega Brook

  ==============================================================================
*/

#pragma once

#ifndef shame_strings_h
#define shame_strings_h


/** Writer's messages **/

/** Info about shame creation
 */
#define SUCCESSFULLY_CREATED	"Connected to %s"
#define ALREADY_EXISTS		"'%s' already exists"
#define COULDNT_CREATE		"Couldn't create '%s' (error %d)"
#define TOO_MUCH_DATA           "Couldn't create '%s'. Too much data to write"
#define TRUNCATE_FAILED         "Couldn't create '%s'. Truncate failed"
#define SHAME_MAP_FAILED        "Couldn't create '%s'. Map failed (errno %D)"
#define NOT_A_VALID_NAME        "'%s' is not a valid name"
#define SUCCESSFULLY_DETTACHED  "Disconnected"

/** Info about connected readers
 */
#define READERS_CONNECTED	"%d readers connected\n"
#define READER_DONT_MATCH_VS	"Some readers don't match vector size (%d samples)"
#define READER_DONT_MATCH_SR	"Some readers don't match sample rate (%.2f Hz)"
#define N_READERS_DONT_MATCH_VS	"%d readers don't match vector size (%d samples)"
#define N_READERS_DONT_MATCH_SR	"%d readers don't match sample rate (%.2f Hz)"


















#endif /* shame_strings_h */
