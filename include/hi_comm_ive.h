/******************************************************************************

  Copyright (C), 2001-2014, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_ive.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software (IVE) group
  Created       : 2011/05/16
  Description   : 
  History       :
  1.Date        : 2011/05/16
    Author      : j00169368
    Modification: Created file

  2.Date        : 2013/07/01~2014/08/08
    Author      : t00228657\c00211359\c00206215
    Modification: Add MPI function
******************************************************************************/
#ifndef _HI_COMM_IVE_H_
#define _HI_COMM_IVE_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#include "hi_type.h"
#include "hi_errno.h"

/*-----------------------------------------------*
 * The fixed-point data type, will be used to    *
 * represent float data in hardware calculations.*
 *-----------------------------------------------*/

/*--u8bit----------------------------------------*/
typedef unsigned char           HI_U0Q8;
typedef unsigned char           HI_U1Q7;
typedef unsigned char           HI_U5Q3;

/*--u16bit---------------------------------------*/
typedef unsigned short          HI_U0Q16;
typedef unsigned short          HI_U4Q12;
typedef unsigned short          HI_U6Q10;
typedef unsigned short          HI_U8Q8;
typedef unsigned short          HI_U12Q4;
typedef unsigned short          HI_U14Q2;

/*--s16bit---------------------------------------*/
typedef short                   HI_S9Q7;
typedef short                   HI_S14Q2;
typedef short                   HI_S1Q15;

/*--u32bit---------------------------------------*/
typedef unsigned int            HI_U22Q10;
typedef unsigned int            HI_U25Q7;

/*--s32bit---------------------------------------*/
typedef int                     HI_S25Q7;
typedef int                     HI_S16Q16;

/*-----------------------------------------------*
 * The fixed-point data type combine with flag_bits.*
 *-----------------------------------------------*/

/*8bits unsigned integer,4bits decimal fraction,4bits flag_bits*/
typedef unsigned short          HI_U8Q4F4;

/*float*/
typedef float                   HI_FLOAT;
/*double*/
typedef double                  HI_DOUBLE;

/*
* Type of the IVE_IMAGE_S data.
* Aded by tanbing 2013-7-22
*/
typedef enum hiIVE_IMAGE_TYPE_E
{
	IVE_IMAGE_TYPE_U8C1           =  0x0,
	IVE_IMAGE_TYPE_S8C1           =  0x1,

	IVE_IMAGE_TYPE_YUV420SP       =  0x2,		/*YUV420 SemiPlanar*/
	IVE_IMAGE_TYPE_YUV422SP       =  0x3,		/*YUV422 SemiPlanar*/
	IVE_IMAGE_TYPE_YUV420P        =  0x4,		/*YUV420 Planar */
	IVE_IMAGE_TYPE_YUV422P		  =	 0x5,       /*YUV422 planar */

	IVE_IMAGE_TYPE_S8C2_PACKAGE   =  0x6,
	IVE_IMAGE_TYPE_S8C2_PLANAR    =  0x7,

	IVE_IMAGE_TYPE_S16C1          =  0x8,
	IVE_IMAGE_TYPE_U16C1          =  0x9,

	IVE_IMAGE_TYPE_U8C3_PACKAGE   =  0xa,
	IVE_IMAGE_TYPE_U8C3_PLANAR    =  0xb,

	IVE_IMAGE_TYPE_S32C1          =  0xc,		
	IVE_IMAGE_TYPE_U32C1          =  0xd,

	IVE_IMAGE_TYPE_S64C1          =  0xe,
	IVE_IMAGE_TYPE_U64C1          =  0xf,

	IVE_IMAGE_TYPE_BUTT

}IVE_IMAGE_TYPE_E;

/*
* Definition of the IVE_IMAGE_S.
* Added by Tan Bing, 2013-7-22.
*/
typedef struct hiIVE_IMAGE_S
{
	IVE_IMAGE_TYPE_E  enType;

	HI_U32  u32PhyAddr[3];
	HI_U8  *pu8VirAddr[3];

	HI_U16  u16Stride[3];
	HI_U16  u16Width;
	HI_U16  u16Height;
	
	HI_U16  u16Reserved;		/*Can be used such as elemSize*/
}IVE_IMAGE_S;

