#include "main.h"
#include "mcm_shared.h"

void MCM_Shared_Init(void)
{
    volatile mcm_shared_memory_t *shm = MCM_SHARED_MEMORY;

    shm->header.magic = MCM_SHM_MAGIC;
    shm->header.version = MCM_SHM_VERSION;

    shm->header.write_index = 0U;
    shm->header.read_index = 0U;

    shm->header.sequence = 0U;
    shm->header.overrun_count = 0U;
    shm->header.status = 0U;
}

int MCM_Shared_WriteBlock(
    uint32_t offset,
    const int32_t *va,
    const int32_t *vb,
    const int32_t *vc,
    const int32_t *ia,
    const int32_t *ib,
    const int32_t *ic,
    uint32_t *written_block,
    uint32_t *written_sequence)

{
    volatile mcm_shared_memory_t *shm = MCM_SHARED_MEMORY;

    uint32_t write_index;
    uint32_t next_index;
    uint32_t i;
    uint32_t seq;

    write_index = shm->header.write_index;

    next_index =
        (write_index + 1U) % MCM_RING_BLOCK_COUNT;

    /*
     * Ring buffer full?
     */
    if (next_index == shm->header.read_index)
    {
        shm->header.overrun_count++;
        return -1;
    }

    volatile mcm_shared_block_t *block =
        &shm->block[write_index];

    seq = shm->header.sequence++;

    block->sequence = seq;
    block->sample_count = MCM_BLOCK_SAMPLES;
    block->channel_count = MCM_CHANNEL_COUNT;
    block->flags = 0U;

    for (i = 0U; i < MCM_BLOCK_SAMPLES; i++)
    {
        block->data[0][i] = va[offset + i];
        block->data[1][i] = vb[offset + i];
        block->data[2][i] = vc[offset + i];

        block->data[3][i] = ia[offset + i];
        block->data[4][i] = ib[offset + i];
        block->data[5][i] = ic[offset + i];
    }

    /*
     * Publish block only after all data is written.
     */
    __DMB();

    shm->header.write_index = next_index;

    if (written_block != NULL)
    {
        *written_block = write_index;
    }

    if (written_sequence != NULL)
    {
        *written_sequence = seq;
    }

    return 0;
}
