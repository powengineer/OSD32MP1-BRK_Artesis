/*
 * mcm_shared.h
 *
 *  Created on: Aug 17, 2026
 *      Author: fatih
 */

#ifndef INC_MCM_SHARED_H_
#define INC_MCM_SHARED_H_

#include <stdint.h>

#define MCM_SHM_BASE_ADDRESS      (0xD3000000UL)

#define MCM_SHM_MAGIC             (0x4D434D31UL) /* "MCM1" */
#define MCM_SHM_VERSION           (1U)

#define MCM_CHANNEL_COUNT         (6U)
#define MCM_BLOCK_SAMPLES         (1024U)
#define MCM_RING_BLOCK_COUNT      (256U)

typedef struct
{
    uint32_t magic;
    uint32_t version;

    volatile uint32_t write_index;
    volatile uint32_t read_index;

    volatile uint32_t sequence;
    volatile uint32_t overrun_count;

    volatile uint32_t status;
    uint32_t reserved[9];

} mcm_shared_header_t;

typedef struct
{
    uint32_t sequence;
    uint32_t sample_count;
    uint32_t channel_count;
    uint32_t flags;

    int32_t data[MCM_CHANNEL_COUNT][MCM_BLOCK_SAMPLES];

} mcm_shared_block_t;

typedef struct
{
    mcm_shared_header_t header;

    mcm_shared_block_t block[MCM_RING_BLOCK_COUNT];

} mcm_shared_memory_t;

#define MCM_SHARED_MEMORY \
    ((volatile mcm_shared_memory_t *)MCM_SHM_BASE_ADDRESS)

void MCM_Shared_Init(void);

int MCM_Shared_WriteBlock(
    uint32_t offset,
    const int32_t *va,
    const int32_t *vb,
    const int32_t *vc,
    const int32_t *ia,
    const int32_t *ib,
    const int32_t *ic,
    uint32_t *written_block,
    uint32_t *written_sequence);



#endif /* INC_MCM_SHARED_H_ */
