#include "AOI.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define DEFAULT_AOI_RADIS 12.5f
#define INVALID_ID (~0)

float distance2(float p1[2] , float p2[2]) {
	float d_0 = p1[0] - p2[0];
	float d_1 = p1[1] - p2[1];
	return d_0 * d_0 + d_1 * d_1;
}

static char *flag_mat = NULL;

static char * get_flag(int a , int b) {
	if (a > b) {
		a ^= b;
		b ^= a;
		a ^= b;
	}

	return flag_mat + a + b * ( b - 1 ) / 2;
}


void dump_flags(int len) {
	puts("dump_flags:");

	for (int i = 0; i < len; ++i)
	{
		putchar(flag_mat[i] ? '1' : '0');
	}

	putchar('\n');
}


static aoi_Callback *g_cb_enter_view = NULL;
static aoi_Callback *g_cb_leave_view = NULL;

struct object {
	float pos[2];
};

struct obj_list;

struct obj_slot {
	int id;
	float pos[2];

	struct aoi_space * space;
	struct obj_list * owner_list;
	int index;
	struct obj_slot * prev;
	struct obj_slot * next;
};

struct obj_list {
	int size;
	struct obj_slot * head;
};

void push_slot_head(struct obj_list * l , struct obj_slot * o) {
	if (l->head) {
		l->head->prev = o;
	}

	o->next = l->head;
	l->head = o;

	++l->size;
}

void remove_slot(struct obj_list * l , struct obj_slot * o) {
	--l->size;

	struct obj_slot * t = o->prev;
	if (t == NULL) {
		l->head = o->next;
	} else {
		t->next = o->next;
	}

	t = o->next;
	if (t != NULL) {
		t->prev = o->prev;
	}

	o->prev = o->next = NULL;
}


struct obj_slot * obj_pool_slots;
struct obj_list obj_pool;

struct obj_slot * get_slot(int index) {
	return obj_pool_slots + index;
}

struct obj_slot * alloc_slot() {
	if (obj_pool.size <= 0) {
		return NULL;
	}

	--obj_pool.size;

	struct obj_slot * ret = obj_pool.head;
	obj_pool.head = ret->next;
	obj_pool.head->prev = NULL;
	ret->next = NULL;
	return ret;
}


void free_slot(struct obj_slot * slot) {
	push_slot_head(&obj_pool , slot);
}


void aoi_initialize(int obj_limit) {
	obj_pool_slots = (struct obj_slot *)malloc(sizeof(struct obj_slot) * obj_limit);
	struct obj_slot * slot = NULL;
	obj_pool.size = obj_limit;
	obj_pool.head = obj_pool_slots;

	int i = 0;
	for ( ; i < obj_limit; ++i) {
		slot = obj_pool_slots + i;
		slot->owner_list = &obj_pool;
		slot->index = i;
		slot->prev = obj_pool_slots + i - 1;
		slot->next = obj_pool_slots + i + 1;
	}
	obj_pool_slots[0].prev = NULL;
	obj_pool_slots[obj_limit - 1].next = NULL;

	int flag_size = obj_limit * ( obj_limit + 1 ) / 2;
	flag_mat = (char *)malloc(flag_size);
	memset(flag_mat , 0 , flag_size);
}


void aoi_cleanup() {
	free(obj_pool_slots);
}


static void *
	default_alloc(void * ud, void *ptr, size_t sz) {
		if (ptr == NULL) {
			void *p = malloc(sz);
			return p;
		}
		free(ptr);
		return NULL;
}


struct aoi_space {
	aoi_Alloc alloc;
	void * alloc_ud;
	struct obj_list * objects;

	float aoi_radius;
	float aoi_radius2;
	float hot_radius2;
};


struct aoi_space * 
	aoi_create(aoi_Alloc alloc, void *ud) {
		struct aoi_space *space = (struct aoi_space*)alloc(ud, NULL, sizeof(struct aoi_space));
		space->alloc = alloc;
		space->alloc_ud = ud;

		space->objects = (struct obj_list *)alloc(ud, NULL, sizeof(struct obj_list));
		space->objects->head = NULL;
		space->objects->size = 0;

		set_aoi_radius(space , DEFAULT_AOI_RADIS);
		return space;
}


