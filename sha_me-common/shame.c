/**
	shame.c

	Created by Antonio Ortega Brook on 9/27/17.
	Copyright © 2017 Antonio Ortega Brook. All rights reserved.

	* NOTE:
	* Avoid C++ reserved words, so it can be compiled as C++ too.
*/


#include "shame.h"



/** Initialize a ShaMe writer
    First argument is a reference to a pointer to a shame struct; second argument
    must not contain any slash, not even at the beggining; the other arguments
    are self-explanatory.
    Returns error code, 0 if success
 */
t_shame_error init_shame_writer(struct shame **shared_mem, char name[], int *writer_number, int sample_rate, int vector_size, long nchannels)
{
        int writer_i;
        int fd_shm;
        int truncate_err;
        struct shame *tmp_shared_mem;
        
        size_t file_size		= MULT_PAGESIZE * getpagesize();
        size_t fixed_size		= sizeof(struct shame) - sizeof(double);
        size_t sample_data_size		= 2 * sizeof(double) * vector_size * nchannels;
        size_t total_number_of_samples	= vector_size * nchannels;
        size_t map_size			= fixed_size + sample_data_size;
	
	t_shame_error return_value;
        
        char formatted_name[256];
        sprintf(formatted_name, "/%s", name);
        
        // Check args
        if (fixed_size + sample_data_size > file_size)
                return E_SHAME_TOO_MUCH_DATA_TO_WRITE;
        
        // Open file without creating
        fd_shm = shm_open(formatted_name, O_RDWR, 0660);
        
        
        /** If the file already exists, we open it so we can read metadata
         */
        if (fd_shm != -1) {	// File already exists
                tmp_shared_mem = (struct shame *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
                
                if (tmp_shared_mem == MAP_FAILED)
                        return E_SHAME_MAP_FAILED;
		
		/** If there are no other writers attached, we own the shame; else, we don´t,
		    which means we can read but we should not write
		 */
		if (!writers_attached(tmp_shared_mem))
			goto label_set_writer_data;
		else {
			return_value = E_SHAME_READABLE_BUT_NOT_WRITABLE;
			goto label_return;
		}
        }
	
        /** If the file doesn't exists, we create it and initialize the struct. If something fails,
         we return an error code. If not, we return 0, which means the caller can write sample data
         */
        fd_shm = shm_open(formatted_name, O_RDWR | O_CREAT | O_EXCL, 0660);
        
        if (fd_shm == -1)
                return E_SHAME_COULDNT_CREATE;
        
        // File successfully created. Set size
        truncate_err = ftruncate(fd_shm, 2 * file_size);
        
        if (truncate_err == -1)
                return E_SHAME_TRUNCATE_FAILED;
	
        // File successfully truncated. Map
        tmp_shared_mem = (struct shame *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
        
        if (tmp_shared_mem == MAP_FAILED)
                return E_SHAME_MAP_FAILED;
        
//label_init:
	sprintf(tmp_shared_mem->name, "%s", name);

        for (int i = 0; i < MAX_READERS; i++) {
                tmp_shared_mem->reader_pid[i] = 0;
                tmp_shared_mem->writer_pid[i] = 0;
        }

	tmp_shared_mem->attached_readers = 0;
	tmp_shared_mem->attached_writers = 0;

label_set_writer_data:

	tmp_shared_mem->sample_rate		= sample_rate;
        tmp_shared_mem->vector_size		= vector_size;
        tmp_shared_mem->nchannels		= nchannels;
        tmp_shared_mem->sample_data_size	= sample_data_size;
        tmp_shared_mem->total_number_of_samples	= total_number_of_samples;
        tmp_shared_mem->map_size		= map_size;
        tmp_shared_mem->read_offset             = vector_size;
        tmp_shared_mem->write_offset            = 0;
	
	return_value = E_SHAME_INIT_WRITER_SUCCESS;
        
label_return:

	// Find the first empty position in PIDs array; this will be our instance index
	writer_i = 0;
	while (tmp_shared_mem->writer_pid[writer_i] != EMPTY)
		writer_i++;
	// (now writer_i = first empty position)
	
	tmp_shared_mem->writer_pid[writer_i] = getpid();
        
        if (!tmp_shared_mem->attached_writers)
                tmp_shared_mem->owner_id = writer_i;
        
	tmp_shared_mem->attached_writers += 1;
	
	// Return
	*shared_mem = tmp_shared_mem;
	*writer_number = writer_i;
	
	return return_value;
}





/** Initialize a ShaMe reader
    First argument is a reference to a pointer to a shame struct; second argument
    must not contain any slash, not even at the beggining; the other arguments
    are self-explanatory.
    Returns error code, 0 if success
 */
int init_shame_reader(struct shame **shared_mem, char name[], int *reader_number, int sample_rate, int vector_size)
{
	int reader_i;
	int fd_shm;
	struct shame *tmp_shared_mem;
	int should_remap = true;
	pid_t our_pid = getpid();
	
	size_t map_size;
	
	char formatted_name[256];
	
	sprintf(formatted_name, "/%s", name);
	
	// Get shared memory
	if ((fd_shm = shm_open(formatted_name, O_RDWR, 0660)) == -1)
		return E_SHAME_DOESNT_EXIST;	// File doesn't exist
	
	// Successfully opened, map once to find full size
	tmp_shared_mem = (struct shame *)mmap(NULL, sizeof(struct shame), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
	
	if (tmp_shared_mem == MAP_FAILED)
		return E_SHAME_MAP_FAILED;	// First map failed
	
	
	
	// Successfully mapped
	// Find the first empty position in PIDs array; this will be our instance index
	reader_i = 0;
	while (tmp_shared_mem->reader_pid[reader_i] != EMPTY) {
		
		// Check if we should remap (if our process has already mapped, we don't do it again)
//		if (tmp_shared_mem->reader_pid[reader_i] == our_pid)
//			should_remap = false;
		
		reader_i++;
	}
	// (now reader_i = first empty position)
	
	
	
	if (should_remap) {
		// Get total size
		map_size = tmp_shared_mem->map_size;
		
		// Unmap...
		if (munmap(tmp_shared_mem, sizeof(struct shame)) == -1)
			return E_SHAME_REMAP_FAILED_UNMAP;	// Unmap failed
		
		// ...and remap
		tmp_shared_mem = (struct shame *)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
		
		if (tmp_shared_mem == MAP_FAILED)
			return E_SHAME_REMAP_FAILED_MAP;	// Remap failed
		
	}
	
	
	// Fill
	tmp_shared_mem->reader_sample_rate[reader_i]	= sample_rate;
	tmp_shared_mem->reader_vector_size[reader_i]	= vector_size;
	tmp_shared_mem->reader_pid[reader_i]		= our_pid;

	tmp_shared_mem->attached_readers += 1;
	
	
	// Return
	*shared_mem = tmp_shared_mem;
        *reader_number = reader_i;
	
	return E_SHAME_INIT_READER_SUCCESS;
}


/** Dettach a ShaMe
    Returns error code, 0 if success
 */
int dettach_shame(struct shame *shared_mem, char name[], int instance_number, int is_reader)
{
        int should_unmap = false;
	int should_unlink;
	int instance_i, aux_i, pid_count;
	struct shame *tmp_shared_mem = shared_mem;
	
	size_t map_size;
	
	char formatted_name[256];
	sprintf(formatted_name, "/%s", name);
        
        
        if (shm_open(formatted_name, O_RDWR, 0660) == -1)
                return 0;	// File doesn't exist
	
	
	
	/** Find out if there are more readers/writers in the same process. If this is the only one,
	    we unmpap; if not, we only decrement count in attached_readers/writers
	 */
	instance_i = aux_i = pid_count = 0;
	
	if (is_reader) {
		
		while (instance_i < tmp_shared_mem->attached_readers) {
			
			if (tmp_shared_mem->reader_pid[aux_i]) {
				
				if (tmp_shared_mem->reader_pid[aux_i] == (tmp_shared_mem->reader_pid[instance_number]))
					pid_count++;
				
				instance_i++;
			}
			aux_i++;
		}
		
		should_unmap = (pid_count <= 1);
		tmp_shared_mem->reader_pid[instance_number] = 0;
		tmp_shared_mem->attached_readers -= 1;
	
	} else { // (is writer)
                if (instance_number == tmp_shared_mem->owner_id) // <- set all samples to 0
                        memset(&tmp_shared_mem->sample_data, 0, tmp_shared_mem->sample_data_size);
                
		while (instance_i < tmp_shared_mem->attached_writers) {
			
			if (tmp_shared_mem->writer_pid[aux_i]) {
				
				if (tmp_shared_mem->writer_pid[aux_i] == (tmp_shared_mem->writer_pid[instance_number]))
					pid_count++;
				
				instance_i++;
			}
			aux_i++;
		}
		
		should_unmap = (pid_count <= 1);
		tmp_shared_mem->writer_pid[instance_number] = 0;
		tmp_shared_mem->attached_writers -= 1;
	}
	
	/** If there are no more readers or writers attached to this shame, we should also unlink
	 */
        should_unlink   = (!tmp_shared_mem->attached_readers && !tmp_shared_mem->attached_writers);
	
	
        if (should_unmap) {
		
		map_size = tmp_shared_mem->map_size;
		
                if (munmap(tmp_shared_mem, map_size) == -1)
                        return E_SHAME_UNMAP_FAILED;
        }
	
	
	if (should_unlink) {

		if (shm_open(formatted_name, O_RDWR, 0660) == -1)
			return E_SHAME_NO_FILE_TO_UNLINK;	// No file to unlink
		
		if (shm_unlink(formatted_name) == -1)
			return E_SHAME_UNLINK_FAILED;	// Unlink failed
	}
	
	return E_SHAME_DETTACH_SUCCESS;
}



/** Force unlink
 */
void force_unlink(char name[])
{
	char formatted_name[256];
	sprintf(formatted_name, "/%s", name);
	
	shm_unlink(formatted_name);
}



/** Report how many writers are connected
 */
int writers_attached(struct shame *shared_mem)
{
	int writers;
	
	/** Acá es donde deberíamos checkear que efectivamente estén activos
	 */
	
	writers = shared_mem->attached_writers;
	
	return writers;
}



/** Report how many readers are connected
 */
int readers_attached(struct shame *shared_mem)
{
	int readers;
	
	/** Acá es donde deberíamos checkear que efectivamente estén activos
	 */
	
	readers = shared_mem->attached_readers;
	
	return readers;
}


/** Report how many connected clients match shame's sample rate
 */
int clients_match_sample_rate(struct shame *shared_mem)
{
	int	reader_count, reader_n;
	int	match = 0;
	
	int	attached_readers = shared_mem->attached_readers;
	double	sample_rate = shared_mem->sample_rate;
	
	reader_count = 0;
	reader_n = 0;
	while (reader_count < attached_readers) {
		if (shared_mem->reader_pid[reader_n]) {
			if (sample_rate == shared_mem->reader_sample_rate[reader_n])
				match++;
			reader_count++;
		}
		reader_n++;
	}
	return match;
}


/** Report how many connected clients match shame's vector size
 */
int clients_match_vector_size(struct shame *shared_mem)
{
	int	reader_count, reader_n;
	int	match = 0;
	
	int	attached_readers = shared_mem->attached_readers;
	int	vector_size = shared_mem->vector_size;
	
	reader_count = 0;
	reader_n = 0;
	while (reader_count < attached_readers) {
		if (shared_mem->reader_pid[reader_n]) {
			if (vector_size == shared_mem->reader_vector_size[reader_n])
				match++;
			reader_count++;
		}
		reader_n++;
	}
	return match;
}



/** Get writer's binary status.
 */
int get_writer_status(struct shame *shared_mem, int writer_id)
{
        int match_sr;
        int match_vs;
        int bin_status = S_UNINITIALIZED;
        
        
        bin_status |= F_INITIALIZED;
        if (!shared_mem)
                return bin_status;
        
        
        bin_status |= F_CAN_READ;
        if (shared_mem->owner_id != writer_id)
                return bin_status;
        
        
        bin_status |= F_CAN_WRITE;
        if (!shared_mem->attached_readers)
                return bin_status;
        
        
        match_sr = clients_match_sample_rate(shared_mem);
        match_vs = clients_match_vector_size(shared_mem);
        
        
        bin_status |= F_READERS_ATTACHED;
        if (!match_vs || !match_sr)
                return bin_status;
        
        
        bin_status |= F_SOME_READERS_CAN_READ;
        if (match_vs < shared_mem->attached_readers || match_sr < shared_mem->attached_readers)
                return bin_status;
        
        
        bin_status |= F_ALL_READERS_CAN_READ;
        
        return bin_status;
}
