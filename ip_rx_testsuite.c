/******************************************************************************
*
* Copyright (C) 2013 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
 *
 * @file XLlFifo_polling_example.c
 * This file demonstrates how to use the Streaming fifo driver on the xilinx AXI
 * Streaming FIFO IP.The AXI4-Stream FIFO core allows memory mapped access to a
 * AXI-Stream interface. The core can be used to interface to AXI Streaming IPs
 * similar to the LogiCORE IP AXI Ethernet core, without having to use full DMA
 * solution.
 *
 * This is the polling example for the FIFO it assumes that at the
 * h/w level FIFO is connected in loopback.In these we write known amount of
 * data to the FIFO and Receive the data and compare with the data transmitted.
 *
 * Note: The TDEST Must be enabled in the H/W design inorder to
 * get correct RDR value.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- -------------------------------------------------------
 * 3.00a adk 08/10/2013 initial release CR:727787
 *
 * </pre>
 *
 * ***************************************************************************
 */

/***************************** Include Files *********************************/
#include "xparameters.h"
#include "xil_exception.h"
#include "xstreamer.h"
#include "xil_cache.h"
#include "xllfifo.h"
#include "xstatus.h"

#ifdef XPAR_UARTNS550_0_BASEADDR
#include "xuartns550_l.h"       /* to use uartns550 */
#endif

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

#define FIFO_DEV_ID	   	XPAR_AXI_FIFO_MM_S_0_DEVICE_ID

#define WORD_SIZE 4			/* Size of words in bytes */

#define MAX_PACKET_LEN 4

#define NO_OF_PACKETS 64

#define MAX_DATA_BUFFER_SIZE NO_OF_PACKETS*MAX_PACKET_LEN

#undef DEBUG

/************************** Function Prototypes ******************************/
#ifdef XPAR_UARTNS550_0_BASEADDR
static void Uart550_Setup(void);
#endif

int XLlFifoPollingExample(XLlFifo *InstancePtr, u16 DeviceId);
int TxSend(XLlFifo *InstancePtr, u32 *SourceAddr, int packetBytes);
int RxReceive(XLlFifo *InstancePtr, u32 *DestinationAddr);
int RunTestCase(XLlFifo *InstancePtr, u32* SourceAddr, u32* DestinationAddr, int sendSize, int testCaseNum);

/************************** Variable Definitions *****************************/
/*
 * Device instance definitions
 */
XLlFifo FifoInstance;
u32 testCase1[] = { 0x1C000045, 0x00400000, 0x90C71101,
					0x9E01A8C0, 0xFAFFFFEF, 0x6C0722A1,
					0x69570800};
u32 testCase2[] = { 0x40020045, 0x00400000, 0x6CC51101,
					0x9E01A8C0, 0xFAFFFFEF, 0x6C0722A1,
					0x45552C02, 0x45532D4D, 0x48435241,
					0x48202A20, 0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220, 0x54534F48,
					0x3332203A, 0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30, 0x0A0D3030,
					0x3A4E414D, 0x73732220, 0x643A7064,
					0x6F637369, 0x22726576, 0x48435241,
					0x48202A20, 0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220, 0x54534F48,
					0x3332203A, 0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30, 0x0A0D3030,
					0x3A4E414D, 0x73732220, 0x643A7064,
					0x6F637369, 0x22726576, 0x48435241,
					0x48202A20, 0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220, 0x54534F48,
					0x3332203A, 0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30, 0x0A0D3030,
					0x3A4E414D, 0x73732220, 0x643A7064,
					0x6F637369, 0x22726576, 0x48435241,
					0x48202A20, 0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220, 0x54534F48,
					0x3332203A, 0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30, 0x0A0D3030,
					0x3A4E414D, 0x73732220, 0x643A7064,
					0x6F637369, 0x22726576, 0x48435241,
					0x48202A20, 0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220, 0x54534F48,
					0x3332203A, 0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30, 0x0A0D3030,
					0x3A4E414D, 0x73732220, 0x643A7064,
					0x6F637369, 0x22726576, 0x48435241,
					0x48202A20, 0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220, 0x54534F48,
					0x3332203A, 0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30, 0x0A0D3030,
					0x3A4E414D, 0x73732220, 0x643A7064,
					0x6F637369, 0x22726576, 0x48435241,
					0x48202A20, 0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220, 0x54534F48,
					0x3332203A, 0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30, 0x0A0D3030,
					0x3A4E414D, 0x73732220, 0x643A7064,
					0x6F637369, 0x22726576, 0x48435241,
					0x48202A20, 0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220, 0x54534F48,
					0x3332203A, 0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30, 0x0A0D3030,
					0x3A4E414D, 0x73732220};
