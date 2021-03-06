#if !defined(__MN34220_CMOS_H_)
#define __MN34220_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hi_comm_sns.h"
#include "hi_comm_video.h"
#include "hi_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_af.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


#define MN34220_ID 34220

/* To change the mode of slow shutter. if defined, add the VMAX to slow shutter;
 * else, add the HMAX to slow shutter. */
#define CMOS_MN34220_SLOW_SHUTTER_VMAX_MODE

/* To change the mode of config. ifndef INIFILE_CONFIG_MODE, quick config mode.*/
/* else, cmos_cfg.ini file config mode*/
#ifdef INIFILE_CONFIG_MODE

extern AE_SENSOR_DEFAULT_S  g_AeDft[];
extern AWB_SENSOR_DEFAULT_S g_AwbDft[];
extern ISP_CMOS_DEFAULT_S   g_IspDft[];
extern HI_S32 Cmos_LoadINIPara(const HI_CHAR *pcName);
#else

#endif

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

extern const unsigned int sensor_i2c_addr;
extern unsigned int sensor_addr_byte;
extern unsigned int sensor_data_byte;

#define EXPOSURE_ADDR_HHH     (0x0220)
#define EXPOSURE_ADDR_H       (0x0202)
#define EXPOSURE_ADDR_L       (0x0203)
#define SHORT_EXPOSURE_ADDR_H (0x312A)
#define SHORT_EXPOSURE_ADDR_L (0x312B)
#define AGC_ADDR_H            (0x0204)
#define AGC_ADDR_L            (0x0205)
#define DGC_ADDR_H            (0x3108)
#define DGC_ADDR_L            (0x3109)
#define VMAX_ADDR_H     (0x0340)
#define VMAX_ADDR_L     (0x0341)

#define INCREASE_LINES (1) /* make real fps less than stand fps because NVR require*/
#define VMAX_1080P30_LINEAR  (1125+INCREASE_LINES)
#define VMAX_1080P60_LINEAR  (1125+INCREASE_LINES)
#define VMAX_1080P60TO30_WDR (1250+INCREASE_LINES)
#define VMAX_720P120_LINEAR  (750+INCREASE_LINES)
#define VMAX_VGA160_LINEAR   (562+INCREASE_LINES)

#define SENSOR_1080P_60FPS_MODE (1)
#define SENSOR_1080P_30FPS_MODE (2)
#define SENSOR_720P_120FPS_MODE (3)
#define SENSOR_VGA_160FPS_MODE  (4)

HI_U8 gu8SensorImageMode = SENSOR_1080P_60FPS_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = VMAX_1080P60_LINEAR;
static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE; 

ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};

#define PATHLEN_MAX 256
#define CMOS_CFG_INI "mn34220_cfg.ini"
static char pcName[PATHLEN_MAX] = "configs/mn34220_cfg.ini";

/* AE default parameter and function */
/* 2to1 WDR 60->30fps*/

static ISP_AE_ROUTE_S gstAERouteAttr_30Fps = 
{
    5,

    {
        {2,   1024,   1},
        {140, 1024,   1},
        {140, 8134,   1},
        {622, 8134,   1},
        {622, 131071, 1}
    }
};

/* 2to1 WDR 50->25fps*/
static ISP_AE_ROUTE_S gstAERouteAttr_25Fps = 
{
    5,

    {
        {2,   1024,   1},
        {390, 1024,   1},
        {390, 8134,   1},
        {747, 8134,   1},
        {747, 131071, 1}
    }
};

#ifdef INIFILE_CONFIG_MODE

static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }
   
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*25/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0.8045;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.38;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.38;

    pstAeSnsDft->u32InitExposure = 921600;

    if((pstAeSnsDft->f32Fps == 50) || (pstAeSnsDft->f32Fps == 60))
    {
        pstAeSnsDft->stIntTimeAccu.f32Offset = 0.6082;
    }
    else if((pstAeSnsDft->f32Fps == 25) || (pstAeSnsDft->f32Fps == 30))
    {
        pstAeSnsDft->stIntTimeAccu.f32Offset = 0.8045;
    }
    else
    {}
    
    switch(genSensorMode)
    {
        default:
        case WDR_MODE_NONE:   /*linear mode*/
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = g_AeDft[0].u8AeCompensation;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[0].u32MaxIntTimeTarget;
            pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[0].u32MinIntTimeTarget;
            
            pstAeSnsDft->u32MaxAgain = 32382; 
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = g_AeDft[0].u32MaxAgainTarget;
            pstAeSnsDft->u32MinAgainTarget = g_AeDft[0].u32MinAgainTarget;
            
            pstAeSnsDft->u32MaxDgain = 32382;  
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = g_AeDft[0].u32MaxDgainTarget;
            pstAeSnsDft->u32MinDgainTarget = g_AeDft[0].u32MinDgainTarget;

            pstAeSnsDft->u32ISPDgainShift = g_AeDft[0].u32ISPDgainShift;
            pstAeSnsDft->u32MinISPDgainTarget = g_AeDft[0].u32MinISPDgainTarget;
            pstAeSnsDft->u32MaxISPDgainTarget = g_AeDft[0].u32MaxISPDgainTarget;
            
        break;
        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE: /*linear mode for ISP frame switching WDR*/
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
           
            pstAeSnsDft->u8AeCompensation = g_AeDft[2].u8AeCompensation;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[2].u32MaxIntTimeTarget;
            pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[2].u32MinIntTimeTarget;
            
            pstAeSnsDft->u32MaxAgain = 32382;
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = g_AeDft[2].u32MaxAgainTarget;
            pstAeSnsDft->u32MinAgainTarget = g_AeDft[2].u32MinAgainTarget;
            
            pstAeSnsDft->u32MaxDgain = 32382;  
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = g_AeDft[2].u32MaxDgainTarget;
            pstAeSnsDft->u32MinDgainTarget = g_AeDft[2].u32MinDgainTarget;

            pstAeSnsDft->u32ISPDgainShift = g_AeDft[2].u32ISPDgainShift;
            pstAeSnsDft->u32MinISPDgainTarget = g_AeDft[2].u32MinISPDgainTarget;
            pstAeSnsDft->u32MaxISPDgainTarget = g_AeDft[2].u32MaxISPDgainTarget;            
        break;
        case WDR_MODE_2To1_LINE:
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
           
            pstAeSnsDft->u8AeCompensation = g_AeDft[1].u8AeCompensation;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;  
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[1].u32MaxIntTimeTarget;
            pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[1].u32MinIntTimeTarget;
            
            pstAeSnsDft->u32MaxAgain = 32382;
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = g_AeDft[1].u32MaxAgainTarget;
            pstAeSnsDft->u32MinAgainTarget = g_AeDft[1].u32MinAgainTarget;
            
            pstAeSnsDft->u32MaxDgain = 32382;
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = g_AeDft[1].u32MaxDgainTarget;
            pstAeSnsDft->u32MinDgainTarget = g_AeDft[1].u32MinDgainTarget;

            pstAeSnsDft->u32ISPDgainShift = g_AeDft[1].u32ISPDgainShift;
            pstAeSnsDft->u32MinISPDgainTarget = g_AeDft[1].u32MinISPDgainTarget;
            pstAeSnsDft->u32MaxISPDgainTarget = g_AeDft[1].u32MaxISPDgainTarget;            

            if (1250 == gu32FullLinesStd)
            {
                memcpy(&pstAeSnsDft->stAERouteAttr, &gstAERouteAttr_30Fps, sizeof(ISP_AE_ROUTE_S));
            }
            else if (1500 == gu32FullLinesStd)
            {
                memcpy(&pstAeSnsDft->stAERouteAttr, &gstAERouteAttr_25Fps, sizeof(ISP_AE_ROUTE_S));
            }
            else
            {}
            
            if((pstAeSnsDft->f32Fps == 50) || (pstAeSnsDft->f32Fps == 60))
            {
                pstAeSnsDft->stIntTimeAccu.f32Offset = 0.2155;
            }
            else if((pstAeSnsDft->f32Fps == 25) || (pstAeSnsDft->f32Fps == 30))
            {
                pstAeSnsDft->stIntTimeAccu.f32Offset = 0.2818;
            }
            else
            {}            
        break;
    }

    return 0;
}

