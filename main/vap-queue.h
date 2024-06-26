_Pragma("once");

#include <stddef.h>
#include <stdbool.h>

typedef struct vap_list_node_s           vap_queue_t;
typedef struct vap_list_node_s           vap_queue_node_t;

struct vap_list_node_s {
	struct vap_list_node_s* p;
	struct vap_list_node_s* n;
};

#define vap_queue_data(x, t, m)                              \
    ((t *) ((char *) (x) - offsetof(t, m)))

extern void              vap_queue_init(vap_queue_t* q);
extern void              vap_queue_enqueue(vap_queue_t* q, vap_queue_node_t* x);
extern vap_queue_node_t* vap_queue_dequeue(vap_queue_t* q);
extern bool              vap_queue_empty(vap_queue_t* q);