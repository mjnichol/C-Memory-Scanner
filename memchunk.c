#include "memchunk.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <err.h>

static sigjmp_buf jumpbuf;

static struct sigaction sa, oldsa;

/* stores the type of memory access: -1 = fail, 0 = read, 1 = write */
static sig_atomic_t cur_a_type;
static sig_atomic_t last_a_type;

static sig_atomic_t read_flag;

/* Define my handler */
static void handler(int signum) {

	if ( read_flag == 0 )
		cur_a_type = -1;
	siglongjmp(jumpbuf, 1);
}

int get_mem_layout (struct memchunk * chunk_list, int size){

	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask); 
  	sa.sa_flags = 0;
  	if( sigaction(SIGSEGV, NULL, &oldsa) == -1) 
		err(1, "Cannot save old sigaction");
  	if( sigaction(SIGSEGV, &sa, NULL) == -1) 
    		err(1, "Cannot do the new sigaction");

  	int page_size = getpagesize(); /* Get the page size on our machine */
  	int first_entry = 0;
  	int i;

  	int *ptr = 0x0; /* start from the bottom of the address space */
  	int temp; /* Try to read from memory into this variable */ 	
	
  	int index = 0;
	last_a_type = -2; /* Force a failure on the first check */

	read_flag = 0;
  
  	for(i = 0 ; i < 0x100000; i++){
	  
    		if (sigsetjmp(jumpbuf, 1 ) == 0 ){
			
      			/* Try to read from the memory */
      			temp = *ptr;
			
			/* Read was successful! */
			cur_a_type = 0;			
			read_flag = 1;
				
      			/* write to memory */
     	 		*ptr = temp;
			cur_a_type = 1;			

    		}	
		
		/* update the arrays! */

		/* Increase the size in bytes */
		if ( cur_a_type == last_a_type && ( index < size ) 
		    && cur_a_type != -1 )
			chunk_list[index].length+= page_size;
		/* Make a new entry! */
		if ( cur_a_type != last_a_type && cur_a_type != -1 ){
			
			if ( !first_entry ){
				first_entry = 1;
				if ( index < size){
					chunk_list[0].start = ptr;
					chunk_list[0].RW = cur_a_type;
					chunk_list[0].length = page_size;
				}
			}
			
			/* Fill the array if it fits! */
			else if ( first_entry ){
				index++;	
				if ( index < size ){
					chunk_list[index].start = ptr;
					chunk_list[index].RW =cur_a_type;
					chunk_list[index].length = page_size;
				}
			}
		}
		
		last_a_type = cur_a_type;
		read_flag = 0;
    		
    		ptr+= (page_size >> 2); 
	}

  	/* restore the old signal handler */
  	if (sigaction(SIGSEGV, &oldsa, NULL) == -1)
    		err(1, "cannot restore the old signal handler.");

	if ( !first_entry )
		return 0;
	else
  		return index + 1;
};