#else

static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }

    memset(&pstAeSnsDft->stAERouteAttr, 0, sizeof(ISP_AE_ROUTE_S));
      
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*25/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0.8045;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.38;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.38;
    
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 8 << pstAeSnsDft->u32ISPDgainShift;
    
    pstAeSnsDft->u32InitExposure = 921600;

    if((pstAeSnsDft->f32Fps == 50) || (pstAeSnsDft->f32Fps == 60))
    {
        pstAeSnsDft->stIntTimeAccu.f32Offset = 0.6082;
    }
    else if((pstAeSnsDft->f32Fps == 25) || (pstAeSnsDft->f32Fps == 30))
    {
        pstAeSnsDft->stIntTimeAccu.f32Offset = 0.8045;
    }
    else
    {}
    
    switch(genSensorMode)
    {
        default:
        case WDR_MODE_NONE:   /*linear mode*/
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = 0x38;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;
            
            pstAeSnsDft->u32MaxAgain = 32382; 
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
            
            pstAeSnsDft->u32MaxDgain = 32382;  
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
        break;
        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE: /*linear mode for ISP frame switching WDR*/
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
           
            pstAeSnsDft->u8AeCompensation = 0x38;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;
            
            pstAeSnsDft->u32MaxAgain = 32382;  
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
            
            pstAeSnsDft->u32MaxDgain = 32382;  
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
        break;
        case WDR_MODE_2To1_LINE:
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
           
            pstAeSnsDft->u8AeCompensation = 0x40;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;  
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;
            
            pstAeSnsDft->u32MaxAgain = 32382;
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = 8134;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
            
            pstAeSnsDft->u32MaxDgain = 32382;
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = 1024;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;

            if (1250 == gu32FullLinesStd)
            {
                memcpy(&pstAeSnsDft->stAERouteAttr, &gstAERouteAttr_30Fps, sizeof(ISP_AE_ROUTE_S));
            }
            else if (1500 == gu32FullLinesStd)
            {
                memcpy(&pstAeSnsDft->stAERouteAttr, &gstAERouteAttr_25Fps, sizeof(ISP_AE_ROUTE_S));
            }
            else
            {}

            if((pstAeSnsDft->f32Fps == 50) || (pstAeSnsDft->f32Fps == 60))
            {
                pstAeSnsDft->stIntTimeAccu.f32Offset = 0.2155;
            }
            else if((pstAeSnsDft->f32Fps == 25) || (pstAeSnsDft->f32Fps == 30))
            {
                pstAeSnsDft->stIntTimeAccu.f32Offset = 0.2818;
            }
            else
            {}
        break;
    }

    return 0;
}

#endif