u32 testCase3[] = { 0x40020045, 0x00400000,
					0x6CC61101, 0x9E01A8C0,
					0xFAFFFFEF, 0x6C0722A1,
					0x45552C02, 0x45532D4D,
					0x48435241, 0x48202A20,
					0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220,
					0x54534F48, 0x3332203A,
					0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30,
					0x0A0D3030, 0x3A4E414D,
					0x73732220, 0x643A7064,
					0x6F637369, 0x22726576,
					0x48435241, 0x48202A20,
					0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220,
					0x54534F48, 0x3332203A,
					0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30,
					0x0A0D3030, 0x3A4E414D,
					0x73732220, 0x643A7064,
					0x6F637369, 0x22726576,
					0x48435241, 0x48202A20,
					0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220,
					0x54534F48, 0x3332203A,
					0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30,
					0x0A0D3030, 0x3A4E414D,
					0x73732220, 0x643A7064,
					0x6F637369, 0x22726576,
					0x48435241, 0x48202A20,
					0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220,
					0x54534F48, 0x3332203A,
					0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30,
					0x0A0D3030, 0x3A4E414D,
					0x73732220, 0x643A7064,
					0x6F637369, 0x22726576,
					0x48435241, 0x48202A20,
					0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220,
					0x54534F48, 0x3332203A,
					0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30,
					0x0A0D3030, 0x3A4E414D,
					0x73732220, 0x643A7064,
					0x6F637369, 0x22726576,
					0x48435241, 0x48202A20,
					0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220,
					0x54534F48, 0x3332203A,
					0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30,
					0x0A0D3030, 0x3A4E414D,
					0x73732220, 0x643A7064,
					0x6F637369, 0x22726576,
					0x48435241, 0x48202A20,
					0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220,
					0x54534F48, 0x3332203A,
					0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30,
					0x0A0D3030, 0x3A4E414D,
					0x73732220, 0x643A7064,
					0x6F637369, 0x22726576,
					0x48435241, 0x48202A20,
					0x2F505454, 0x0D312E31,
					0x3A584D0A, 0x0A0D3220,
					0x54534F48, 0x3332203A,
					0x35322E39, 0x35322E35,
					0x35322E35, 0x39313A30,
					0x0A0D3030, 0x3A4E414D,
					0x73732220};

u32 testCase5[] = { 0x56020045,0x00400000,
					0x6AC51101,0x9E01A8C0,
					0xFAFFFFEF,0x6C0722A1,
					0x43552E02,0x45532D4D,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x00001322};
u32 testCase6[] = { 0x41020045,0x00400000,
					0x6BC51101,0x9E01A8C0,
					0xFAFFFFEF,0x6C0722A1,
					0x44552D02,0x45532D4D,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x00000022};
u32 testCase7[] = { 0x41020045,0x00400000,
					0x76C50601,0x9E01A8C0,
					0xFAFFFFEF,0x6C0722A1,
					0x44552D02,0x45532D4D,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220,0x643A7064,
					0x6F637369,0x22726576,
					0x48435241,0x48202A20,
					0x2F505454,0x0D312E31,
					0x3A584D0A,0x0A0D3220,
					0x54534F48,0x3332203A,
					0x35322E39,0x35322E35,
					0x35322E35,0x39313A30,
					0x0A0D3030,0x3A4E414D,
					0x73732220, 0x00000022};
u32 SourceBuffer[MAX_DATA_BUFFER_SIZE * WORD_SIZE];
u32 DestinationBuffer[16384];

