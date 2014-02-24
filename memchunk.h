#ifndef MEMCHUNK_H
#define MEMCHUNK_H

struct memchunk {
	void *start;
	unsigned long length;
	int RW;
};

int get_mem_layout(struct memchunk *chunk_list, int size);

#endif