/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
#ifdef CMOS_MN34220_SLOW_SHUTTER_VMAX_MODE
    if (WDR_MODE_2To1_LINE == genSensorMode)
    {
        /* In WDR_MODE_2To1_LINE mode, 30fps means 60 frames(30 short and 30 long) combine to 30 frames per sencond */
        if ((f32Fps <= 30) && (f32Fps >= 0.5))
        {
            gu32FullLinesStd = VMAX_1080P60TO30_WDR * 30 / f32Fps;
            pstAeSnsDft->stIntTimeAccu.f32Offset = 0.2818;
        }
        else
        {
            printf("Not support Fps: %f\n", f32Fps);
            return;
        }

        if (25 == f32Fps)
        {
            memcpy(&pstAeSnsDft->stAERouteAttr, &gstAERouteAttr_25Fps, sizeof(ISP_AE_ROUTE_S));
        }
        else
        {
            memcpy(&pstAeSnsDft->stAERouteAttr, &gstAERouteAttr_30Fps, sizeof(ISP_AE_ROUTE_S));
        }
    }
    else
    {
        if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
        {
            if ((f32Fps <= 30) && (f32Fps >= 0.5))
            {
                /* In 1080P30fps mode, the VMAX(FullLines) is VMAX_1080P30_LINEAR, 
                     and there are (VMAX_1080P30_LINEAR*30) lines in 1 second,
                     so in f32Fps mode, VMAX(FullLines) is (VMAX_1080P30_LINEAR*30)/f32Fps */
                gu32FullLinesStd = (VMAX_1080P30_LINEAR * 30) / f32Fps;
                pstAeSnsDft->stIntTimeAccu.f32Offset = 0.8045;
            }
            else
            {
                printf("Not support Fps: %f\n", f32Fps);
                return;
            }
        }
        else if (SENSOR_1080P_60FPS_MODE == gu8SensorImageMode)
        {
            if ((f32Fps <= 60) && (f32Fps >= 0.5))
            {
                /* In 1080P60fps mode, the VMAX(FullLines) is VMAX_1080P60_LINEAR, 
                     and there are (VMAX_1080P60_LINEAR*60) lines in 1 second,
                     so in f32Fps mode, VMAX(FullLines) is (VMA X_1080P60_LINEAR*60)/f32Fps */
                gu32FullLinesStd = (VMAX_1080P60_LINEAR * 60) / f32Fps;
                pstAeSnsDft->stIntTimeAccu.f32Offset = 0.6082;
            }
            else
            {
                printf("Not support Fps: %f\n", f32Fps);
                return;
            }
        }
        else if (SENSOR_720P_120FPS_MODE == gu8SensorImageMode)
        {
            if ((f32Fps <= 120) && (f32Fps >= 0.5))
            {
                /* In 720P120fps mode, the VMAX(FullLines) is VMAX_720P120_LINEAR, 
                     and there are (VMAX_720P120_LINEAR*120) lines in 1 second,
                     so in f32Fps mode, VMAX(FullLines) is (VMA X_720P120_LINEAR*120)/f32Fps */
                gu32FullLinesStd = (VMAX_720P120_LINEAR * 120) / f32Fps;
                pstAeSnsDft->stIntTimeAccu.f32Offset = 0.5960;  
            }
            else
            {
                printf("Not support Fps: %f\n", f32Fps);
                return;
            }
        }
        else if (SENSOR_VGA_160FPS_MODE == gu8SensorImageMode)
        {
            if ((f32Fps <= 240) && (f32Fps >= 0.5))
            {
                /* In VGA 160fps mode, the VMAX(FullLines) is VMAX_VGA160_LINEAR, 
                     and there are (VMAX_VGA160_LINEAR*160) lines in 1 second,
                     so in f32Fps mode, VMAX(FullLines) is (VMA VMAX_VGA160_LINEAR*160)/f32Fps */
                gu32FullLinesStd = (VMAX_VGA160_LINEAR * 160) / f32Fps;
                pstAeSnsDft->stIntTimeAccu.f32Offset = 0.5960;
            }
            else
            {
                printf("Not support Fps: %f\n", f32Fps);
                return;
            }
        }
        else
        {
            printf("Not support! gu8SensorImageMode:%d, f32Fps:%f\n", gu8SensorImageMode, f32Fps);
            return;
        }
    }

    gu32FullLinesStd = (gu32FullLinesStd > 0xFFFF) ? 0xFFFF : gu32FullLinesStd;
    if(WDR_MODE_NONE == genSensorMode)
    {
        g_stSnsRegsInfo.astI2cData[7].u32Data = ((gu32FullLinesStd & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[8].u32Data = (gu32FullLinesStd & 0xFF);        
    }
    else
    {
        g_stSnsRegsInfo.astI2cData[9].u32Data = ((gu32FullLinesStd & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[10].u32Data = (gu32FullLinesStd & 0xFF);
    }

    pstAeSnsDft->f32Fps = f32Fps;
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * f32Fps / 2;
    pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
#else
    
#endif

    return;
}

static HI_VOID cmos_slow_framerate_set(HI_U32 u32FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    u32FullLines = (u32FullLines > 0xFFFF) ? 0xFFFF : u32FullLines;

    if(WDR_MODE_NONE == genSensorMode)
    {
        g_stSnsRegsInfo.astI2cData[7].u32Data = ((u32FullLines & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[8].u32Data = (u32FullLines & 0xFF);        
    }
    else
    {
        g_stSnsRegsInfo.astI2cData[9].u32Data = ((u32FullLines & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[10].u32Data = (u32FullLines & 0xFF);
    }

    pstAeSnsDft->u32MaxIntTime = u32FullLines - 2;

    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    static HI_BOOL bFirst = HI_TRUE; 

    if ((WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode) || (WDR_MODE_2To1_FRAME == genSensorMode))
    {
        if (bFirst) /* short exposure */
        {
            g_stSnsRegsInfo.astI2cData[0].u32Data = ((u32IntTime & 0xFF00) >> 8);
            g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime & 0xFF);
            bFirst = HI_FALSE;
        }
        else    /* long exposure */
        {
            g_stSnsRegsInfo.astI2cData[7].u32Data = ((u32IntTime & 0xFF00) >> 8);
            g_stSnsRegsInfo.astI2cData[8].u32Data = (u32IntTime & 0xFF);
            bFirst = HI_TRUE;
        }        
    }
    else if (WDR_MODE_2To1_LINE == genSensorMode)
    {
        if (bFirst) /* short exposure */
        {
            g_stSnsRegsInfo.astI2cData[7].u32Data = ((u32IntTime & 0xFF00) >> 8);
            g_stSnsRegsInfo.astI2cData[8].u32Data = (u32IntTime & 0xFF);
            bFirst = HI_FALSE;
        }
        else    /* long exposure */
        {
            g_stSnsRegsInfo.astI2cData[0].u32Data = ((u32IntTime & 0xFF00) >> 8);
            g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime & 0xFF);
            bFirst = HI_TRUE;
        }
    }
    else
    {
        g_stSnsRegsInfo.astI2cData[0].u32Data = ((u32IntTime & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime & 0xFF);
        bFirst = HI_TRUE;
    }

    return;
}

static HI_U32 ad_gain_table[81]=
{    
    1024, 1070, 1116, 1166, 1217, 1271, 1327, 1386, 1446, 1511, 1577, 1647, 1719, 1796, 1874, 1958, 2043,
    2135, 2227, 2327, 2428, 2537, 2647, 2766, 2886, 3015, 3146, 3287, 3430, 3583, 3739, 3907, 4077, 4259, 
    4444, 4643, 4845, 5062, 5282, 5518, 5758, 6016, 6278, 6558, 6844, 7150, 7461, 7795, 8134, 8498, 8867,
    9264, 9667, 10100,10539, 11010, 11489, 12003, 12526, 13086, 13655, 14266, 14887, 15552, 16229, 16955,
    17693, 18484, 19289, 20151, 21028, 21968, 22925, 23950, 24992, 26110, 27246, 28464, 29703, 31031, 32382    
};

static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;

    if((HI_NULL == pu32AgainLin) ||(HI_NULL == pu32AgainDb))
    {
        printf("null pointer when get ae sensor gain info  value!\n");
        return;
    }

    if (*pu32AgainLin >= ad_gain_table[80])
    {
         *pu32AgainLin = ad_gain_table[80];
         *pu32AgainDb = 80;
         return ;
    }
    
    for (i = 1; i < 81; i++)
    {
        if (*pu32AgainLin < ad_gain_table[i])
        {
            *pu32AgainLin = ad_gain_table[i - 1];
            *pu32AgainDb = i - 1;
            break;
        }
    }
    
    return;
}

static HI_VOID cmos_dgain_calc_table(HI_U32 *pu32DgainLin, HI_U32 *pu32DgainDb)
{
    int i;

    if((HI_NULL == pu32DgainLin) ||(HI_NULL == pu32DgainDb))
    {
        printf("null pointer when get ae sensor gain info  value!\n");
        return;
    }

    if (*pu32DgainLin >= ad_gain_table[80])
    {
         *pu32DgainLin = ad_gain_table[80];
         *pu32DgainDb = 80;
         return ;
    }
    
    for (i = 1; i < 81; i++)
    {
        if (*pu32DgainLin < ad_gain_table[i])
        {
            *pu32DgainLin = ad_gain_table[i - 1];
            *pu32DgainDb = i - 1;
            break;
        }
    }

    return;
}

static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{  
    HI_U32 u32Tmp_Again = 0x100 + u32Again * 4;
    HI_U32 u32Tmp_Dgain = 0x100 + u32Dgain * 4;

    if ((g_stSnsRegsInfo.astI2cData[3].u32Data == ((u32Tmp_Again & 0xFF00) >> 8))
        && (g_stSnsRegsInfo.astI2cData[4].u32Data == (u32Tmp_Again & 0xFF)))
    {
        g_stSnsRegsInfo.astI2cData[2].u32Data = 0x30;
    }
    else
    {
        g_stSnsRegsInfo.astI2cData[2].u32Data = 0x0;
    }

    g_stSnsRegsInfo.astI2cData[3].u32Data = ((u32Tmp_Again & 0xFF00) >> 8);
    g_stSnsRegsInfo.astI2cData[4].u32Data = (u32Tmp_Again & 0xFF);

    g_stSnsRegsInfo.astI2cData[5].u32Data = ((u32Tmp_Dgain & 0xFF00) >> 8);
    g_stSnsRegsInfo.astI2cData[6].u32Data = (u32Tmp_Dgain & 0xFF);

    return;
}

/* Only used in WDR_MODE_2To1_LINE and WDR_MODE_2To1_FRAME mode */
static HI_VOID cmos_get_inttime_max(HI_U32 u32Ratio, HI_U32 *pu32IntTimeMax)
{
    if(HI_NULL == pu32IntTimeMax)
    {
        printf("null pointer when get ae sensor IntTimeMax value!\n");
        return;
    }

    if ((WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode) || (WDR_MODE_2To1_FRAME == genSensorMode))
    {
        *pu32IntTimeMax = (gu32FullLinesStd - 2) * 0x40 / DIV_0_TO_1(u32Ratio);
    }
    else if ((WDR_MODE_2To1_LINE == genSensorMode))
    {
        /* 2 + Short + 3 + Long <= 1V; 
           Ratio = Long * 0x40 / Short */
        /* To avoid Long frame and Short frame exposure time out of sync, so use (gu32FullLinesStd - 30) */
        *pu32IntTimeMax = (gu32FullLinesStd - 30) * 0x40 / (u32Ratio + 0x40);
    }

    return;
}

HI_S32 cmos_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
    pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
    pstExpFuncs->pfn_cmos_slow_framerate_set= cmos_slow_framerate_set;    
    pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
    pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
    pstExpFuncs->pfn_cmos_again_calc_table  = cmos_again_calc_table;
    pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_dgain_calc_table;
    pstExpFuncs->pfn_cmos_get_inttime_max   = cmos_get_inttime_max;    

    return 0;
}


/* AWB default parameter and function */
#ifdef INIFILE_CONFIG_MODE

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    HI_U8 i;
    
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));
    
    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:            
            pstAwbSnsDft->u16WbRefTemp = g_AwbDft[0].u16WbRefTemp;
            for(i= 0; i < 4; i++)
            {
                pstAwbSnsDft->au16GainOffset[i] = g_AwbDft[0].au16GainOffset[i];
            }
           
            for(i= 0; i < 6; i++)
            {
                pstAwbSnsDft->as32WbPara[i] = g_AwbDft[0].as32WbPara[i];
            }

            memcpy(&pstAwbSnsDft->stCcm, &g_AwbDft[0].stCcm, sizeof(AWB_CCM_S));            
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_AwbDft[0].stAgcTbl, sizeof(AWB_AGC_TABLE_S));
        break;
        case WDR_MODE_2To1_LINE:
        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE:
            pstAwbSnsDft->u16WbRefTemp = g_AwbDft[1].u16WbRefTemp;
            for(i= 0; i < 4; i++)
            {
                pstAwbSnsDft->au16GainOffset[i] = g_AwbDft[1].au16GainOffset[i];
            }
           
            for(i= 0; i < 6; i++)
            {
                pstAwbSnsDft->as32WbPara[i] = g_AwbDft[1].as32WbPara[i];
            }

            memcpy(&pstAwbSnsDft->stCcm, &g_AwbDft[1].stCcm, sizeof(AWB_CCM_S));            
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_AwbDft[1].stAgcTbl, sizeof(AWB_AGC_TABLE_S));
        break;
    }
    
    return 0;
}