typedef IVE_IMAGE_S IVE_SRC_IMAGE_S;
typedef IVE_IMAGE_S IVE_DST_IMAGE_S;

/*
* Definition of the IVE_MEM_INFO_S.This struct special purpose for input or ouput, such as Hist, CCL, ShiTomasi.
* Added by Chen Quanfu, 2013-7-23.
*/
typedef struct hiIVE_MEM_INFO_S
{
	HI_U32  u32PhyAddr;
	HI_U8  *pu8VirAddr;
	HI_U32  u32Size;
}IVE_MEM_INFO_S;
typedef IVE_MEM_INFO_S IVE_SRC_MEM_INFO_S;
typedef IVE_MEM_INFO_S IVE_DST_MEM_INFO_S;

/*
*Data struct ,created by Chen Quanfu 2013-07-19
*/
typedef struct hiIVE_DATA_S
{
	HI_U32  u32PhyAddr;  /*Physical address of the data*/
	HI_U8  *pu8VirAddr;

	HI_U16  u16Stride;   /*2D data stride by byte*/
	HI_U16  u16Width;    /*2D data width by byte*/
	HI_U16  u16Height;   /*2D data height*/

    HI_U16 u16Reserved;
}IVE_DATA_S;
typedef IVE_DATA_S IVE_SRC_DATA_S;
typedef IVE_DATA_S IVE_DST_DATA_S;

/*
* Definition of the union of IVE_8BIT_U.
* Added by Tan Bing, 2013-7-22.
*/
typedef union hiIVE_8BIT_U
{
	HI_S8 s8Val;
	HI_U8 u8Val;
}IVE_8BIT_U;	

/*
* Definition of u16 point
*/
typedef struct hiIVE_POINT_U16_S
{
    HI_U16 u16X;
    HI_U16 u16Y;
}IVE_POINT_U16_S;

/*
*Float point represented by Fixed-point SQ25.7
*/
typedef struct hiIVE_POINT_S25Q7_S
{
    HI_S25Q7     s25q7X;                /*X coordinate*/
    HI_S25Q7     s25q7Y;                /*Y coordinate*/
}IVE_POINT_S25Q7_S;

/*
* Definition of rect
*/
typedef struct hiIVE_RECT_U16_S
{
    HI_U16 u16X;
    HI_U16 u16Y;
    HI_U16 u16Width;
    HI_U16 u16Height;
}IVE_RECT_U16_S;

typedef struct hiIVE_LOOK_UP_TABLE_S
{
    IVE_MEM_INFO_S stTable;
    HI_U16 u16ElemNum;          /*LUT's elements number*/

    HI_U8 u8TabInPreci;
    HI_U8 u8TabOutNorm;

    HI_S32 s32TabInLower;       /*LUT's original input lower limit*/
    HI_S32 s32TabInUpper;       /*LUT's original input upper limit*/
}IVE_LOOK_UP_TABLE_S;


typedef enum hiEN_IVE_ERR_CODE_E
{
    ERR_IVE_SYS_TIMEOUT    = 0x40,   /* IVE process timeout */
    ERR_IVE_QUERY_TIMEOUT  = 0x41,   /* IVE query timeout */
    ERR_IVE_OPEN_FILE      = 0x42,   /* IVE open file error */
    ERR_IVE_READ_FILE      = 0x43,   /* IVE read file error */
    ERR_IVE_WRITE_FILE     = 0x44,   /* IVE write file error */
	
    ERR_IVE_BUTT
}EN_IVE_ERR_CODE_E;

typedef enum hiEN_FD_ERR_CODE_E
{
	ERR_FD_SYS_TIMEOUT   = 0x40,	 /* FD process timeout */
	ERR_FD_CFG           = 0x41,	 /* FD configuration error */
	ERR_FD_FACE_NUM_OVER = 0x42,   	 /* FD condidate face num over*/
	
	ERR_FD_BUTT
}EN_FD_ERR_CODE_E;

