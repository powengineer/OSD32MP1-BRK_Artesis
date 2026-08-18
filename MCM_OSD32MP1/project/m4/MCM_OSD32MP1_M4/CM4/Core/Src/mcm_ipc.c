#if 0
#include "mcm_ipc.h"
#include "openamp.h"

static struct rpmsg_endpoint mcm_ept;

static int MCM_RPMsg_Callback(
    struct rpmsg_endpoint *ept,
    void *data,
    size_t len,
    uint32_t src,
    void *priv)
{
    (void)ept;
    (void)src;
    (void)priv;

    if ((data == NULL) || (len < sizeof(mcm_rpmsg_msg_t)))
    {
        return 0;
    }

    mcm_rpmsg_msg_t *msg = (mcm_rpmsg_msg_t *)data;

    if (msg->magic != MCM_MSG_MAGIC)
    {
        return 0;
    }

    switch (msg->type)
    {
        case MCM_MSG_START:
            /* Daha sonra START işlemi */
            break;

        case MCM_MSG_STOP:
            /* Daha sonra STOP işlemi */
            break;

        case MCM_MSG_STATUS:
            /* Daha sonra status işlemi */
            break;

        default:
            break;
    }

    return 0;
}

static void MCM_RPMsg_Unbind(struct rpmsg_endpoint *ept)
{
    (void)ept;
}
#endif

#include "main.h"
#include "mcm_ipc.h"
#include "mcm_shared.h"
#include "openamp.h"

#include <stddef.h>


/*
 * Bu endpoint yalnız MCM uygulamasına ait.
 */
static struct rpmsg_endpoint mcm_ept;


/*
 * Endpoint başarıyla hazır mı?
 */
static volatile uint32_t mcm_ipc_ready = 0U;


/*
 * Linux'tan gelen START / STOP / GET_STATUS
 * komutlarını burada saklayacağız.
 *
 * Callback içinde uzun işlem yapmıyoruz.
 */
static volatile uint32_t mcm_pending_command = 0U;


/*
 * Linux -> M4 RPMsg receive callback
 */
static int MCM_RPMsg_Callback(struct rpmsg_endpoint *ept,
                              void *data,
                              size_t len,
                              uint32_t src,
                              void *priv)
{
    const mcm_rpmsg_msg_t *msg;

    (void)ept;
    (void)src;
    (void)priv;

    /*
     * Null pointer veya beklenenden küçük mesaj ise yok say.
     */
    if ((data == NULL) || (len < sizeof(mcm_rpmsg_msg_t)))
    {
        return 0;
    }

    msg = (const mcm_rpmsg_msg_t *)data;

    /*
     * Bizim protokolümüz değilse yok say.
     */
    if (msg->magic != MCM_MSG_MAGIC)
    {
        return 0;
    }

    /*
     * Callback içinde acquisition başlatıp durdurmuyoruz.
     * Yalnızca komutu kaydediyoruz.
     *
     * Daha sonra communicationTask uygun şekilde işleyecek.
     */
    switch (msg->type)
    {
        case MCM_MSG_START:
            mcm_pending_command = MCM_MSG_START;
            break;

        case MCM_MSG_STOP:
            mcm_pending_command = MCM_MSG_STOP;
            break;

        case MCM_MSG_GET_STATUS:
            mcm_pending_command = MCM_MSG_GET_STATUS;
            break;

        default:
            break;
    }

    return 0;
}


/*
 * Linux tarafı RPMsg endpoint/channel bağlantısını kaldırdığında
 * OpenAMP tarafından çağrılabilir.
 */
static void MCM_RPMsg_Unbind(struct rpmsg_endpoint *ept)
{
    (void)ept;

    mcm_ipc_ready = 0U;
}


int MCM_IPC_Init(void)
{
    int status;

    /*
     * STM32MP157 Cortex-M4, OpenAMP tarafında REMOTE processor.
     *
     * Linux/A7 tarafı remoteproc master/host tarafıdır.
     */
    status = MX_OPENAMP_Init(RPMSG_REMOTE, NULL);

    if (status != 0)
    {
        return status;
    }

    /*
     * Kendi RPMsg endpoint'imizi oluşturuyoruz.
     *
     * dest = RPMSG_ADDR_ANY:
     * Linux tarafındaki karşı endpoint henüz bilinmediği için
     * OpenAMP/Name Service tarafından eşleştirilecek.
     */
    status = OPENAMP_create_endpoint(
                 &mcm_ept,
                 MCM_RPMSG_SERVICE_NAME,
                 RPMSG_ADDR_ANY,
                 MCM_RPMsg_Callback,
                 MCM_RPMsg_Unbind);

    if (status != 0)
    {
        return status;
    }

    /*
     * Linux tarafındaki endpoint'in hazır olmasını bekle.
     */
    OPENAMP_Wait_EndPointready(&mcm_ept);

    mcm_ipc_ready = 1U;

    return 0;
}


void MCM_IPC_Process(void)
{
    /*
     * IPCC/mailbox üzerinden gelen OpenAMP/RPMsg
     * olaylarının işlenmesini sağlar.
     */
    OPENAMP_check_for_message();
}


int MCM_IPC_SendBlockReady(uint32_t block_index,
                           uint32_t sequence)
{
    mcm_rpmsg_msg_t msg;

    if (mcm_ipc_ready == 0U)
    {
        return -1;
    }

    msg.magic = MCM_MSG_MAGIC;
    msg.type = MCM_MSG_BLOCK_READY;

    msg.sequence = sequence;
    msg.block_index = block_index;

    msg.sample_count = MCM_BLOCK_SAMPLES;
    msg.channel_count = MCM_CHANNEL_COUNT;

    msg.flags = 0U;

    /*
     * OPENAMP_send sizin generated openamp.h içinde
     *
     * #define OPENAMP_send rpmsg_send
     *
     * olarak zaten tanımlı.
     */
    return OPENAMP_send(&mcm_ept,
                        &msg,
                        sizeof(msg));
}


uint32_t MCM_IPC_GetPendingCommand(void)
{
    return mcm_pending_command;
}


void MCM_IPC_ClearPendingCommand(void)
{
    mcm_pending_command = 0U;
}
