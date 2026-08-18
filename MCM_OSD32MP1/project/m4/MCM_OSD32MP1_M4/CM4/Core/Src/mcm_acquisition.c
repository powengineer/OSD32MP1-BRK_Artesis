#include "mcm_acquisition.h"
#include "dfsdm.h"

extern osThreadId_t acquisitionTaskHandle;

/*
 * Filter/channel assignment
 *
 * FLT0 / CH1 -> VA
 * FLT1 / CH3 -> VB
 * FLT2 / CH4 -> VC
 * FLT3 / CH5 -> IA
 * FLT4 / CH6 -> IB
 * FLT5 / CH7 -> IC
 */

int32_t mcm_dma_va[MCM_DMA_BUFFER_SAMPLES];
int32_t mcm_dma_vb[MCM_DMA_BUFFER_SAMPLES];
int32_t mcm_dma_vc[MCM_DMA_BUFFER_SAMPLES];

int32_t mcm_dma_ia[MCM_DMA_BUFFER_SAMPLES];
int32_t mcm_dma_ib[MCM_DMA_BUFFER_SAMPLES];
int32_t mcm_dma_ic[MCM_DMA_BUFFER_SAMPLES];

HAL_StatusTypeDef MCM_Acquisition_Start(void)
{
    /*
     * Filters 1..5 are synchronized slaves.
     * Arm them first.
     */

    if (HAL_DFSDM_FilterRegularStart_DMA(
            &hdfsdm1_filter1,
            mcm_dma_vb,
            MCM_DMA_BUFFER_SAMPLES) != HAL_OK)
        return HAL_ERROR;

    if (HAL_DFSDM_FilterRegularStart_DMA(
            &hdfsdm1_filter2,
            mcm_dma_vc,
            MCM_DMA_BUFFER_SAMPLES) != HAL_OK)
        return HAL_ERROR;

    if (HAL_DFSDM_FilterRegularStart_DMA(
            &hdfsdm1_filter3,
            mcm_dma_ia,
            MCM_DMA_BUFFER_SAMPLES) != HAL_OK)
        return HAL_ERROR;

    if (HAL_DFSDM_FilterRegularStart_DMA(
            &hdfsdm1_filter4,
            mcm_dma_ib,
            MCM_DMA_BUFFER_SAMPLES) != HAL_OK)
        return HAL_ERROR;

    if (HAL_DFSDM_FilterRegularStart_DMA(
            &hdfsdm1_filter5,
            mcm_dma_ic,
            MCM_DMA_BUFFER_SAMPLES) != HAL_OK)
        return HAL_ERROR;

    /*
     * Filter0 is the master.
     * Start it last so its SW trigger starts the synchronized filters.
     */
    if (HAL_DFSDM_FilterRegularStart_DMA(
            &hdfsdm1_filter0,
            mcm_dma_va,
            MCM_DMA_BUFFER_SAMPLES) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

HAL_StatusTypeDef MCM_Acquisition_Stop(void)
{
    HAL_StatusTypeDef status = HAL_OK;

    if (HAL_DFSDM_FilterRegularStop_DMA(&hdfsdm1_filter0) != HAL_OK)
        status = HAL_ERROR;

    if (HAL_DFSDM_FilterRegularStop_DMA(&hdfsdm1_filter1) != HAL_OK)
        status = HAL_ERROR;

    if (HAL_DFSDM_FilterRegularStop_DMA(&hdfsdm1_filter2) != HAL_OK)
        status = HAL_ERROR;

    if (HAL_DFSDM_FilterRegularStop_DMA(&hdfsdm1_filter3) != HAL_OK)
        status = HAL_ERROR;

    if (HAL_DFSDM_FilterRegularStop_DMA(&hdfsdm1_filter4) != HAL_OK)
        status = HAL_ERROR;

    if (HAL_DFSDM_FilterRegularStop_DMA(&hdfsdm1_filter5) != HAL_OK)
        status = HAL_ERROR;

    return status;
}

void HAL_DFSDM_FilterRegConvHalfCpltCallback(
    DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
    uint32_t flag = 0U;

    if (hdfsdm_filter == &hdfsdm1_filter0)
        flag = MCM_HALF_FLT0;
    else if (hdfsdm_filter == &hdfsdm1_filter1)
        flag = MCM_HALF_FLT1;
    else if (hdfsdm_filter == &hdfsdm1_filter2)
        flag = MCM_HALF_FLT2;
    else if (hdfsdm_filter == &hdfsdm1_filter3)
        flag = MCM_HALF_FLT3;
    else if (hdfsdm_filter == &hdfsdm1_filter4)
        flag = MCM_HALF_FLT4;
    else if (hdfsdm_filter == &hdfsdm1_filter5)
        flag = MCM_HALF_FLT5;

    if ((flag != 0U) && (acquisitionTaskHandle != NULL))
    {
        (void)osThreadFlagsSet(acquisitionTaskHandle, flag);
    }
}

void HAL_DFSDM_FilterRegConvCpltCallback(
    DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
    uint32_t flag = 0U;

    if (hdfsdm_filter == &hdfsdm1_filter0)
        flag = MCM_FULL_FLT0;
    else if (hdfsdm_filter == &hdfsdm1_filter1)
        flag = MCM_FULL_FLT1;
    else if (hdfsdm_filter == &hdfsdm1_filter2)
        flag = MCM_FULL_FLT2;
    else if (hdfsdm_filter == &hdfsdm1_filter3)
        flag = MCM_FULL_FLT3;
    else if (hdfsdm_filter == &hdfsdm1_filter4)
        flag = MCM_FULL_FLT4;
    else if (hdfsdm_filter == &hdfsdm1_filter5)
        flag = MCM_FULL_FLT5;

    if ((flag != 0U) && (acquisitionTaskHandle != NULL))
    {
        (void)osThreadFlagsSet(acquisitionTaskHandle, flag);
    }
}

