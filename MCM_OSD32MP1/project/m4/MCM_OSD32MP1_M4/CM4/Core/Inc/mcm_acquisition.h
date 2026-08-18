#ifndef MCM_ACQUISITION_H
#define MCM_ACQUISITION_H

#include "main.h"
#include "cmsis_os2.h"

#define MCM_DMA_HALF_SAMPLES      1024U
#define MCM_DMA_BUFFER_SAMPLES    (2U * MCM_DMA_HALF_SAMPLES)

/* Half-transfer flags */
#define MCM_HALF_FLT0   (1UL << 0)
#define MCM_HALF_FLT1   (1UL << 1)
#define MCM_HALF_FLT2   (1UL << 2)
#define MCM_HALF_FLT3   (1UL << 3)
#define MCM_HALF_FLT4   (1UL << 4)
#define MCM_HALF_FLT5   (1UL << 5)

#define MCM_HALF_ALL    (0x0000003FUL)

/* Full-transfer flags */
#define MCM_FULL_FLT0   (1UL << 8)
#define MCM_FULL_FLT1   (1UL << 9)
#define MCM_FULL_FLT2   (1UL << 10)
#define MCM_FULL_FLT3   (1UL << 11)
#define MCM_FULL_FLT4   (1UL << 12)
#define MCM_FULL_FLT5   (1UL << 13)

#define MCM_FULL_ALL    (0x00003F00UL)

HAL_StatusTypeDef MCM_Acquisition_Start(void);
HAL_StatusTypeDef MCM_Acquisition_Stop(void);

extern int32_t mcm_dma_va[MCM_DMA_BUFFER_SAMPLES];
extern int32_t mcm_dma_vb[MCM_DMA_BUFFER_SAMPLES];
extern int32_t mcm_dma_vc[MCM_DMA_BUFFER_SAMPLES];
extern int32_t mcm_dma_ia[MCM_DMA_BUFFER_SAMPLES];
extern int32_t mcm_dma_ib[MCM_DMA_BUFFER_SAMPLES];
extern int32_t mcm_dma_ic[MCM_DMA_BUFFER_SAMPLES];

#endif
