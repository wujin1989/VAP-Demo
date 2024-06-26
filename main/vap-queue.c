#include "vap-queue.h"

typedef struct vap_list_node_s           vap_list_node_t;
typedef struct vap_list_node_s           vap_list_t;

void vap_list_init(vap_list_t* l) {
	(l)->n = (l);
	(l)->p = (l);
}

void vap_list_insert_head(vap_list_t* l, vap_list_node_t* x) {
	(x)->n = (x);
	(x)->p = (x);
	(x)->n = (l)->n;
	(x)->n->p = (x);
	(x)->p = (l);
	(l)->n = (x);
}

void vap_list_insert_tail(vap_list_t* l, vap_list_node_t* x) {
	(x)->n = (x);
	(x)->p = (x);
	(x)->p = (l)->p;
	(x)->p->n = (x);
	(x)->n = (l);
	(l)->p = (x);
}

bool vap_list_empty(vap_list_t* l) {
	return (l) == (l)->p;
}

vap_list_node_t* vap_list_head(vap_list_t* l) {
	return (l)->n;
}

vap_list_node_t* vap_list_tail(vap_list_t* l) {
	return (l)->p;
}

vap_list_node_t* vap_list_sentinel(vap_list_t* l) {
	return (l);
}

void vap_list_remove(vap_list_node_t* x) {
	(x)->n->p = (x)->p;
	(x)->p->n = (x)->n;
}

vap_list_node_t* vap_list_next(vap_list_node_t* x) {
	return (x)->n;
}

vap_list_node_t* vap_list_prev(vap_list_node_t* x) {
	return (x)->p;
}

void vap_queue_init(vap_queue_t* q) {
	vap_list_init(q);
}

void vap_queue_enqueue(vap_queue_t* q, vap_queue_node_t* x) {
	vap_list_insert_tail(q, x);
}

bool vap_queue_empty(vap_queue_t* q) {
	return vap_list_empty(q);
}

vap_queue_node_t* vap_queue_dequeue(vap_queue_t* q) {
	if (vap_queue_empty(q)) {
		return NULL;
	}
	vap_queue_node_t* n = vap_list_head(q);
	vap_list_remove(n);
	return n;
}