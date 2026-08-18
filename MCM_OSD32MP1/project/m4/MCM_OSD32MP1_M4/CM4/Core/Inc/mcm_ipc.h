#if 0
#ifndef MCM_IPC_H
#define MCM_IPC_H

#include <stdint.h>

#define MCM_RPMSG_SERVICE_NAME   "mcm-rpmsg"

#define MCM_MSG_MAGIC            0x4D434D52UL   /* "MCMR" */

#define MCM_MSG_BLOCK_READY      1U
#define MCM_MSG_START            2U
#define MCM_MSG_STOP             3U
#define MCM_MSG_STATUS           4U

#define MCM_COMM_BLOCK_READY     (1UL << 0)

typedef struct
{
    uint32_t magic;
    uint32_t type;

    uint32_t sequence;
    uint32_t block_index;

    uint32_t sample_count;
    uint32_t channel_count;

    uint32_t flags;

} mcm_rpmsg_msg_t;

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

#endif
#endif

#ifndef MCM_IPC_H
#define MCM_IPC_H

#include <stdint.h>

/*
 * Linux tarafında da aynı service name kullanılacak.
 */
#define MCM_RPMSG_SERVICE_NAME    "mcm-rpmsg"

/*
 * Mesajın gerçekten bizim protokolümüze ait olduğunu
 * anlamak için kullanılacak magic value.
 *
 * ASCII karşılığı yaklaşık "MCMR".
 */
#define MCM_MSG_MAGIC             0x4D434D52UL

/*
 * M4 -> A7 mesajları
 */
#define MCM_MSG_BLOCK_READY       1U
#define MCM_MSG_STATUS            2U
#define MCM_MSG_ERROR             3U

/*
 * A7 -> M4 komutları
 */
#define MCM_MSG_START             10U
#define MCM_MSG_STOP              11U
#define MCM_MSG_GET_STATUS        12U

/*
 * communicationTask'i uyandırmak için kullanacağız.
 */
#define MCM_COMM_BLOCK_READY_FLAG (1UL << 0)


typedef struct
{
    uint32_t magic;
    uint32_t type;

    uint32_t sequence;
    uint32_t block_index;

    uint32_t sample_count;
    uint32_t channel_count;

    uint32_t flags;

} mcm_rpmsg_msg_t;

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



/*
 * OpenAMP/RPMsg altyapısını başlatır ve
 * mcm-rpmsg endpoint'ini oluşturur.
 *
 * 0  : başarılı
 * <0 : hata
 */
int MCM_IPC_Init(void);


/*
 * OpenAMP mailbox/RPMsg işlemlerini yürütür.
 * communicationTask içinde periyodik çağrılacak.
 */
void MCM_IPC_Process(void);


/*
 * A7/Linux tarafına "shared DDR block hazır" bildirimi gönderir.
 */
int MCM_IPC_SendBlockReady(uint32_t block_index,
                           uint32_t sequence);


/*
 * İleride START/STOP komut durumunu buradan okuyacağız.
 * Şimdilik altyapıyı hazırlıyoruz.
 */
uint32_t MCM_IPC_GetPendingCommand(void);

void MCM_IPC_ClearPendingCommand(void);

#endif /* MCM_IPC_H */
