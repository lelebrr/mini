/**
 * @file lv_mem.h
 *
 */

#ifndef LV_LEGACY_MEM_H
#define LV_LEGACY_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

/* Use the new stdlib-based memory API and string helpers */
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_string.h"

#include "lv_types.h"

/*********************
 *      DEFINES
 ******************_code**new*<//
/

/**********************
 *      TYPEDEFS
 **********************/

/* Legacy temporary buffer descriptor kept for backwards compatibility. */
typedef struct {
    void * p;
    uint16_t size;
    uint8_t used : 1;
} lv_mem_buf_t;

typedef lv_mem_buf_t lv_mem_buf_arr_t[LV_MEM_BUF_MAX_NUM];

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get a temporal buffer with the given size.
 * @param size the required size
 */
void * lv_mem_buf_get(uint32_t size);

/**
 * Release a memory buffer
 * @param p buffer to release
 */
void lv_mem_buf_release(void * p);

/**
 * Free all memory buffers
 */
void lv_mem_buf_free_all(void);

/**********************
 *      MACROS
 **********************/

/* Map legacy allocation API to the new stdlib-based functions */
#define lv_mem_alloc(size)   lv_malloc(size)
#define lv_mem_free(p)       lv_free(p)
#define lv_mem_realloc(p, s) lv_realloc((p), (s))

//! @cond Doxygen_Suppress
/* For legacy code that called lv_memset_00 directly, provide a thin wrapper
 * on top of lv_memzero implemented in lv_string.h. */
static inline void lv_memset_00(void * dst, size_t len)
{
    lv_memzero(dst, len);
}
//! @endcond

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LEGACY_MEM_H*/