#else

static AWB_CCM_S g_stAwbCcm =
{  
    4900,
    {
       0x018C,  0x808E,  0x0002,
       0x8059,  0x01BB,  0x8062,
       0x0016,  0x8139,  0x0223
    },

    3850,
    {
       0x019B,  0x80AD,  0x0012,
       0x8064,  0x019A,  0x8037,
       0x002a,  0x8161,  0x0236
    },
       
    2650,
    {     
       0x01B6,  0x8057,  0x805F, 
       0x806D,  0x01CD,  0x8060,
       0x0000,  0x8125,  0x0225
    }     
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,
    
    /*1,  2,  4,  8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768*/
    /* saturation */   
    {0x7a,0x7a,0x78,0x74,0x68,0x60,0x58,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38,0x38}

};

static AWB_AGC_TABLE_S g_stAwbAgcTableFSWDR =
{
    /* bvalid */
    1,

    /* saturation */ 
    {0x80,0x80,0x80,0x78,0x70,0x68,0x58,0x48,0x40,0x38,0x38,0x38,0x38,0x38,0x38,0x38}
};

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));

    pstAwbSnsDft->u16WbRefTemp = 5000;

    pstAwbSnsDft->au16GainOffset[0] = 0x1F0;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x1A0;

    pstAwbSnsDft->as32WbPara[0] = 41;
    pstAwbSnsDft->as32WbPara[1] = 127;
    pstAwbSnsDft->as32WbPara[2] = -87;
    pstAwbSnsDft->as32WbPara[3] = 174535;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -123490;

    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
    
    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));
        break;
        case WDR_MODE_2To1_LINE:
        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE:
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTableFSWDR, sizeof(AWB_AGC_TABLE_S));
        break;
    }
    
    return 0;
}

#endif

HI_S32 cmos_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;

    return 0;
}


/* ISP default parameter and function */
#ifdef INIFILE_CONFIG_MODE

HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    
    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:           
            memcpy(&pstDef->stDrc, &g_IspDft[0].stDrc, sizeof(ISP_CMOS_DRC_S));
            memcpy(&pstDef->stAgcTbl, &g_IspDft[0].stAgcTbl, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stNoiseTbl, &g_IspDft[0].stNoiseTbl, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stDemosaic, &g_IspDft[0].stDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_IspDft[0].stRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
            memcpy(&pstDef->stGamma, &g_IspDft[0].stGamma, sizeof(ISP_CMOS_GAMMA_S));
        break;
        case WDR_MODE_2To1_LINE:
        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE:            
            memcpy(&pstDef->stDrc, &g_IspDft[1].stDrc, sizeof(ISP_CMOS_DRC_S));
            memcpy(&pstDef->stAgcTbl, &g_IspDft[1].stAgcTbl, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stNoiseTbl, &g_IspDft[1].stNoiseTbl, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stDemosaic, &g_IspDft[1].stDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_IspDft[1].stRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));            
            memcpy(&pstDef->stGamma, &g_IspDft[1].stGamma, sizeof(ISP_CMOS_GAMMA_S));
            memcpy(&pstDef->stGammafe, &g_IspDft[1].stGammafe, sizeof(ISP_CMOS_GAMMAFE_S));
        break;

    }

    pstDef->stSensorMaxResolution.u32MaxWidth  = 1920;
    pstDef->stSensorMaxResolution.u32MaxHeight = 1080;

    return 0;
}

#else

static ISP_CMOS_AGC_TABLE_S g_stIspAgcTable =
{
    /* bvalid */
    1,

    /* sharpen_alt_d */
    {0x40,0x3e,0x3c,0x38,0x34,0x30,0x2c,0x28,0x20,0x18,0x14,0x10,0x10,0x10,0x10,0x10},
        
    /* sharpen_alt_ud */
    {0x60,0x60,0x58,0x50,0x48,0x40,0x38,0x28,0x20,0x20,0x10,0x10,0x10,0x10,0x10,0x10},
        
    /* snr_thresh */
    {0x08,0x0c,0x10,0x14,0x18,0x20,0x28,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},
        
    /* demosaic_lum_thresh */
    {0x50,0x50,0x4c,0x42,0x38,0x30,0x28,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
        
    /* demosaic_np_offset */
    {0x0,0xa,0x12,0x1a,0x20,0x28,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},
        
    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37},

    /* RGBsharpen_strength */
    {0x60,0x60,0x5c,0x52,0x48,0x40,0x38,0x28,0x20,0x20,0x10,0x10,0x10,0x10,0x10,0x10}
};

