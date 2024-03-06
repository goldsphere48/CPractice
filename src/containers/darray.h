#define DARRAY_RESIZE_FACTOR 2

typedef struct
{
	int length;
	int capacity;
	int stride;
} darray_header;

void *darray_create(int length, int stride);
void darray_free(void *darray);
void *darray_resize(void *darray);
void *darray_push_back(void *darray, const void *value_ptr);
void darray_pop_back(void *darray, void *dest);
void darray_set_at(void *darray, int index, const void *vlaue_ptr);
void *darray_get_at(void *darray, int index);

int darray_length(void *darray);
int darray_capacity(void *darray);
int darray_stride(void *darray);