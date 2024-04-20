#include "inc_stripped.h"

void init_g(void) {
	set_ptr_to_globals((char*)xzalloc(sizeof(G1)+sizeof(G)) + sizeof(G1));
	// The above line allocates memory for G1 and G, and then move the pointer to the end of G1.
	// That is to say, the pointer points to the start of G.
	// Now `ptr_to_globals[-1]` gives the start of G1, and `ptr_to_globals[0]` gives the start of G.
	G.next_token__ltclass = TC_OPTERM;
	G.evaluate__seed = 1;
}

// This is a helper function to print the value of global intvar array.
void print_intvar(void) 
{
	// loop the intvar array and print its addr and its pointed var_s struct
	// typedef struct var_s {
	// 	unsigned type;            /* flags */
	// 	double number;
	// 	char *string;
	// 	union {
	// 		int aidx;               /* func arg idx (for compilation stage) */
	// 		struct xhash_s *array;  /* array ptr */
	// 		struct var_s *parent;   /* for func args, ptr to actual parameter */
	// 		walker_list *walker;    /* list of array elements (for..in) */
	// 	} x;
	// } var;
	for (int i = 0; i < 19; i++) {
		printf("intvar[%d]: \n", i);
		if (intvar[i] == NULL) {
			continue;
		} else{
			print_var(intvar[i]);
		}
	}
}

// This is a helper function to print `var_s` struct.
void print_var(struct var_s *v) {
	printf("var addr: %p\n", v);
	printf("type: %u\n", v->type);
	printf("number: %f\n", v->number);
	printf("string: %s\n", v->string);
	printf("x.aidx: %d\n", v->x.aidx);
	printf("x.array: %p\n", v->x.array);
	printf("x.parent: %p\n", v->x.parent);
	printf("x.walker: %p\n", v->x.walker);
	printf("\n");
}

// This is a helper function to print `rstream_s` struct.
void print_rstream(struct rstream_s *rs) {
	// typedef struct rstream_s {
	// 	FILE *F;
	// 	char *buffer;
	// 	int adv;
	// 	int size;
	// 	int pos;
	// 	smallint is_pipe;
	// } rstream;
	printf("rstream addr: %p\n", rs);
	printf("F: %p\n", rs->F);
	printf("buffer: %s\n", rs->buffer);
	printf("adv: %d\n", rs->adv);
	printf("size: %d\n", rs->size);
	printf("pos: %d\n", rs->pos);
	printf("is_pipe: %d\n", rs->is_pipe);
	printf("\n");
}