static ISP_CMOS_AGC_TABLE_S g_stIspAgcTableFSWDR =
{
    /* bvalid */
    1,

    /* sharpen_alt_d */
    {0x3C,0x3C,0x3C,0x3C,0x38,0x30,0x28,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
    
    /* sharpen_alt_ud */
    {0x60,0x60,0x60,0x60,0x50,0x40,0x30,0x20,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
        
    /* snr_thresh */
    {0x8,0xC,0x10,0x14,0x18,0x20,0x28,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},
        
    /* demosaic_lum_thresh */
    {0x50,0x50,0x40,0x40,0x30,0x30,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
        
    /* demosaic_np_offset */
    {0x0,0xa,0x12,0x1a,0x20,0x28,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},
        
    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37},

    /* RGBsharpen_strength */
    {0x60,0x60,0x60,0x60,0x50,0x40,0x30,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20}
};

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTable =
{
    /* bvalid */
    1,
    
    /* nosie_profile_weight_lut */
    {
        0,0,0,0,0,0,0,2,15,20,24,27,30,32,33,35,36,37,38,39,40,41,42,42,43,44,44,45,45,46,
        46,47,47,48,48,49,49,49,50,50,50,51,51,51,52,52,52,53,53,53,53,54,54,54,54,55,55,
        55,55,56,56,56,56,56,57,57,57,57,57,58,58,58,58,58,59,59,59,59,59,59,59,60,60,60,
        60,60,60,61,61,61,61,61,61,61,62,62,62,62,62,62,62,62,63,63,63,63,63,63,63,63,63,
        64,64,64,64,64,64,64,64,64,65,65,65,65,65,65,65,65

    },

    /* demosaic_weight_lut */
    {
        0,2,15,20,24,27,30,32,33,35,36,37,38,39,40,41,42,42,43,44,44,45,45,46,
        46,47,47,48,48,49,49,49,50,50,50,51,51,51,52,52,52,53,53,53,53,54,54,54,54,55,55,
        55,55,56,56,56,56,56,57,57,57,57,57,58,58,58,58,58,59,59,59,59,59,59,59,60,60,60,
        60,60,60,61,61,61,61,61,61,61,62,62,62,62,62,62,62,62,63,63,63,63,63,63,63,63,63,
        64,64,64,64,64,64,64,64,64,65,65,65,65,65,65,65,65,65,65,65,65,65,65
    }
};

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTableFSWDR =
{
    /* bvalid */
    1,
    
    /* nosie_profile_weight_lut */
    {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,5,7,10,13,15,18,21,23,26,29,31,34,37,39,42,45
    },

    /* demosaic_weight_lut */
    {
        3,12,16,20,22,25,26,28,29,30,32,33,34,34,35,36,37,37,38,38,39,39,40,40,41,41,42,42,43,43,43,44,44,44,45,45,45,46,46,46,47,47,47,47,48,48,48,48,49,49,49,49,50,50,50,50,50,51,51,51,51,51,52,52,52,52,52,52,53,53,53,53,53,53,54,54,54,54,54,54,54,55,55,55,55,55,55,55,56,56,56,56,56,56,56,56,57,57,57,57,57,57,57,57,57,58,58,58,58,58,58,58,58,58,58,59,59,59,59,59,59,59,59,59,59,59,59,59
    }
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    0xac,

    /*aa_slope*/
    0xaa,

    /*va_slope*/
    0xa8,

    /*uu_slope*/
    0xa0,

    /*sat_slope*/
    0x5d,

    /*ac_slope*/
    0xa0,
    
    /*fc_slope*/
    0x90,

    /*vh_thresh*/
    0x00,

    /*aa_thresh*/
    0x00,

    /*va_thresh*/
    0x00,

    /*uu_thresh*/
    0x08,

    /*sat_thresh*/
    0x00,

    /*ac_thresh*/
    0x1b3
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaicFSWDR =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    0xdc,

    /*aa_slope*/
    0xc8,

    /*va_slope*/
    0xb9,

    /*uu_slope*/
    0xa8,

    /*sat_slope*/
    0x5d,

    /*ac_slope*/
    0xa0,
    
    /*fc_slope*/
    0x80,

    /*vh_thresh*/
    0x00,

    /*aa_thresh*/
    0x00,

    /*va_thresh*/
    0x00,

    /*uu_thresh*/
    0x08,

    /*sat_thresh*/
    0x00,

    /*ac_thresh*/
    0x1b3
};

static ISP_CMOS_RGBSHARPEN_S g_stIspRgbSharpen =
{   
    /* bvalid */   
    1,   
    
    /*lut_core*/   
    255,  
    
    /*lut_strength*/  
    127, 
    
    /*lut_magnitude*/   
    8      
};

static ISP_CMOS_GAMMA_S g_stIspGamma =
{
    /* bvalid */
    1,
    
#if 1    
    {0, 180, 320, 426, 516, 590, 660, 730, 786, 844, 896, 946, 994, 1040, 1090, 1130, 1170, 1210, 1248,
    1296, 1336, 1372, 1416, 1452, 1486, 1516, 1546, 1580, 1616, 1652, 1678, 1714, 1742, 1776, 1798, 1830,
    1862, 1886, 1912, 1940, 1968, 1992, 2010, 2038, 2062, 2090, 2114, 2134, 2158, 2178, 2202, 2222, 2246,
    2266, 2282, 2300, 2324, 2344, 2360, 2372, 2390, 2406, 2422, 2438, 2458, 2478, 2494, 2510, 2526, 2546,
    2562, 2582, 2598, 2614, 2630, 2648, 2660, 2670, 2682, 2698, 2710, 2724, 2736, 2752, 2764, 2780, 2792,
    2808, 2820, 2836, 2848, 2864, 2876, 2888, 2896, 2908, 2920, 2928, 2940, 2948, 2960, 2972, 2984, 2992,
    3004, 3014, 3028, 3036, 3048, 3056, 3068, 3080, 3088, 3100, 3110, 3120, 3128, 3140, 3148, 3160, 3168,
    3174, 3182, 3190, 3202, 3210, 3218, 3228, 3240, 3256, 3266, 3276, 3288, 3300, 3306, 3318, 3326, 3334,
    3342, 3350, 3360, 3370, 3378, 3386, 3394, 3398, 3406, 3414, 3422, 3426, 3436, 3444, 3454, 3466, 3476,
    3486, 3498, 3502, 3510, 3518, 3526, 3530, 3538, 3546, 3554, 3558, 3564, 3570, 3574, 3582, 3590, 3598,
    3604, 3610, 3618, 3628, 3634, 3640, 3644, 3652, 3656, 3664, 3670, 3678, 3688, 3696, 3700, 3708, 3712,
    3716, 3722, 3730, 3736, 3740, 3748, 3752, 3756, 3760, 3766, 3774, 3778, 3786, 3790, 3800, 3808, 3812,
    3816, 3824, 3830, 3832, 3842, 3846, 3850, 3854, 3858, 3862, 3864, 3870, 3874, 3878, 3882, 3888, 3894,
    3900, 3908, 3912, 3918, 3924, 3928, 3934, 3940, 3946, 3952, 3958, 3966, 3974, 3978, 3982, 3986, 3990,
    3994, 4002, 4006, 4010, 4018, 4022, 4032, 4038, 4046, 4050, 4056, 4062, 4072, 4076, 4084, 4090, 4095
    }
#else  /*higher  contrast*/
    {0  , 54 , 106, 158, 209, 259, 308, 356, 403, 450, 495, 540, 584, 628, 670, 713,
    754 ,795 , 835, 874, 913, 951, 989,1026,1062,1098,1133,1168,1203,1236,1270,1303,
    1335,1367,1398,1429,1460,1490,1520,1549,1578,1607,1635,1663,1690,1717,1744,1770,
    1796,1822,1848,1873,1897,1922,1946,1970,1993,2017,2040,2062,2085,2107,2129,2150,
    2172,2193,2214,2235,2255,2275,2295,2315,2335,2354,2373,2392,2411,2429,2447,2465,
    2483,2501,2519,2536,2553,2570,2587,2603,2620,2636,2652,2668,2684,2700,2715,2731,
    2746,2761,2776,2790,2805,2819,2834,2848,2862,2876,2890,2903,2917,2930,2944,2957,
    2970,2983,2996,3008,3021,3033,3046,3058,3070,3082,3094,3106,3118,3129,3141,3152,
    3164,3175,3186,3197,3208,3219,3230,3240,3251,3262,3272,3282,3293,3303,3313,3323,
    3333,3343,3352,3362,3372,3381,3391,3400,3410,3419,3428,3437,3446,3455,3464,3473,
    3482,3490,3499,3508,3516,3525,3533,3541,3550,3558,3566,3574,3582,3590,3598,3606,
    3614,3621,3629,3637,3644,3652,3660,3667,3674,3682,3689,3696,3703,3711,3718,3725,
    3732,3739,3746,3752,3759,3766,3773,3779,3786,3793,3799,3806,3812,3819,3825,3831,
    3838,3844,3850,3856,3863,3869,3875,3881,3887,3893,3899,3905,3910,3916,3922,3928,
    3933,3939,3945,3950,3956,3962,3967,3973,3978,3983,3989,3994,3999,4005,4010,4015,
    4020,4026,4031,4036,4041,4046,4051,4056,4061,4066,4071,4076,4081,4085,4090,4095,4095}
#endif
};

static ISP_CMOS_GAMMA_S g_stIspGammaFSWDR =
{
    /* bvalid */
    1,
    
#if 0    
    {
        0,   1,   2,   4,   8,  12,  17,  23,  30,  38,  47,  57,  68,  79,  92, 105, 120, 133, 147, 161, 176, 192, 209, 226, 243, 260, 278, 296, 315, 333, 351, 370, 390, 410, 431, 453, 474, 494, 515, 536, 558, 580, 602, 623, 644, 665, 686, 708, 730, 751, 773, 795, 818, 840, 862, 884, 907, 929, 951, 974, 998,1024,1051,1073,1096,1117,1139,1159,1181,1202,1223,1243,1261,1275,1293,1313,1332,1351,1371,1389,1408,1427,1446,1464,1482,1499,1516,1533,1549,1567,1583,1600,1616,1633,1650,1667,1683,1700,1716,1732,1749,1766,1782,1798,1815,1831,1847,1863,1880,1896,1912,1928,1945,1961,1977,1993,2009,2025,2041,2057,2073,2089,2104,2121,2137,2153,2168,2184,2200,2216,2231,2248,2263,2278,2294,2310,2326,2341,2357,2373,2388,2403,2419,2434,2450,2466,2481,2496,2512,2527,2543,2558,2573,2589,2604,2619,2635,2650,2665,2680,2696,2711,2726,2741,2757,2771,2787,2801,2817,2832,2847,2862,2877,2892,2907,2922,2937,2952,2967,2982,2997,3012,3027,3041,3057,3071,3086,3101,3116,3130,3145,3160,3175,3190,3204,3219,3234,3248,3263,3278,3293,3307,3322,3337,3351,3365,3380,3394,3409,3424,3438,3453,3468,3482,3497,3511,3525,3540,3554,3569,3584,3598,3612,3626,3641,3655,3670,3684,3699,3713,3727,3742,3756,3770,3784,3799,3813,3827,3841,3856,3870,3884,3898,3912,3927,3941,3955,3969,3983,3997,4011,4026,4039,4054,4068,4082,4095
    }
#else  /*higher  contrast*/
    {
        0,1,2,4,8,12,17,23,30,38,47,57,68,79,92,105,120,133,147,161,176,192,209,226,243,260,278,296,317,340,365,390,416,440,466,491,517,538,561,584,607,631,656,680,705,730,756,784,812,835,858,882,908,934,958,982,1008,1036,1064,1092,1119,1143,1167,1192,1218,1243,1269,1296,1323,1351,1379,1408,1434,1457,1481,1507,1531,1554,1579,1603,1628,1656,1683,1708,1732,1756,1780,1804,1829,1854,1877,1901,1926,1952,1979,2003,2024,2042,2062,2084,2106,2128,2147,2168,2191,2214,2233,2256,2278,2296,2314,2335,2352,2373,2391,2412,2431,2451,2472,2492,2513,2531,2547,2566,2581,2601,2616,2632,2652,2668,2688,2705,2721,2742,2759,2779,2796,2812,2826,2842,2857,2872,2888,2903,2920,2934,2951,2967,2983,3000,3015,3033,3048,3065,3080,3091,3105,3118,3130,3145,3156,3171,3184,3197,3213,3224,3240,3252,3267,3281,3295,3310,3323,3335,3347,3361,3372,3383,3397,3409,3421,3432,3447,3459,3470,3482,3497,3509,3521,3534,3548,3560,3572,3580,3592,3602,3613,3625,3633,3646,3657,3667,3679,3688,3701,3709,3719,3727,3736,3745,3754,3764,3773,3781,3791,3798,3806,3816,3823,3833,3840,3847,3858,3865,3872,3879,3888,3897,3904,3911,3919,3926,3933,3940,3948,3955,3962,3970,3973,3981,3988,3996,4003,4011,4018,4026,4032,4037,4045,4053,4057,4064,4072,4076,4084,4088,4095        
        //0,1,2,4,8,12,17,23,30,38,47,57,68,79,92,105,120,133,147,161,176,192,209,226,243,260,278,296,317,340,365,390,416,440,466,491,517,538,561,584,607,631,656,680,705,730,756,784,812,835,858,882,908,934,958,982,1008,1036,1064,1092,1119,1143,1167,1192,1218,1243,1269,1294,1320,1346,1372,1398,1424,1450,1476,1502,1528,1554,1580,1607,1633,1658,1684,1710,1735,1761,1786,1811,1836,1860,1884,1908,1932,1956,1979,2002,2024,2046,2068,2090,2112,2133,2154,2175,2196,2217,2237,2258,2278,2298,2318,2337,2357,2376,2395,2414,2433,2451,2469,2488,2505,2523,2541,2558,2575,2592,2609,2626,2642,2658,2674,2690,2705,2720,2735,2750,2765,2779,2793,2807,2821,2835,2848,2861,2874,2887,2900,2913,2925,2937,2950,2962,2974,2986,2998,3009,3021,3033,3044,3056,3067,3078,3088,3099,3109,3119,3129,3139,3148,3158,3168,3177,3187,3197,3207,3217,3227,3238,3248,3259,3270,3281,3292,3303,3313,3324,3335,3346,3357,3368,3379,3389,3400,3410,3421,3431,3441,3451,3461,3471,3481,3491,3501,3511,3521,3531,3541,3552,3562,3572,3583,3593,3604,3615,3625,3636,3646,3657,3668,3679,3689,3700,3711,3721,3732,3743,3753,3764,3774,3784,3795,3805,3816,3826,3837,3847,3858,3869,3880,3891,3902,3913,3925,3937,3949,3961,3973,3985,3997,4009,4022,4034,4046,4058,4071,4083,4095
    }
#endif
};

static ISP_CMOS_GAMMAFE_S g_stGammafeFSWDR = 
{
    /* bvalid */
    1,

    /* gamma_fe0 */
    {
        0, 17537, 35074, 36089, 37105, 38120, 39136, 40151, 41166, 42182, 43197, 44213, 45228, 46243, 47259, 48274, 49290, 50305, 51320, 52336, 53351, 54367, 55382, 56397, 57413, 58428, 59444, 60459, 61474, 62490, 63505, 64521, 65535
    },

    /* gamma_fe1 */
    {
        0, 73, 147, 221, 296, 371, 447, 524, 601, 678, 757, 837, 916, 997, 1078, 1160, 1243, 1326, 1410, 1496, 1582, 1669, 1757, 1846, 1936, 2026, 2119, 2212, 2305, 2401, 2498, 2595, 2694, 2795, 2897, 3000, 3105, 3212, 3320, 3430, 3542, 3656, 3772, 3890, 4011, 4133, 4259, 4388, 4519, 4654, 4792, 4935, 5081, 5231, 5387, 5548, 5715, 5889, 6071, 6262, 6462, 6676, 6903, 7150, 7420, 7722, 8071, 8500, 9122, 10827, 11453, 11883, 12233, 12535, 12805, 13051, 13279, 13492, 13693, 13884, 14066, 14240, 14407, 14568, 14724, 14875, 15021, 15163, 15301, 15436, 15568, 15696, 15822, 15945, 16066, 16184, 16300, 16414, 16526, 16636, 16744, 16851, 16956, 17059, 17161, 17261, 17361, 17459, 17555, 17650, 17745, 17837, 17929, 18021, 18110, 18199, 18287, 18374, 18460, 18545, 18630, 18713, 18796, 18878, 18959, 19040, 19120, 19199, 19277, 19355, 19432, 19509, 19585, 19660, 19735, 19809, 19883, 19956, 21136, 22209, 23196, 24114, 24975, 25789, 26564, 27304, 28014, 28697, 29357, 29994, 30611, 31210, 31793, 32361, 32915, 33456, 33985, 34503, 35009, 35506, 35993, 36471, 36941, 37402, 37856, 38302, 38742, 39175, 39602, 40023, 40437, 40846, 41250, 41649, 42043, 42432, 42817, 43197, 43573, 43945, 44312, 44676, 45035, 45392, 45744, 46093, 46439, 46782, 47121, 47458, 47791, 48122, 48449, 48774, 49096, 49416, 49733, 50047, 50360, 50669, 50977, 51282, 51584, 51885, 52183, 52480, 52774, 53066, 53357, 53645, 53931, 54216, 54499, 54780, 55059, 55337, 55612, 55887, 56160, 56431, 56700, 56968, 57234, 57499, 57762, 58024, 58285, 58544, 58802, 59058, 59313, 59567, 59819, 60070, 60320, 60569, 60817, 61063, 61308, 61552, 61795, 62037, 62277, 62517, 62755, 62992, 63229, 63464, 63698, 63931, 64163, 64394, 64624, 64854, 65082, 65309, 65535
    }
};

HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    
    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:
            
            pstDef->stDrc.bEnable               = HI_FALSE;
            pstDef->stDrc.u32BlackLevel         = 0x00;
            pstDef->stDrc.u32WhiteLevel         = 0x4FF; 
            pstDef->stDrc.u32SlopeMax           = 0x30;
            pstDef->stDrc.u32SlopeMin           = 0x00;
            pstDef->stDrc.u32VarianceSpace      = 0x04;
            pstDef->stDrc.u32VarianceIntensity  = 0x01;
            pstDef->stDrc.u32Asymmetry          = 0x14;
            pstDef->stDrc.u32BrightEnhance      = 0xC8;
            
            memcpy(&pstDef->stAgcTbl, &g_stIspAgcTable, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTable, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stDemosaic, &g_stIspDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
            memcpy(&pstDef->stGamma, &g_stIspGamma, sizeof(ISP_CMOS_GAMMA_S));
        break;
        case WDR_MODE_2To1_LINE:
        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE:
            
            pstDef->stDrc.bEnable               = HI_TRUE;
            pstDef->stDrc.u32BlackLevel         = 0x00;
            pstDef->stDrc.u32WhiteLevel         = 0xFFF; 
            pstDef->stDrc.u32SlopeMax           = 0x38;
            pstDef->stDrc.u32SlopeMin           = 0xC0;
            pstDef->stDrc.u32VarianceSpace      = 0x06;
            pstDef->stDrc.u32VarianceIntensity  = 0x08;
            pstDef->stDrc.u32Asymmetry          = 0x14;
            pstDef->stDrc.u32BrightEnhance      = 0xC8;

            memcpy(&pstDef->stAgcTbl, &g_stIspAgcTableFSWDR, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTableFSWDR, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stDemosaic, &g_stIspDemosaicFSWDR, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));            
            memcpy(&pstDef->stGamma, &g_stIspGammaFSWDR, sizeof(ISP_CMOS_GAMMA_S));
            memcpy(&pstDef->stGammafe, &g_stGammafeFSWDR, sizeof(ISP_CMOS_GAMMAFE_S));
        break;

    }

    pstDef->stSensorMaxResolution.u32MaxWidth  = 1920;
    pstDef->stSensorMaxResolution.u32MaxHeight = 1080;

    return 0;
}