/*****************************************************************************/
/**
*
* Main function
*
* This function is the main entry of the Axi FIFO Polling test.
*
* @param	None
*
* @return	- XST_SUCCESS if tests pass
* 		- XST_FAILURE if fails.
*
* @note		None
*
******************************************************************************/
int main()
{
	int Status;

	xil_printf("--- Entering main() ---\n\r");

	Status = XLlFifoPollingExample(&FifoInstance, FIFO_DEV_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Axi Streaming FIFO Polling Example TestFailed\n\r");
		xil_printf("--- Exiting main() ---\n\r");
		return XST_FAILURE;
	}

	xil_printf("Axi Streaming FIFO Polling Example Test passed\n\r");
	xil_printf("--- Exiting main() ---\n\r");

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function demonstrates the usage AXI FIFO
* It does the following:
*       - Set up the output terminal if UART16550 is in the hardware build
*       - Initialize the Axi FIFO Device.
*	- Transmit the data
*	- Receive the data from fifo
*	- Compare the data
*	- Return the result
*
* @param	InstancePtr is a pointer to the instance of the
*		XLlFifo component.
* @param	DeviceId is Device ID of the Axi Fifo Deive instance,
*		typically XPAR_<AXI_FIFO_instance>_DEVICE_ID value from
*		xparameters.h.
*
* @return	-XST_SUCCESS to indicate success
*		-XST_FAILURE to indicate failure
*
******************************************************************************/
int XLlFifoPollingExample(XLlFifo *InstancePtr, u16 DeviceId)
{
	XLlFifo_Config *Config;
	int Status;
	int tc1Status, tc2Status, tc3Status, tc4Status, tc5Status, tc6Status, tc7Status;
	int i;
	int Error;
	int numRx;
	Status = XST_SUCCESS;


	/* Initial setup for Uart16550 */
#ifdef XPAR_UARTNS550_0_BASEADDR

	Uart550_Setup();

#endif

	/* Initialize the Device Configuration Interface driver */
	Config = XLlFfio_LookupConfig(DeviceId);
	if (!Config) {
		xil_printf("No config found for %d\r\n", DeviceId);
		return XST_FAILURE;
	}

	/*
	 * This is where the virtual address would be used, this example
	 * uses physical address.
	 */
	Status = XLlFifo_CfgInitialize(InstancePtr, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("Initialization failed\n\r");
		return Status;
	}

	/* Check for the Reset value */
	Status = XLlFifo_Status(InstancePtr);
	XLlFifo_IntClear(InstancePtr,0xffffffff);
	Status = XLlFifo_Status(InstancePtr);
	if(Status != 0x0) {
		xil_printf("\n ERROR : Reset value of ISR0 : 0x%x\t"
			    "Expected : 0x0\n\r",
			    XLlFifo_Status(InstancePtr));
		return XST_FAILURE;
	}

	///////////////////// Test Case 1 (minimum packet length) ///////////////////////

	memset(DestinationBuffer, 0, sizeof(DestinationBuffer));
	tc1Status = RunTestCase(InstancePtr, testCase1, DestinationBuffer, 28, 1);

	///////////////////// Test Case 2 (minimum required length) /////////////////////

	memset(DestinationBuffer, 0, sizeof(DestinationBuffer));
	tc2Status = RunTestCase(InstancePtr, testCase2, DestinationBuffer, 576, 2);

	///////////////////// Test Case 3 (minimum required length bad chksum //////////

	memset(DestinationBuffer, 0, sizeof(DestinationBuffer));
	tc3Status = RunTestCase(InstancePtr, testCase3, DestinationBuffer, 576, 3);

	///////////////////// Test Case 4 (maximum length packet) //////////////////////

//	memset(DestinationBuffer, 0, sizeof(DestinationBuffer));
//	tc4Status = RunTestCase(InstancePtr, testCase4, DestinationBuffer, 65535, 4);

	///////////////////// Test Case 5 (common length even) /////////////////////////

	memset(DestinationBuffer, 0, sizeof(DestinationBuffer));
	tc5Status = RunTestCase(InstancePtr, testCase5, DestinationBuffer, 598, 5);

	///////////////////// Test Case 6 (common length odd) //////////////////////////

	memset(DestinationBuffer, 0, sizeof(DestinationBuffer));
	tc6Status = RunTestCase(InstancePtr, testCase6, DestinationBuffer, 597, 6);

	///////////////////// Test Case 7 (non udp) ////////////////////////////////////

	memset(DestinationBuffer, 0, sizeof(DestinationBuffer));
	tc7Status = RunTestCase(InstancePtr, testCase7, DestinationBuffer, 597, 7);

	xil_printf("Summary: \n\r\n\r");
	xil_printf("Test Case 1: %s\n\r", (tc1Status!=XST_SUCCESS)?"Failed":"Passed");
	xil_printf("Test Case 2: %s\n\r", (tc2Status!=XST_SUCCESS)?"Failed":"Passed");
	xil_printf("Test Case 3: %s\n\r", (tc3Status!=XST_SUCCESS)?"Failed":"Passed");
	//xil_printf("Test Case 4: %s\n\r", (tc4Status!=XST_SUCCESS)?"Failed":"Passed");
	xil_printf("Test Case 5: %s\n\r", (tc5Status!=XST_SUCCESS)?"Failed":"Passed");
	xil_printf("Test Case 6: %s\n\r", (tc6Status!=XST_SUCCESS)?"Failed":"Passed");
	xil_printf("Test Case 7: %s\n\r", (tc7Status!=XST_SUCCESS)?"Failed":"Passed");


	return Status;
}

/*****************************************************************************/
/*
*
* TxSend routine, It will send the requested amount of data at the
* specified addr.
*
* @param	InstancePtr is a pointer to the instance of the
*		XLlFifo component.
*
* @param	SourceAddr is the address where the FIFO stars writing
*
* @return	-XST_SUCCESS to indicate success
*		-XST_FAILURE to indicate failure
*
* @note		None
*
******************************************************************************/
int TxSend(XLlFifo *InstancePtr, u32  *SourceAddr, int packetBytes)
{

	int j;
	xil_printf(" Transmitting Data ... \r\n");

	int i = 0;
	/* Writing into the FIFO Transmit Port Buffer */
	for (j=0 ; j < packetBytes ; j=j+4){
		if( XLlFifo_iTxVacancy(InstancePtr) ){
			XLlFifo_TxPutWord(InstancePtr,
				*(SourceAddr+i));
		}
		i++;
	}

	xil_printf(" number of bytes: %d ... \r\n", packetBytes);

	/* Start Transmission by writing transmission length into the TLR */
	XLlFifo_iTxSetLen(InstancePtr, packetBytes);

	/* Check for Transmission completion */
	while( !(XLlFifo_IsTxDone(InstancePtr)) ){
//		xil_printf(" Not Done ... \r\n");
	}

	/* Transmission Complete */
	return XST_SUCCESS;
}

/*****************************************************************************/
/*
*
* RxReceive routine.It will receive the data from the FIFO.
*
* @param	InstancePtr is a pointer to the instance of the
*		XLlFifo instance.
*
* @param	DestinationAddr is the address where to copy the received data.
*
* @return	-XST_SUCCESS to indicate success
*		-XST_FAILURE to indicate failure
*
* @note		None
*
******************************************************************************/
int RxReceive (XLlFifo *InstancePtr, u32* DestinationAddr)
{

	int i;
	int Status;
	u32 RxWord;
	static u32 ReceiveLength;

	xil_printf(" Receiving data ....\n\r");
	/* Read Recieve Length */
	ReceiveLength = (XLlFifo_iRxGetLen(InstancePtr));
	xil_printf(" Received length: %d", ReceiveLength);
	/* Start Receiving */
	int j = 0;
	for ( i=0; i < ReceiveLength; i=i+4){
		RxWord = 0;
		RxWord = XLlFifo_RxGetWord(InstancePtr);

		if(XLlFifo_iRxOccupancy(InstancePtr)){
			RxWord = XLlFifo_RxGetWord(InstancePtr);
		}
		*(DestinationAddr+j) = RxWord;
		j++;
	}



	Status = XLlFifo_IsRxDone(InstancePtr);
	if(Status != TRUE){
		xil_printf("Failing in receive complete ... \r\n");
		return 0;
	}

	return ReceiveLength;
}

int RunTestCase(XLlFifo *InstancePtr, u32* SourceAddr, u32* DestinationAddr, int sendSize, int testCaseNum){
	xil_printf("Test Case %d (non-udp)\n\r", testCaseNum);
	int Status = XST_SUCCESS;
	int numRx = 0;
	int i,j;
	u32 LSB;
	/* Transmit the Data Stream */
	Status = TxSend(InstancePtr, SourceAddr, sendSize);
	if (Status != XST_SUCCESS){
		xil_printf("Transmisson of Data failed\n\r");
		return XST_FAILURE;
	}

	/* Receive the Data Stream */
	numRx = RxReceive(InstancePtr, DestinationAddr);


	/* Compare the data send with the data received */
	xil_printf(" Received data ...\n\r");
	xil_printf(" Bytes received: %d ...\n\r", numRx);
	j=0;
	for( i=0 ; i<numRx ; i+=4 ){
		if(j%2 == 0){
			LSB = *(DestinationAddr + j);
			if(i==numRx-1){
				xil_printf("0x%08x%08x \n\r", 0, LSB);
			}
		} else {
			xil_printf("0x%08x%08x \n\r", *(DestinationAddr + j), LSB);
		}
		if(*(SourceAddr + j + 2) != *(DestinationAddr + j)){
			Status = XST_FAILURE;
			xil_printf("Data Mismatch 0x%08x != 0x%08x \n\r", *(SourceAddr + j + 2), *(DestinationAddr + j));
		}
		j++;
	}
	xil_printf("\n\r");

	return Status;
}

#ifdef XPAR_UARTNS550_0_BASEADDR
/*****************************************************************************/
/*
*
* Uart16550 setup routine, need to set baudrate to 9600 and data bits to 8
*
* @param	None
*
* @return	None
*
* @note		None
*
******************************************************************************/
static void Uart550_Setup(void)
{

	XUartNs550_SetBaud(XPAR_UARTNS550_0_BASEADDR,
			XPAR_XUARTNS550_CLOCK_HZ, 9600);

	XUartNs550_SetLineControlReg(XPAR_UARTNS550_0_BASEADDR,
			XUN_LCR_8_DATA_BITS);
}
#endif