void aoi_release(struct aoi_space * space)
{
	space->alloc(space->alloc_ud , space->objects , sizeof(struct obj_list));
	space->alloc(space->alloc_ud , space , sizeof(struct aoi_space));
}


int aoi_insert(struct aoi_space * space , int id , float pos[2]) {
	struct obj_slot *slot = alloc_slot();
	if (slot == NULL)
		return INVALID_ID;

	slot->id = id;
	slot->space = space;
	slot->pos[0] = pos[0];
	slot->pos[1] = pos[1];
	push_slot_head(space->objects , slot);
	return slot->index;
}


void aoi_remove(struct aoi_space * space , int index , void *ud) {
	if (g_cb_enter_view == NULL || g_cb_leave_view == NULL)
		return;

	struct obj_slot *slot = get_slot(index);
	remove_slot(space->objects , slot);
	free_slot(slot);

	int obj_count = space->objects->size;
	char *flag;
	struct obj_slot * slot1 = space->objects->head;

	int i = 0;
	for ( ; i < obj_count ; ++i)
	{
		flag = get_flag(slot1->index , index);
		if (*flag) {
			*flag = 0;
			// g_cb_leave_view(ud , slot1->id , slot->id);
		}

		slot1 = slot1->next;
	}
}


void aoi_update(struct aoi_space * space , int index , float pos[2] , void *ud) {
	struct obj_slot *slot = get_slot(index);
	slot->pos[0] = pos[0];
	slot->pos[1] = pos[1];
	if (slot->space != space) {
		aoi_remove(slot->space , index , ud);
		aoi_insert(space , slot->id , pos);
	}
}


void aoi_message(struct aoi_space *space, void *ud)
{
	if (g_cb_enter_view == NULL || g_cb_leave_view == NULL)
		return;

	int obj_count = space->objects->size;
	if (obj_count <= 1)
		return;

	struct obj_slot * head = space->objects->head;
	struct obj_slot * slot1 = head->next;
	struct obj_slot * slot2 = NULL;
	int s1_index , s2_index;
	float *p1 = NULL;
	float view_radius2 = space->aoi_radius2;
	float hot_radius2 = space->hot_radius2;
	char *flag = NULL;
	float dist2 = 0;

	int i = 1 , j = 0;
	for ( ; i < obj_count ; ++i)
	{
		s1_index = slot1->index;
		p1 = slot1->pos;
		slot2 = head;

		for (j = 0 ; j < i ; ++j)
		{
			s2_index = slot2->index;
			flag = get_flag(s1_index , s2_index);
			dist2 = distance2(p1 , slot2->pos);
			if (dist2 < view_radius2) {
				if (*flag == 0) {
					*flag = 1;
					g_cb_enter_view(ud , slot1->id , slot2->id);
					// printf(" > > g_cb_enter_view %d (%.1f,%.1f) | %d (%.1f,%.1f) dist2 = %f\n" ,
					// 	slot1->id , slot1->pos[0] , slot1->pos[1] ,  slot2->id , slot2->pos[0] , slot2->pos[1] , dist2);
				}
			} else if (dist2 > hot_radius2) {
				if (*flag) {
					*flag = 0;
					g_cb_leave_view(ud , slot1->id , slot2->id);
					// printf(" > > g_cb_leave_view %d (%.1f,%.1f) | %d (%.1f,%.1f) dist2 = %f\n" ,
					// 	slot1->id , slot1->pos[0] , slot1->pos[1] ,  slot2->id , slot2->pos[0] , slot2->pos[1] , dist2);
				}
			}

			slot2 = slot2->next;
		}

		slot1 = slot1->next;
	}
}


struct aoi_space * 
	aoi_new() {
		return aoi_create(default_alloc, NULL);
}

void
	set_aoi_radius(struct aoi_space *space, float radius) {
		space->aoi_radius = radius;
		space->aoi_radius2 = radius * radius;
		space->hot_radius2 = (radius + 2) * (radius + 2);
}

float
	get_aoi_radius(struct aoi_space *space) {
		return space->aoi_radius;
}

void
	set_message_callback(aoi_Callback *cb_enter_view , aoi_Callback *cb_leave_view) {
		g_cb_enter_view = cb_enter_view;
		g_cb_leave_view = cb_leave_view;
}

