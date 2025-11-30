/**
 * @file lv_mem.c
 * Legacy temporary buffer helpers built on top of the stdlib memory API.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_mem.h"
#include "lv_gc.h"
#include "lv_assert.h"
#include "lv_log.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#if LV_LOG_TRACE_MEM
    #define MEM_TRACE(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define MEM_TRACE(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Get a temporal buffer with the given size.
 * @param size the required size
 */
void * lv_mem_buf_get(uint32_t size)
{
    if(size == 0) return NULL;

    MEM_TRACE("begin, getting %d bytes", (int)size);

    /*Try to find a free buffer with suitable size*/
    int8_t i_guess = -1;
    for(uint8_t i = 0; i < LV_MEM_BUF_MAX_NUM; i++) {
        if(LV_GC_ROOT(lv_mem_buf[i]).used == 0 && LV_GC_ROOT(lv_mem_buf[i]).size >= size) {
            if(LV_GC_ROOT(lv_mem_buf[i]).size == size) {
                LV_GC_ROOT(lv_mem_buf[i]).used = 1;
                return LV_GC_ROOT(lv_mem_buf[i]).p;
            }
            else if(i_guess < 0) {
                i_guess = i;
            }
            /*If size of `i` is closer to `size` prefer it*/
            else if(LV_GC_ROOT(lv_mem_buf[i]).size < LV_GC_ROOT(lv_mem_buf[i_guess]).size) {
                i_guess = i;
            }
        }
    }

    if(i_guess >= 0) {
        LV_GC_ROOT(lv_mem_buf[i_guess]).used = 1;
        MEM_TRACE("returning already allocated buffer (buffer id: %d, address: %p)", i_guess,
                  LV_GC_ROOT(lv_mem_buf[i_guess]).p);
        return LV_GC_ROOT(lv_mem_buf[i_guess]).p;
    }

    /*Reallocate a free buffer*/
    for(uint8_t i = 0; i < LV_MEM_BUF_MAX_NUM; i++) {
        if(LV_GC_ROOT(lv_mem_buf[i]).used == 0) {
            /*if this fails you probably need to increase your LV_MEM_SIZE/heap size*/
            void * buf = lv_realloc(LV_GC_ROOT(lv_mem_buf[i]).p, size);
            LV_ASSERT_MSG(buf != NULL,
                          "Out of memory, can't allocate a new buffer (increase your LV_MEM_SIZE/heap size)");
            if(buf == NULL) return NULL;

            LV_GC_ROOT(lv_mem_buf[i]).used = 1;
            LV_GC_ROOT(lv_mem_buf[i]).size = (uint16_t)size;
            LV_GC_ROOT(lv_mem_buf[i]).p    = buf;
            MEM_TRACE("allocated (buffer id: %d, address: %p)", i, LV_GC_ROOT(lv_mem_buf[i]).p);
            return LV_GC_ROOT(lv_mem_buf[i]).p;
        }
    }

    LV_LOG_ERROR("no more buffers. (increase LV_MEM_BUF_MAX_NUM)");
    LV_ASSERT_MSG(false, "No more buffers. Increase LV_MEM_BUF_MAX_NUM.");
    return NULL;
}

/**
 * Release a memory buffer
 * @param p buffer to release
 */
void lv_mem_buf_release(void * p)
{
    MEM_TRACE("begin (address: %p)", p);

    for(uint8_t i = 0; i < LV_MEM_BUF_MAX_NUM; i++) {
        if(LV_GC_ROOT(lv_mem_buf[i]).p == p) {
            LV_GC_ROOT(lv_mem_buf[i]).used = 0;
            return;
        }
    }

    LV_LOG_ERROR("p is not a known buffer");
}

/**
 * Free all memory buffers
 */
void lv_mem_buf_free_all(void)
{
    for(uint8_t i = 0; i < LV_MEM_BUF_MAX_NUM; i++) {
        if(LV_GC_ROOT(lv_mem_buf[i]).p) {
            lv_free(LV_GC_ROOT(lv_mem_buf[i]).p);
            LV_GC_ROOT(lv_mem_buf[i]).p = NULL;
            LV_GC_ROOT(lv_mem_buf[i]).used = 0;
            LV_GC_ROOT(lv_mem_buf[i]).size = 0;
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