#endif

HI_U32 cmos_get_isp_black_level(ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel)
{
    HI_S32  i;
    
    if (HI_NULL == pstBlackLevel)
    {
        printf("null pointer when get isp black level value!\n");
        return -1;
    }

    /* Don't need to update black level when iso change */
    pstBlackLevel->bUpdate = HI_FALSE;
          
    for (i=0; i<4; i++)
    {
        pstBlackLevel->au16BlackLevel[i] = 0x100; 
    }

    return 0;  
    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    HI_U32 u32FullLines_5Fps, u32MaxIntTime_5Fps;
    
    if (WDR_MODE_2To1_LINE == genSensorMode)
    {
        return;
    }
    else
    {
        if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (VMAX_1080P30_LINEAR * 30) / 5;
        }
        else if (SENSOR_1080P_60FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (VMAX_1080P60_LINEAR * 60) / 5;
        }
        else if (SENSOR_720P_120FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (VMAX_720P120_LINEAR * 120) / 5;
        }
        else if (SENSOR_VGA_160FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (VMAX_VGA160_LINEAR * 160) / 5;
        }
        else
        {
            return;
        }
    }

    u32FullLines_5Fps = (u32FullLines_5Fps > 0xFFFF) ? 0xFFFF : u32FullLines_5Fps;
    u32MaxIntTime_5Fps = u32FullLines_5Fps - 2;

    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register(VMAX_ADDR_H, (u32FullLines_5Fps & 0xFF00) >> 8);  /* 5fps */
        sensor_write_register(VMAX_ADDR_L, u32FullLines_5Fps & 0xFF);           /* 5fps */
        sensor_write_register(0x0202, (u32MaxIntTime_5Fps & 0xFF00) >> 8);      /* max exposure lines */
        sensor_write_register(0x0203, u32MaxIntTime_5Fps & 0xFF);               /* max exposure lines */
        sensor_write_register(0x0204, 0x01);                                    /* min AG */
        sensor_write_register(0x0205, 0x00);                                    /* min AG */
        sensor_write_register(0x3108, 0x01);                                    /* min DG */
        sensor_write_register(0x3109, 0x00);                                    /* min DG */
    }
    else /* setup for ISP 'normal mode' */
    {
        sensor_write_register(VMAX_ADDR_H, (gu32FullLinesStd & 0xFF00) >> 8);
        sensor_write_register(VMAX_ADDR_L, gu32FullLinesStd & 0xFF);
        bInit = HI_FALSE;
    }

    return;
}

