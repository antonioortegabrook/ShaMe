/**
	shame.h

	Created by Antonio Ortega Brook on 9/27/17.
	Copyright © 2017 Antonio Ortega Brook. All rights reserved.

	* NOTE:
	* Avoid C++ reserved words, so it can be compiled as C++ too.
 */

#ifndef shame_h
#define shame_h


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>




/** Maximum number of readers and writers
 */
#define MAX_READERS	128
#define MAX_WRITERS	128

/** Page size multiplier (file size is this multiple of page size)
 */
#define MULT_PAGESIZE	32

/** Empty position in an array
 */
#define EMPTY		0

/** ShaMe writer status flags
 */
#define F_INITIALIZED			1	/* 000001 */
#define F_CAN_READ			2	/* 000010 */
#define F_CAN_WRITE			4	/* 000100 */
#define F_READERS_ATTACHED		8	/* 001000 */
#define F_SOME_READERS_CAN_READ		16	/* 010000 */
#define F_ALL_READERS_CAN_READ		32	/* 100000 */

/** ShaMe writer status codes
 */
#define	S_UNINITIALIZED			0	/* 000000 */
#define S_CANT_READ			1	/* 000001 */  /* S_UNINITIALIZED	  | F_INITIALIZED	    */
#define S_CANT_WRITE			3	/* 000011 */  /* S_CANT_READ		  | F_CAN_READ		    */
#define S_NO_READERS_ATTACHED		7	/* 000111 */  /* S_CANT_WRITE		  | F_CAN_WRITE		    */
#define S_NO_READERS_CAN_READ		15	/* 001111 */  /* S_NO_READERS_ATTACHED	  | F_READERS_ATTACHED	    */
#define S_SOME_READERS_CANT_READ	31	/* 011111 */  /* S_NO_READERS_CAN_READ	  | F_SOME_READERS_CAN_READ */
#define S_ALL_READERS_CAN_READ		63	/* 111111 */  /* S_SOME_READERS_CANT_READ | F_ALL_READERS_CAN_READ  */


/** ShaMe error codes
 */
typedef enum {
	/** General error */
	E_SHAME_SUCCESS = 0,
	
	/** init_shame_writer() errors	*/
	E_SHAME_INIT_WRITER_SUCCESS,
	E_SHAME_TOO_MUCH_DATA_TO_WRITE,
	E_SHAME_READABLE_BUT_NOT_WRITABLE,
	E_SHAME_MAP_FAILED,
	E_SHAME_COULDNT_CREATE,
	E_SHAME_TRUNCATE_FAILED,
	
	/** init_shame_reader() errors	*/
	E_SHAME_INIT_READER_SUCCESS,
	E_SHAME_DOESNT_EXIST,
	E_SHAME_REMAP_FAILED_UNMAP,
	E_SHAME_REMAP_FAILED_MAP,
	
	/** dettach_shame() errors	*/
	E_SHAME_DETTACH_SUCCESS,
	E_SHAME_UNMAP_FAILED,
	E_SHAME_NO_FILE_TO_UNLINK,
	E_SHAME_UNLINK_FAILED
	
} t_shame_error;

/** The Shared Memory struct
	* Server-side init:
		1 - Open the file.
		2 - If we are creating it, ftruncate() to sizeof(struct shame).
		3 - mmap() with size = sizeof(struct shame).
		4 - Fill the server_* fields and find out sample_data_size.
		5 - mmap() again with size sizeof(struct shame) + sample_data_size.
*/
struct shame {
	char	name[256];
	
	/** Sample_data is allocated from these, because they belong to the
	 writer process.
	 */
	double	sample_rate;
	int	vector_size;
	long	nchannels;
	
	/** We use these to know when should we unlink, etc. they might also be
	 useful to display some connection information
	 */
	
	/** Was created by a reader? (bool) (not in use)
	 */
	int	created_by_reader;
	
	/** The ID of the writer that can write to this shame
	 */
	int	owner_id;
	
	/** How many readers and writers are attached?
	 */
	int	attached_readers;
	int	attached_writers;
	
	/** Readers and writers PIDs
	 */
	pid_t	reader_pid[MAX_READERS];
	pid_t	writer_pid[MAX_WRITERS];
	
	/** readers' sample rates and buffer sizes; these are only for dysplay
	 information on the writer-side; readers cannot modify attributes of
	 sample_data and they are responsible for not reading anything they
	 can not read.
	 */
	int	reader_sample_rate[MAX_READERS];
	int	reader_vector_size[MAX_READERS];
	
	
	/** This is the size (in bytes) of the block we write the samples to.
	 It is equal to:
	 server_vector_size * server_nchannels * sizeof(double) - sizeof(double)
	 */
	size_t	sample_data_size;
	size_t	total_number_of_samples;
	size_t	map_size;
	
	/** The offset for reading/writing.
	 When it's equal to 0 for reading, it's equal to vector_size for writing,
	 and viceversa.
	 */
	int     read_offset;
	int     write_offset;
	
	/** The pointer to the sample data
	 */
	double	sample_data;
};




/** Initialize a ShaMe writer
    First argument is a reference to a pointer to a shame struct; second argument
    must not contain any backslash, not even at the beggining; the other arguments
    are self-explanatory.
    Returns error code, 0 if success
 */
t_shame_error init_shame_writer(struct shame **shared_mem, char name[], int *writer_number, int sample_rate, int vector_size, long nchannels);


/** Initialize a ShaMe reader
 First argument is a reference to a pointer to a shame struct; second argument
 must not contain any slash, not even at the beggining; the other arguments
 are self-explanatory.
 Returns error code, 0 if success
 */
int init_shame_reader(struct shame **shared_mem, char name[], int *reader_number, int sample_rate, int vector_size);


/** Dettach a ShaMe
 Returns error code, 0 if success
 */
int dettach_shame(struct shame *shared_mem, char name[], int instance_number, int is_reader);


/** Force unlink
 */
void force_unlink(char name[]);


/** Report how many writers are connected
 */
int writers_attached(struct shame *shared_mem);


/** Report how many readers are connected
 */
int readers_attached(struct shame *shared_mem);


/** Report how many connected clients match shame's sample rate
 */
int clients_match_sample_rate(struct shame *shared_mem);


/** Report how many connected clients match shame's vector size
 */
int clients_match_vector_size(struct shame *shared_mem);


/** Get writer's binary status.
 */
int get_writer_status(struct shame *shared_mem, int writer_id);


#endif /* shame_h */
