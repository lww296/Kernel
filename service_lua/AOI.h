#ifndef __AOI_H__
#define __AOI_H__

#include <stdint.h>
#include <stddef.h>

typedef void * (*aoi_Alloc)(void *ud, void * ptr, size_t sz);
typedef void (aoi_Callback)(void *ud, uint32_t watcher, uint32_t marker);

void aoi_initialize(int obj_limit);
void aoi_cleanup();

struct aoi_space;

struct aoi_space * aoi_create(aoi_Alloc alloc, void *ud);
struct aoi_space * aoi_new();
void aoi_release(struct aoi_space *);

int aoi_insert(struct aoi_space * space , int id , float pos[2]);
void aoi_remove(struct aoi_space * space , int index , void *ud);

// w(atcher) m(arker) d(rop)
void aoi_update(struct aoi_space * space , int index , float pos[2] , void *ud);
void aoi_message(struct aoi_space *space, void *ud);

void set_aoi_radius(struct aoi_space *space, float radius);
float get_aoi_radius(struct aoi_space *space);

void set_message_callback(aoi_Callback cb_enter_view , aoi_Callback cb_leave_view);

void dump_flags(int len);

#endif //__AOI_H__