HI_VOID cmos_set_wdr_mode(HI_U8 u8Mode)
{
    bInit = HI_FALSE;
    
    switch(u8Mode)
    {
        case WDR_MODE_NONE:
            if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_1080P30_LINEAR;
            }
            else if (SENSOR_1080P_60FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_1080P60_LINEAR;
            }
            else if (SENSOR_720P_120FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_720P120_LINEAR;
            }
            else if (SENSOR_VGA_160FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_VGA160_LINEAR;
            }
            genSensorMode = WDR_MODE_NONE;
            printf("linear mode\n");
        break;

        case WDR_MODE_2To1_LINE:
            gu32FullLinesStd = VMAX_1080P60TO30_WDR;
            genSensorMode = WDR_MODE_2To1_LINE;
            printf("2to1 line WDR mode\n");
        break;

        case WDR_MODE_2To1_FRAME:
            if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_1080P30_LINEAR;
            }
            else if (SENSOR_1080P_60FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_1080P60_LINEAR;
            }
            genSensorMode = WDR_MODE_2To1_FRAME;
        
            printf("2to1 half-rate frame WDR mode\n");
        break;

        case WDR_MODE_2To1_FRAME_FULL_RATE:
            if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_1080P30_LINEAR;
            }
            else if (SENSOR_1080P_60FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_1080P60_LINEAR;
            }
            genSensorMode = WDR_MODE_2To1_FRAME_FULL_RATE;

            printf("2to1 full-rate frame WDR mode\n");
        break;

        default:
            printf("NOT support this mode!\n");
            return;
        break;
    }
    
    return;
}

