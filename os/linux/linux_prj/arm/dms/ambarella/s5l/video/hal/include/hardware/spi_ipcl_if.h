/******************************************************************************
*                      Copyright Statement
*              CONFIDENTIAL VISTEON CORPORATION
*
*  This is an unpublished work of authorship, which contains trade secrets,
*  created in 2012. Visteon Corporation owns all rights to this work and
*  intends to maintain it in confidence to preserve its trade secret status.
*  Visteon Corporation reserves the right, under the copyright laws of the
*  United States or those of any other country that may have jurisdiction,
*  to protect this work as an unpublished work, in the event of an inadvertent
*  or deliberate unauthorized publication. Visteon Corporation also reserves
*  its rights under all copyright laws to protect this work as a published
*  work, when appropriate. Those having access to this work may not copy it,
*  use it, modify it or disclose the information contained in it without the
*  written authorization of Visteon Corporation.
*
*******************************************************************************
*
*  Module:             spi_ipcl_if.h
*  Description:        IPC communication module
*  Project Scope:      2013 Renault/Nissan R0-13
*
*  Author:             Vijaya kumar Munusamy
*  Compiler/Assembler: Cross compiler arm-none-linux-gnueabi-gcc
*  Target Hardware:    R0-13
*  Date:               05/04/2012
*
*******************************************************************************
*  History:
*******************************************************************************
*  05/30/2012     v0.01   Vijaya kumar Munusamy      Updated version
*******************************************************************************/

#ifndef _SPI_IPCL_H_
#define _SPI_IPCL_H_

#include<stdio.h>
#include"hal_common_if.h"
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<semaphore.h>

/*-------Project Configuration---------*/
#define IPCL_LFC		0
#define IPCL_R015	1
#define IPCL_NGI		2
#define IPCL_STD		3
#define CURRENT_IPCL	IPCL_NGI	

#define IPCL_ENABLE	(1)
#define IPCL_DISABLE 	(0)

#define IPCL_HOST_SW_VER	"1.5.0"
#define HOST_SRQ_REQUEST_SPI_COM 	(IPCL_DISABLE)

/*-----------Trace configuration-------------*/
#define IPCL_SPI_TRACE(x)  		//write(1, x, strlen(x))
#define IPCL_SPI_DEBUG(str, ...) 	//printf("IPCL SPI: " str, __VA_ARGS__)
#define IPCL_SPI_ERROR(x) 		write(1, x, strlen(x))

/*---------IPCL Send and Receive Frame Size Configuration----------*/
#define MAX_MSG_FRAME		(1096)	
#define MAX_IPCL_FRAME		(1100) 

#define ACK_FRAME_LENGTH 		(5)	
#define ACK_DATA_LENGTH 		(3)	

#define IPCL_MSG_HEAD			(2)	/*1: Start position of the first IPCL message; 2: IPCL message head length is 2 bytes.*/
#define IPCL_FRAME_HEAD		(2)	/*Data Frame Length*/
#define IPCL_FRAME_TAIL			(2)	/*Data Frame Checksum length*/

/*-------Global definitions-----*/
typedef enum
{
	SPI_INIT_STATUS_SUCCESS = 0,
	SPI_INIT_STATUS_SRQ_FAIL,
	SPI_INIT_STATUS_CS_FAIL,
	SPI_INIT_STATUS_SPI_OPEN_FAIL,
	SPI_INIT_STATUS_TX_QUEUE_FAIL,
	SPI_INIT_STATUS_RX_QUEUE_FAIL,
	SPI_INIT_STATUS_SIGACTION_FAILED,
}SPI_INIT_STATUS;

typedef enum 
{
	WRITE_SUCCESS = 0,
	WRITE_QUEUE_FULL,
	READ_SUCCESS,
	READ_FAIL,
	CHECKSUM_ERROR,
	READ_BUFF_EMPTY,
}BUFFER_STATUS;

/**********************************************************************************************
*   Function name:  spi_ipcl_init
*   Description:    This function needs to be called only once by VGW. 
*  		    It will create the semaphores and thread needed for communication
*   Parameters:     void
*   Return value:   SPI_INIT_STATUS
***********************************************************************************************/
SPI_INIT_STATUS spi_ipcl_init ( void );

/**********************************************************************************************
*   Function name:  spi_ipcl_write
*   Description:    spi_ipcl_write() API Implementation that can be used by VGW 
*   Parameters:     Buffer:		Pointer to data bytes 
*		   
*   Return value:   BUFFER_STATUS
***********************************************************************************************/
extern BUFFER_STATUS spi_ipcl_write(const unsigned8 *vgw_message);

/**********************************************************************************************
*   Function name:  spi_ipcl_read
*   Description:    spi_ipcl_read() API Implementation that can be used by VGW 
*   Parameters:     Buffer:		Pointer to data bytes
*		    Buffer_size:	Length of the data bytes
*   Return value:   BUFFER_STATUS 
***********************************************************************************************/
extern BUFFER_STATUS spi_ipcl_read(unsigned8 *ipcl_message, unsigned32 *ipcl_message_len);

/**********************************************************************************************
*   Function name:  spi_ipcl_close
*   Description:    This function needs to be called only once by VGW.
*  		    It will destroy the mutex for read and write, remove the Tx and Rx Message queue
*		    and also close the spi and corresponding gpio lines.
*   Parameters:     void
*   Return value:   void
***********************************************************************************************/
extern void spi_ipcl_close ( void );

#endif
