

typedef struct _meta {
//there is a bug in gcc

#define MINCHUNKSIZE (2 * sizeof(mem) + 16)


	mem * prev = (void *) &free_list_ptr;
	mem * curr_good_chunk = NULL;
	mem * prev_good_chunk = NULL;

	unsigned int min_chunk_size = 0;
	void * heap_top = NULL;
	void * heap_top_new = NULL;
	while(finder!=NULL)
	{
			 && (!curr_good_chunk || min_chunk_size > chunk_size))
		{
		prev = finder;
		finder = finder->next;

	{
		{ /* split block */
		else
		{
			prev_good_chunk->next = curr_good_chunk->next;
		}
	{
			&& (void *)(free_list_ptr+1) + free_list_ptr->len == heap_top) 
		{
		{
		}

	mem * head = ptr - sizeof(mem);
	mem * prev = (void*)&free_list_ptr;
	void * finder_next = NULL;
	void * head_next = ptr + head->len;

	while(1){
		{
		finder_next = finder ? ((void *) (finder+1)) + finder->len : 0;
		if (finder_next == (void *)head)
		{
		{
		prev = finder;
		finder = finder->next;
	if (!size)
	{
		free(ptr);
		return NULL;
	}

	mem * prev = (mem *) (ptr - sizeof(mem));

	void * newptr;
	void * finder;
	free(ptr);
	{
	}

	{