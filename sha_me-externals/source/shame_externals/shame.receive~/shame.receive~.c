/**
 Antonio Ortega Brook
	September 26 2017
 */


#include "ext.h"			// standard Max include, always required (except in Jitter)
#include "ext_obex.h"			// required for "new" style objects
#include "z_dsp.h"			// required for MSP objects

#include "shame.h"

// struct to represent the object's state
typedef struct _shame_receive_tilde {
	t_pxobject	ob;			// the object itself (t_pxobject in MSP instead of t_object)
	
	
	int		instantiated;
	// attributes
	t_symbol	name;
	t_int		nchannels;
	
	char		shame_name[256];
	int		reader_n;
	int		reader_initialized;
	int		shame_readable;
	
	struct shame *shame_read;
} t_shame_receive_tilde;


// method prototypes
void *shame_receive_tilde_new(t_symbol *s, long argc, t_atom *argv);
void shame_receive_tilde_free(t_shame_receive_tilde *x);
void shame_receive_tilde_assist(t_shame_receive_tilde *x, void *b, long m, long a, char *s);
void shame_receive_tilde_float(t_shame_receive_tilde *x, double f);
void shame_receive_tilde_dsp64(t_shame_receive_tilde *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void shame_receive_tilde_perform64(t_shame_receive_tilde *x, t_object *dsp64, double **ins, long numins, double **outs, long nchannels, long sampleframes, long flags, void *userparam);
void shame_receive_tilde_perform64_no_out(t_shame_receive_tilde *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);


void shame_receive_tilde_refresh(t_shame_receive_tilde *x);
void set_readable(t_shame_receive_tilde *x, int be_verbose);

//attr accessors
t_max_err shame_receive_tilde_name_get(t_shame_receive_tilde *x, void *attr, long *ac, t_atom **av);
t_max_err shame_receive_tilde_name_set(t_shame_receive_tilde *x, void *attr, long *argc, t_atom *argv);
t_max_err shame_receive_tilde_nchannels_set(t_shame_receive_tilde *x, void *attr, long *argc, t_atom *argv);


// notifications
t_max_err shame_receive_tilde_notify(t_shame_receive_tilde *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

// global class pointer variable
static t_class *shame_receive_tilde_class = NULL;


//***********************************************************************************************

void ext_main(void *r)
{
	// object initialization, note the use of dsp_free for the freemethod, which is required
	// unless you need to free allocated memory, in which case you should call dsp_free from
	// your custom free function.
	
	t_class *c = class_new("shame.receive~", (method)shame_receive_tilde_new, (method)shame_receive_tilde_free, (long)sizeof(t_shame_receive_tilde), 0L, A_GIMME, 0);
	
	class_addmethod(c, (method)shame_receive_tilde_dsp64,		"dsp64",	A_CANT, 0);
	class_addmethod(c, (method)shame_receive_tilde_assist,		"assist",	A_CANT, 0);
	
	class_addmethod(c, (method)shame_receive_tilde_refresh,		"refresh",		0);
	
	// notifications
	class_addmethod(c, (method)shame_receive_tilde_notify,		"notify",	A_CANT, 0);
	
	// Attributes
	CLASS_ATTR_SYM(c,	"name", 0, t_shame_receive_tilde, name);
	CLASS_ATTR_ACCESSORS(c, "name",	(method)shame_receive_tilde_name_get, (method)shame_receive_tilde_name_set);
	
	CLASS_ATTR_LONG(c,	"nchannels", 0, t_shame_receive_tilde, nchannels);
	CLASS_ATTR_ACCESSORS(c, "nchannels",	NULL, (method)shame_receive_tilde_nchannels_set);
	
	
	class_dspinit(c);
	class_register(CLASS_BOX, c);
	shame_receive_tilde_class = c;
}


void *shame_receive_tilde_new(t_symbol *s, long argc, t_atom *argv)
{
	t_shame_receive_tilde *x = (t_shame_receive_tilde *)object_alloc(shame_receive_tilde_class);
	
	if (!x)
		return NULL;
	
	x->instantiated		= false;
	x->reader_initialized	= false;
	x->nchannels		= 1;
	x->name			= *gensym("");
	
	attr_args_process(x, argc, argv);
	
	dsp_setup((t_pxobject *)x, 0);		// MSP inlets: arg is # of inlets and is REQUIRED!
						// use 0 if you don't need inlets
	for (int i = 0; i < x->nchannels; i++)
		outlet_new((t_pxobject *)x, "signal"); 		// signal outlet (note "signal" rather than NULL)
	
	
	x->instantiated = true;
	
	return (x);
}


void shame_receive_tilde_free(t_shame_receive_tilde *x)
{
	int dettach_result;
	
	dsp_free((t_pxobject *)x);
	
	dettach_result = dettach_shame(x->shame_read, x->name.s_name, x->reader_n,true);
	
	switch (dettach_result) {
		case E_SHAME_UNMAP_FAILED:
			object_error((t_object *)x, "Unmap failed for '%s'", x->name.s_name);
			break;
			
		case E_SHAME_NO_FILE_TO_UNLINK:
			object_error((t_object *)x, "No file to unlink for '%s'", x->name.s_name);
			break;
			
		case E_SHAME_UNLINK_FAILED:
			object_error((t_object *)x, "Unlink failed for '%s'", x->name.s_name);
			break;
	}
}


void shame_receive_tilde_assist(t_shame_receive_tilde *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { //inlet
		sprintf(s, "messages");
	}
	else {	// outlet
		sprintf(s, "output %ld", a);
	}
}

// notifications
t_max_err shame_receive_tilde_notify(t_shame_receive_tilde *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	return 0;
}



// registers a function for the signal chain in Max
void shame_receive_tilde_dsp64(t_shame_receive_tilde *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	// instead of calling dsp_add(), we send the "dsp_add64" message to the object representing the dsp chain
	// the arguments passed are:
	// 1: the dsp64 object passed-in by the calling function
	// 2: the symbol of the "dsp_add64" message we are sending
	// 3: a pointer to your object
	// 4: a pointer to your 64-bit perform method
	// 5: flags to alter how the signal chain handles your object -- just pass 0
	// 6: a generic pointer that you can use to pass any additional data to your perform method
	
	if (x->reader_initialized)
		set_readable(x, false);
	

	object_method(dsp64, gensym("dsp_add64"), x, shame_receive_tilde_perform64, 0, NULL);
}


// this is the 64-bit perform method audio vectors
void shame_receive_tilde_perform64(t_shame_receive_tilde *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
	t_double *outlets[numouts];	// we get audio for each outlet of the object from the **outs argument
	
	long n;
	
	if (x->shame_readable) {
		
		long active_outs = x->shame_read->nchannels < numouts ? x->shame_read->nchannels : numouts;
		
		int offset = x->shame_read->read_offset;
		double *samp_ptr = &x->shame_read->sample_data + offset;
		
		
		// read samples from shame
		for (long i = 0; i < active_outs; i++) {
			outlets[i] = outs[i];
			n = sampleframes;
			
			while (n--)
				*outlets[i]++ = *samp_ptr++;
		}
		
		// send 0 trhough the remaining outputs, if any
		for (long i = active_outs; i < numouts; i++) {
			outlets[i] = outs[i];
			n = sampleframes;
			
			while (n--)
				*outlets[i]++ = 0;
		}
	}

	else {
		for (long i = 0; i < numouts; i++) {
			outlets[i] = outs[i];
			n = sampleframes;
			
			while (n--)
				*outlets[i]++ = 0;
		}
	}
}

void shame_receive_tilde_perform64_no_out(t_shame_receive_tilde *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
	t_double *outlets[numouts];	// we get audio for each outlet of the object from the **outs argument
	long n;
	
	for (long i = 0; i < numouts; i++) {
		outlets[i] = outs[i];
		n = sampleframes;
		
		while (n--)
			*outlets[i]++ = 0;
	}
}

//--------------------------------------------------------------------------------------
// Attributes accessors

t_max_err shame_receive_tilde_name_set(t_shame_receive_tilde *x, void *attr, long *argc, t_atom *argv)
{
	int dettach_result;
	
	if (x->reader_initialized) {
		x->shame_readable = false;
		dettach_result = dettach_shame(x->shame_read, x->name.s_name, x->reader_n,true);
		
		if (dettach_result != E_SHAME_DETTACH_SUCCESS)
			return MAX_ERR_GENERIC;
		
		x->reader_initialized = false;
	}
	
	x->name = *atom_getsym(argv);
	
	if (strcmp(x->name.s_name, ""))
		shame_receive_tilde_refresh(x);
	
	return MAX_ERR_NONE;
}

t_max_err shame_receive_tilde_name_get(t_shame_receive_tilde *x, void *attr, long *ac, t_atom **av)
{
	if (ac && av) {
		char alloc;
		
		if (atom_alloc(ac, av, &alloc)) {
			return MAX_ERR_GENERIC;
		}
		atom_setsym(*av, &x->name);
	}
	return MAX_ERR_NONE;
}

t_max_err shame_receive_tilde_nchannels_set(t_shame_receive_tilde *x, void *attr, long *argc, t_atom *argv)
{
	if (x->instantiated)
		object_error((t_object *)x, "Number of outputs cannot be changed dinamically");
	else
		x->nchannels = atom_getlong(argv);
	
	return MAX_ERR_NONE;
}
//--------------------------------------------------------------------------------------

void shame_receive_tilde_refresh(t_shame_receive_tilde *x)
{
	int init_result;
	
//	x->shame_readable = false;
	if (!x->reader_initialized) {
		init_result = init_shame_reader(&x->shame_read, x->name.s_name, &x->reader_n, sys_getsr(), sys_getblksize());
		
		switch (init_result) {
			case E_SHAME_INIT_READER_SUCCESS:
				object_post((t_object *)x, "Connected to %s", x->name.s_name);
				break;
				
			case E_SHAME_DOESNT_EXIST:
				object_post((t_object *)x, "No writer found whith name '%s'. Refresh object after writer's creation", x->name);
				break;
				
			case E_SHAME_MAP_FAILED:
				object_error((t_object *)x, "Map failed for %s (error %d)", x->name.s_name, init_result);
				break;
				
			case E_SHAME_REMAP_FAILED_UNMAP:
				object_error((t_object *)x, "Unmap failed for %s (error %d)", x->name.s_name, init_result);
				break;
				
			case E_SHAME_REMAP_FAILED_MAP:
				object_error((t_object *)x, "Remap failed for %s (error %d)", x->name.s_name, init_result);
				break;
		}
		
		x->reader_initialized = (init_result == E_SHAME_INIT_READER_SUCCESS);
	}
	
	if (x->reader_initialized)
		set_readable(x, true);
}




void set_readable(t_shame_receive_tilde *x, int be_verbose)
{
	int previous_state = x->shame_readable;
	
	int match_sample_rate;
	int match_vector_size;
	
	x->shame_read->reader_sample_rate[x->reader_n] = sys_getsr();
	if (x->shame_read->sample_rate != sys_getsr()) {
		
		match_sample_rate = false;
		
		if (match_sample_rate != previous_state || be_verbose)
			object_warn((t_object *)x, "%s doesn't match sample rate", x->name.s_name);
		
	} else
		match_sample_rate = true;
	
	
	x->shame_read->reader_vector_size[x->reader_n] = sys_getblksize();
	if (x->shame_read->vector_size != sys_getblksize()) {
		
		match_vector_size = false;
		
		if (match_vector_size != previous_state || be_verbose)
			object_warn((t_object *)x, "%s doesn't match vector size", x->name.s_name);
		
	} else
		match_vector_size = true;
	
	
	x->shame_readable = (match_sample_rate && match_vector_size);
	
	if (x->shame_readable && (!previous_state || be_verbose))
		object_post((t_object *)x, "%s is readable", x->name.s_name);
}