HI_U32 cmos_get_sns_regs_info(ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
    HI_S32 i;

    if (HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
        g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 2;
        
        g_stSnsRegsInfo.u32RegNum = 9;
        if (WDR_MODE_2To1_LINE == genSensorMode)
        {
            g_stSnsRegsInfo.u32RegNum += 2;
        }
        if ((WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode) || (WDR_MODE_2To1_FRAME == genSensorMode))
        {
            g_stSnsRegsInfo.u32RegNum += 2;
        }
        
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
        {
            g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }
        /* Shutter (Shutter Long) */
        g_stSnsRegsInfo.astI2cData[0].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = EXPOSURE_ADDR_H;
        g_stSnsRegsInfo.astI2cData[1].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = EXPOSURE_ADDR_L;

        /* OB correction filter coefficient */
        g_stSnsRegsInfo.astI2cData[2].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = 0x3280;

        /* AG */
        g_stSnsRegsInfo.astI2cData[3].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = AGC_ADDR_H;
        g_stSnsRegsInfo.astI2cData[4].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[4].u32RegAddr = AGC_ADDR_L;

        /* DG */
        g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[5].u32RegAddr = DGC_ADDR_H;
        g_stSnsRegsInfo.astI2cData[6].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[6].u32RegAddr = DGC_ADDR_L;

        /* VMAX */
        g_stSnsRegsInfo.astI2cData[7].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[7].u32RegAddr = VMAX_ADDR_H;
        g_stSnsRegsInfo.astI2cData[8].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[8].u32RegAddr = VMAX_ADDR_L;
        
        if (WDR_MODE_2To1_LINE == genSensorMode)
        {
            g_stSnsRegsInfo.astI2cData[7].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astI2cData[7].u32RegAddr = SHORT_EXPOSURE_ADDR_H;
            g_stSnsRegsInfo.astI2cData[8].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astI2cData[8].u32RegAddr = SHORT_EXPOSURE_ADDR_L;
            
            g_stSnsRegsInfo.astI2cData[9].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[9].u32RegAddr = VMAX_ADDR_H;
            g_stSnsRegsInfo.astI2cData[10].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[10].u32RegAddr = VMAX_ADDR_L;            
        }
        if ((WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode) || (WDR_MODE_2To1_FRAME == genSensorMode))
        {
            g_stSnsRegsInfo.astI2cData[7].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astI2cData[7].u32RegAddr = EXPOSURE_ADDR_H;
            g_stSnsRegsInfo.astI2cData[8].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astI2cData[8].u32RegAddr = EXPOSURE_ADDR_L;

            g_stSnsRegsInfo.astI2cData[9].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[9].u32RegAddr = VMAX_ADDR_H;
            g_stSnsRegsInfo.astI2cData[10].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[10].u32RegAddr = VMAX_ADDR_L;             
        }

        bInit = HI_TRUE;
    }
    else
    {
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
        {
            if (g_stSnsRegsInfo.astI2cData[i].u32Data == g_stPreSnsRegsInfo.astI2cData[i].u32Data)
            {
                g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_FALSE;
            }
            else
            {
                g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            }
        }

        if ((WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode) || (WDR_MODE_2To1_FRAME == genSensorMode))
        {
            g_stSnsRegsInfo.astI2cData[0].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[1].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[7].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[8].bUpdate = HI_TRUE;
        }
    }
    
    if (HI_NULL == pstSnsRegsInfo)
    {
        printf("null pointer when get sns reg info!\n");
        return -1;
    }

    memcpy(pstSnsRegsInfo, &g_stSnsRegsInfo, sizeof(ISP_SNS_REGS_INFO_S));
    memcpy(&g_stPreSnsRegsInfo, &g_stSnsRegsInfo, sizeof(ISP_SNS_REGS_INFO_S));

    return 0;
}

static HI_S32 cmos_set_image_mode(ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    HI_U8 u8SensorImageMode = gu8SensorImageMode;
    
    bInit = HI_FALSE;    
        
    if (HI_NULL == pstSensorImageMode )
    {
        printf("null pointer when set image mode\n");
        return -1;
    }

    if ((pstSensorImageMode->u16Width <= 640) && (pstSensorImageMode->u16Height <= 480))
    {
        if (WDR_MODE_NONE == genSensorMode)
        {
            if (pstSensorImageMode->f32Fps <= 160)
            {
                u8SensorImageMode = SENSOR_VGA_160FPS_MODE;
            }
            else
            {
                printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);
            }
        }
        else
        {
            printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);
        }
    }
    else if ((pstSensorImageMode->u16Width <= 1280) && (pstSensorImageMode->u16Height <= 720))
    {
        if (WDR_MODE_NONE == genSensorMode)
        {
            if (pstSensorImageMode->f32Fps <= 120)
            {
                u8SensorImageMode = SENSOR_720P_120FPS_MODE;
            }
            else
            {
                printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);
            }
        }
        else
        {
            printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);
        }
    }
    else if ((pstSensorImageMode->u16Width <= 1920) && (pstSensorImageMode->u16Height <= 1080))
    {
        if (WDR_MODE_2To1_LINE == genSensorMode)
        {
            if (pstSensorImageMode->f32Fps > 30)
            {
                printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);
                
                return -1;
            }
        }
        else
        {
            if (pstSensorImageMode->f32Fps <= 30)
            {
                u8SensorImageMode = SENSOR_1080P_30FPS_MODE;
            }
            else if (pstSensorImageMode->f32Fps <= 60)
            {
                u8SensorImageMode = SENSOR_1080P_60FPS_MODE;
            }
            else
            {
                printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);

                return -1;
            }
        }
    }
    else
    {
        printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
            pstSensorImageMode->u16Width, 
            pstSensorImageMode->u16Height,
            pstSensorImageMode->f32Fps,
            genSensorMode);

        return -1;
    }

    /* Sensor first init */
    if (HI_FALSE == bSensorInit)
    {
        gu8SensorImageMode = u8SensorImageMode;
        
        return 0;
    }

    /* Switch SensorImageMode */
    if (u8SensorImageMode == gu8SensorImageMode)
    {
        /* Don't need to switch SensorImageMode */
        return -1;
    }
    
    gu8SensorImageMode = u8SensorImageMode;

    return 0;
}

int  sensor_set_inifile_path(const char *pcPath)
{
    memset(pcName, 0, sizeof(pcName));
        
    if (HI_NULL == pcPath)
    {        
        strncat(pcName, "configs/", strlen("configs/"));
        strncat(pcName, CMOS_CFG_INI, sizeof(CMOS_CFG_INI));
    }
    else
    {
        if(strlen(pcPath) > (PATHLEN_MAX - 30))
        {
            printf("Set inifile path is larger PATHLEN_MAX!\n");
            return -1;        
        }
        
        strncat(pcName, pcPath, strlen(pcPath));
        strncat(pcName, CMOS_CFG_INI, sizeof(CMOS_CFG_INI));
    }
    
    return 0;
}

HI_VOID sensor_global_init()
{     
    gu8SensorImageMode = SENSOR_1080P_30FPS_MODE;
    genSensorMode = WDR_MODE_NONE;
    gu32FullLinesStd = VMAX_1080P30_LINEAR;
    bInit = HI_FALSE;
    bSensorInit = HI_FALSE; 

    memset(&g_stSnsRegsInfo, 0, sizeof(ISP_SNS_REGS_INFO_S));
    memset(&g_stPreSnsRegsInfo, 0, sizeof(ISP_SNS_REGS_INFO_S));
  
#ifdef INIFILE_CONFIG_MODE 
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = Cmos_LoadINIPara(pcName);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Cmos_LoadINIPara failed!!!!!!\n");
    }
#else

#endif

}

HI_S32 cmos_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = sensor_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = sensor_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = sensor_global_init;
    pstSensorExpFunc->pfn_cmos_set_image_mode = cmos_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = cmos_set_wdr_mode;
    
    pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
    pstSensorExpFunc->pfn_cmos_get_isp_black_level = cmos_get_isp_black_level;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = cmos_get_sns_regs_info;

    return 0;
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/
 
int sensor_register_callback(void)
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    ALG_LIB_S stLib;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;

    cmos_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, MN34220_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, MN34220_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, MN34220_ID, &stAwbRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to awb lib failed!\n");
        return s32Ret;
    }
    
    return 0;
}

int sensor_unregister_callback(void)
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    ALG_LIB_S stLib;

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, MN34220_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, MN34220_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, MN34220_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }
    
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __MN34220_CMOS_H_ */
