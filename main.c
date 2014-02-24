#include "memchunk.h"
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

void usage()
{
	extern char * __progname;
	fprintf(stderr, "usage: %s size\n", __progname);
	exit(1);
}

/* Lets pass the size of the memchunk array to the command line */
int main(int argc, char *argv[])
{
	int num_chunks;
 	int num_entries;
	int i;
	struct memchunk *mem_list = NULL;
	long temp;
	char * ep;

	/* ensure that we have passed the correct number of arguments */
	if (argc != 2)
		usage();
  
	/* set the size of the memchunk array */
	temp = strtol(argv[1], &ep, 10);
	if (*argv[1] == '\0' || *ep != '\0') {
		/* parameter wasn't a number, or was empty */
		fprintf(stderr, "%s - not a number\n", argv[1]);
		usage();
	}

	/* make sure that he wanted you to use a 32-bit integer  */
	if ((errno == ERANGE && (temp == LONG_MAX || temp == LONG_MIN)) || 
	    (temp > INT_MAX || temp < 0) || (errno != 0 && temp == 0) ){
		/* It's a number, but can't fit in an int 
		 * or it's just bigger than we want to allow
 		 */
		fprintf(stderr, "%s - value out of range\n", argv[1]);
		usage();	
	}

	num_entries = (int) temp;

	/* allocate memory for the memchunk structs */
	mem_list = malloc(num_entries * sizeof(struct memchunk) );
	if (mem_list == NULL)
		err(1, "malloc failed");
    
	num_chunks = get_mem_layout(mem_list, num_entries);

	printf("There are %d memory chunks in the process's address space.\n"
	    , num_chunks);

	if ( num_chunks < num_entries )
		num_entries = num_chunks;

	printf("MEMORY LAYOUT:\n\n");
	
	for ( i = 0 ; i < num_entries ; i++ ){
		printf("Address: %p\n", mem_list[i].start);
		printf("Length: 0x%x\n", mem_list[i].length );
		
		if ( mem_list[i].RW == 0 )
			printf("Access Type: R\n\n");
		else if ( mem_list[i].RW == 1 )
			printf("Access Type: RW\n\n");
		else 
			printf("Error!\n\n");
	} 

	printf("Here are the addresses of some variables and functions:\n\n");
	printf("Dynamically allocated:\n");
	printf("mem_list entries are at: %p\n\n", mem_list);

	printf("address of memlist is at: %p\n\n", &mem_list);

	printf("Stack variables:\n");
	printf("num_entries at: %p\n\n", &num_entries);

	printf("Function addresses:\n"); 
	printf("main at: %p\n", &main);
	printf("get_mem_layout at: %p\n\n", &get_mem_layout);

	printf("command line inputs:\n");
	printf("argv's contents are at: %p\n", argv);
	
	printf("address of argv is: %p\n", &argv);
	
	printf("argc is at: %p\n", &argc);

	printf("address of malloc function is: %p\n", malloc);
	printf("address of the ep pointer is: %p\n", &ep);

	/* Deallocate the memory */
	free(mem_list);
	return 0;
}