/*Invalid device ID*/
#define HI_ERR_IVE_INVALID_DEVID     HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/*Invalid channel ID*/
#define HI_ERR_IVE_INVALID_CHNID     HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/*At least one parameter is illegal. For example, an illegal enumeration value exists.*/
#define HI_ERR_IVE_ILLEGAL_PARAM     HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/*The channel exists.*/
#define HI_ERR_IVE_EXIST             HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/*The UN exists.*/
#define HI_ERR_IVE_UNEXIST           HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/*A null point is used.*/
#define HI_ERR_IVE_NULL_PTR          HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/*Try to enable or initialize the system, device, or channel before configuring attributes.*/
#define HI_ERR_IVE_NOT_CONFIG        HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/*The operation is not supported currently.*/
#define HI_ERR_IVE_NOT_SURPPORT      HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/*The operation, changing static attributes for example, is not permitted.*/
#define HI_ERR_IVE_NOT_PERM          HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/*A failure caused by the malloc memory occurs.*/
#define HI_ERR_IVE_NOMEM             HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/*A failure caused by the malloc buffer occurs.*/
#define HI_ERR_IVE_NOBUF             HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/*The buffer is empty.*/
#define HI_ERR_IVE_BUF_EMPTY         HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/*No buffer is provided for storing new data.*/
#define HI_ERR_IVE_BUF_FULL          HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/*The system is not ready because it may be not initialized or loaded.
 *The error code is returned when a device file fails to be opened. */
#define HI_ERR_IVE_NOTREADY          HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)

/*The source address or target address is incorrect during the operations such as calling copy_from_user or copy_to_user.*/
#define HI_ERR_IVE_BADADDR           HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/*The resource is busy during the operations such as destroying a VENC channel without deregistering it.*/
#define HI_ERR_IVE_BUSY              HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/*IVE process timeout: 0xA01D8040*/
#define HI_ERR_IVE_SYS_TIMEOUT       HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, ERR_IVE_SYS_TIMEOUT)
/*IVE query timeout: 0xA01D8041*/
#define HI_ERR_IVE_QUERY_TIMEOUT     HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, ERR_IVE_QUERY_TIMEOUT)
/*IVE open file error: 0xA01D8042*/
#define HI_ERR_IVE_OPEN_FILE         HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, ERR_IVE_OPEN_FILE)
/*IVE read file error: 0xA01D8043*/
#define HI_ERR_IVE_READ_FILE         HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, ERR_IVE_READ_FILE)
/*IVE read file error: 0xA01D8044*/
#define HI_ERR_IVE_WRITE_FILE        HI_DEF_ERR(HI_ID_IVE, EN_ERR_LEVEL_ERROR, ERR_IVE_WRITE_FILE)

/*FD precess timeout:         0xA02F8040*/
#define HI_ERR_FD_SYS_TIMEOUT        HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, ERR_FD_SYS_TIMEOUT)
/*FD configuration error:     0xA02F8041*/
#define HI_ERR_FD_CFG                HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, ERR_FD_CFG)
/*FD condidate face num over: 0xA02F8042*/
#define HI_ERR_FD_FACE_NUM_OVER      HI_DEF_ERR(HI_ID_FD, EN_ERR_LEVEL_ERROR, ERR_FD_FACE_NUM_OVER)

/*ODT exist: 0xA0308004*/
#define HI_ERR_ODT_EXIST			 HI_DEF_ERR(HI_ID_ODT, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/*ODT the system is not ready because it may be not initialized: 0xA0308010*/
#define HI_ERR_ODT_NOTREADY			 HI_DEF_ERR(HI_ID_ODT, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/*ODT busy: 0xA0308012*/
#define HI_ERR_ODT_BUSY				 HI_DEF_ERR(HI_ID_ODT, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif/*__HI_COMM_IVE_H__*/
