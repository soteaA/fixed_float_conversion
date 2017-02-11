/* Header Files */
#include "stdio.h"
#include "stdlib.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "xaxicdma.h"
#include "xdevcfg.h"
#include "xil_cache.h"
#include "math.h"
#include "xtime_l.h"


/*
 * For Neural Network BRAM Setup
 *
 * The below addresses will be seen from AXI CDMA
 * 	1. FEATURE_BRAM
 * 		0xC0000000 - 0xC003FFFF
 * 	2. WEIGHT_BRAM
 * 		0xC2000000 - 0xC200FFFF
 * 	3. INTER_RESULT_BRAM
 * 		0xC4000000 - 0xC4003FFF
 * 	4. ERROR_BRAM
 * 		0xC6000000 - 0xC6003FFF
 */
#define INPUT_LAYER 784
#define OUTPUT_LAYER 10

#define RANGE					0x00003FFF
#define FEATURE_BASEADDR		0xC0000000
#define WEIGHT_BASEADDR			0xC2000000
#define INTER_RESULT_BASEADDR	0xC4000000
#define ERROR_BASEADDR			0xC6000000

#define CONTROLLER_BASEADDR		0x43C00000
#define CONTROLLER_HIGHADDR		0x43C0FFFF
#define CTRL_INTR_BASEADDR		0x43C10000
#define CTRL_INTR_HIGHADDR		0x43C1FFFF

/*
 *** For Reading Feature, Weight, and Target data from CSV files
 *
 * The data format follows suit of TensorFlow
 *
 * ### All the Definition ###
 * 1. The number of elements of a feature vector
 * 		FEATURE_SIZE:	784
 * 2. The number of feature vectors for learning
 * 		LEARN_DATANUM:	60,000
 * 3. The number of elements of a weight vector
 * 		WEIGHT_SIZE:	10
 */
#define FEATURE_SIZE 784
#define LEARN_DATANUM 60000
#define WEIGHT_SIZE 10

/* AXI DMA parameter definition here */
XAxiCdma *AxiCdmaInstancePtr;
XAxiCdma_Config *AxiCdmaCfg;
XAxiCdma_Bd *AxiCdma_Bd4feature, *AxiCdma_Bd4weight, *AxiCdma_Bd4target;
volatile unsigned int* axi_cdma = XPAR_AXI_CDMA_0_BASEADDR;
volatile unsigned int* controller = CONTROLLER_BASEADDR;


/*
 * fixed-point 32bit
 */
typedef union UnFix32_ {
	signed int iNum;
	struct StFix32 {
		unsigned int fraction: 16;
		unsigned int decimal : 15;
		unsigned int sign    : 1;
	} stFix32;
} UnFix32;

/*
 * fixed-point 16bit
 */
typedef union UnFix16_ {
	signed short iNum;
	struct StFix16 {
		unsigned short fraction: 12;
		unsigned short decimal : 3;
		unsigned short sign    : 1;
	} stFix16;
} UnFix16;

/*
 * fixed-point 10bit
 */
typedef union UnFix10_ {
	signed short iNum;
	struct StFix10 {
		unsigned short fraction: 6;
		unsigned short decimal : 3;
		unsigned short sign    : 1;
		unsigned short padding : 6;
	} stFix10;
} UnFix10;

/*
 * fixed-point 8bit
 */
typedef union UnFix8_ {
	signed char iNum;
	struct StFix8 {
		unsigned char fraction: 6;
		unsigned char decimal : 1;
		unsigned char sign    : 1;
	} stFix8;
} UnFix8;

typedef union UnFloat_ {
	signed int iNum;
	struct StFloat {
		unsigned int fraction: 23;
		unsigned int exponent:  8;
		unsigned int sign    :  1;
	} stFloat;
} UnFloat;

/* Prototype Declaration */
/*
 * Conversion Function
 *
 * Convert Floating-point to Fixed-point Data
 *  -float2fix32
 *  -float2fix16
 *  -float2fix10
 *  -float2fix8
 *
 * Convert Fixed-point to Floating-point Data
 *  -fix2float32
 *  -fix2float16
 *  -fix2float10
 *  -fix2float8
 */
UnFix32 float2fix32(float fL);
UnFix16 float2fix16(float fL);
UnFix10 float2fix10(float fL);
UnFix8 float2fix8(float fL);
float fix2float32(UnFix32 uF);
float fix2float16(UnFix16 uF);
float fix2float10(UnFix10 uF);
float fix2float8(UnFix8 uF);
float sigmoid(float fL);

typedef struct FEATURE {
	UnFix16 in3;
	UnFix16 in2;
	UnFix16 in1;
	UnFix16 in0;
} F;

typedef struct WEIGHT {
	UnFix16 in3;
	UnFix16 in2;
	UnFix16 in1;
	UnFix16 in0;
} W;

typedef struct TARGET {
	UnFix16 in3;
	UnFix16 in2;
	UnFix16 in1;
	UnFix16 in0;
} T;

typedef struct RESULT {
	UnFix16 out3;
	UnFix16 out2;
	UnFix16 out1;
	UnFix16 out0;
} R;

int main()
{
	/*** Infrastructure Variables ***/
	int i, j, Status;

	/*** Hardware Timer ***/
	XTime tStart, tEnd;
	float cps = (float)COUNTS_PER_SECOND/1000000.0;

	Xil_DCacheDisable();

	printf("sizeof(XAxiCdma_Bd): %d\r\n", (int)sizeof(XAxiCdma_Bd));

	/*
	 * AXI CDMA Initialization
	 */
	AxiCdmaCfg = XAxiCdma_LookupConfig(XPAR_AXI_CDMA_0_DEVICE_ID);
	Status = XAxiCdma_CfgInitialize(AxiCdmaInstancePtr, AxiCdmaCfg, AxiCdmaCfg->BaseAddress);
	if (XST_SUCCESS != Status) xil_printf("AXICDMA Initialization Failed\n\r");

	/*************** Read MNIST learning dataset ***************/
	FILE *fp;
	float *feature;
	F *fixed_feature;

	/* Read Feature Vector Data */
	feature = malloc(sizeof(float)*LEARN_DATANUM*FEATURE_SIZE);
	printf("feature size: %d\r\n", sizeof(float)*LEARN_DATANUM*FEATURE_SIZE);
	fixed_feature = malloc(sizeof(F)*LEARN_DATANUM*FEATURE_SIZE/4);
	printf("fixed feature size: %d\r\n", sizeof(float)*LEARN_DATANUM*FEATURE_SIZE/4);

	if ((fp = fopen("training_feature.csv","r")) == NULL) {
		printf ("file open error\r\n");
		exit(EXIT_FAILURE);
	}

	for (i=0; i<LEARN_DATANUM; i++) {
		for (j=0; j<FEATURE_SIZE; j++) {
			if (fscanf(fp, "%f,", &feature[j+FEATURE_SIZE*i])!='\0');
		}
	}

	for (i=0; i<LEARN_DATANUM*FEATURE_SIZE/4; i++) {
		fixed_feature[i].in0 = float2fix16(feature[4*i]);
		fixed_feature[i].in1 = float2fix16(feature[4*i+1]);
		fixed_feature[i].in2 = float2fix16(feature[4*i+2]);
		fixed_feature[i].in3 = float2fix16(feature[4*i+3]);
		printf("feature: %f, %f, %f, %f\r\n", feature[4*i],feature[4*i+1],feature[4*i+2],feature[4*i+3]);
		printf("fixed_feature: %d, %d, %d, %d\r\n", fixed_feature[i].in0, fixed_feature[i].in1, fixed_feature[i].in2, fixed_feature[i].in3);
	}

	free(feature);
	fclose(fp);

	/***********************************************************/

	/* AXI CDMA Buffer Descriptor Setup */
	/*
	 * 後でこの箇所のSourceAddressとDestinationAddressをうまく変更する必要がある。
	 * 例えば、ミニバッチ学習を行うとき、データをDDRからBRAMにロードするためのBDを生成する必要があり、
	 * この際、ロードするデータのアドレスが毎回変わるので、そこの一般性を記述する必要がある。
	 */
	AxiCdma_Bd4feature = malloc(sizeof(XAxiCdma_Bd)*196*16);
	for (i=0; i<196*16; i++) {
		if (i != 196*96-1) XAxiCdma_BdSetNextPtr(&AxiCdma_Bd4feature[i], (UINTPTR)&AxiCdma_Bd4feature[i+1]);
		else XAxiCdma_BdSetNextPtr(&AxiCdma_Bd4feature[i], (UINTPTR)&AxiCdma_Bd4feature[0]);
		/*** Source Address in DDR external memory***/
		Status = XAxiCdma_BdSetSrcBufAddr(&AxiCdma_Bd4feature[i], (UINTPTR)&fixed_feature[i]);
		if (XST_SUCCESS != Status) xil_printf("AXICDMA Buffer Descriptor Failed\n\r");
		/*** Destination Address in on-chip BRAM ***/
		Status = XAxiCdma_BdSetDstBufAddr(&AxiCdma_Bd4feature[i], (UINTPTR)0xC0000000+0x8*i);
		if (XST_SUCCESS != Status) xil_printf("AXICDMA Buffer Descriptor Failed\n\r");
		/*** Bytes to transfer: 8byte = 64bit ***/
		Status = XAxiCdma_BdSetLength(&AxiCdma_Bd4feature[i], 8);
		if (XST_SUCCESS != Status) xil_printf("AXICDMA Buffer Descriptor Failed\n\r");
	}


	/*** HW offloading
	 *
	 * Data Transaction between DDR3 and on-chip BRAMs
	 * 	1. Feature BRAM
	 * 	2. Weight BRAM
	 * At this point, the hardware process can be triggered by asserting start_bit to HIGH(1)
	 *  3. Target BRAM
	 */
//	axi_cdma[(XAXICDMA_SRCADDR_OFFSET) >> 2] = f;
//	axi_cdma[(XAXICDMA_DSTADDR_OFFSET) >> 2] = FEATURE_BASEADDR;
//	axi_cdma[(XAXICDMA_BTT_OFFSET) >> 2] = sizeof(F)*4;
//	while (1) {
//		if ((axi_cdma[(XAXICDMA_SR_OFFSET) >> 2] & 0x2) != 0x0) break;
//	}
//
//	axi_cdma[(XAXICDMA_SRCADDR_OFFSET) >> 2] = w;
//	axi_cdma[(XAXICDMA_DSTADDR_OFFSET) >> 2] = WEIGHT_BASEADDR;
//	axi_cdma[(XAXICDMA_BTT_OFFSET) >> 2] = sizeof(W)*16;
//	while (1) {
//		if ((axi_cdma[(XAXICDMA_SR_OFFSET) >> 2] & 0x2) != 0x0) break;
//	}

	return 0;
}

/* Conversion Functions */
UnFix32 float2fix32(float fL) {
	UnFix32 uF;
	UnFloat *uFl = (UnFloat*) &fL;

	uF.iNum = 0;

	if (uFl->stFloat.sign) {
		uFl->stFloat.sign = 0;
		// convert float to integer, corresponding to decimal part
		uF.stFix32.decimal = (unsigned int) fL;
		fL = ( fL - (unsigned int)fL + 1.0);
		uF.stFix32.fraction |= uFl->iNum >> 7;
		uF.iNum = (~uF.iNum + 1);
	} else {
		uF.stFix32.decimal = (unsigned int)fL;
		fL = (fL - (unsigned int)fL + 1.0);
		uF.stFix32.fraction |= uFl->iNum >> 7;
	}
	return uF;
}

UnFix16 float2fix16(float fL) {
	UnFix16 uF;
	UnFloat *uFl = (UnFloat*) &fL;

	uF.iNum = 0;

	if (uFl->stFloat.sign) {
		uFl->stFloat.sign = 0;
		// convert float to integer, corresponding to decimal part
		uF.stFix16.decimal = (unsigned int) fL;
		fL = ( fL - (unsigned int)fL + 1.0);
		uF.stFix16.fraction |= uFl->iNum >> 11;
		uF.iNum = (~uF.iNum + 1);
	} else {
		uF.stFix16.decimal = (unsigned int)fL;
		fL = (fL - (unsigned int)fL + 1.0);
		uF.stFix16.fraction |= uFl->iNum >> 11;
	}
	return uF;
}

UnFix10 float2fix10(float fL) {
	UnFix10 uF;
	UnFloat *uFl = (UnFloat*) &fL;

	uF.iNum = 0;

	if (uFl->stFloat.sign) {
		uFl->stFloat.sign = 0;
		// convert float to integer, corresponding to decimal part
		uF.stFix10.decimal = (unsigned int) fL;
		fL = ( fL - (unsigned int)fL + 1.0);
		uF.stFix10.fraction |= uFl->iNum >> 17;
		uF.iNum = (~uF.iNum + 1);
	} else {
		uF.stFix10.decimal = (unsigned int)fL;
		fL = (fL - (unsigned int)fL + 1.0);
		uF.stFix10.fraction |= uFl->iNum >> 17;
	}
	uF.stFix10.padding = 0;
	return uF;
}

UnFix8 float2fix8(float fL) {
	UnFix8 uF;
	UnFloat *uFl = (UnFloat*) &fL;

	uF.iNum = 0;

	if (uFl->stFloat.sign) {
		uFl->stFloat.sign = 0;
		// convert float to integer, corresponding to decimal part
		uF.stFix8.decimal = (unsigned int) fL;
		fL = ( fL - (unsigned int)fL + 1.0);
		uF.stFix8.fraction |= uFl->iNum >> 17;
		uF.iNum = (~uF.iNum + 1);
	} else {
		uF.stFix8.decimal = (unsigned int)fL;
		fL = (fL - (unsigned int)fL + 1.0);
		uF.stFix8.fraction |= uFl->iNum >> 17;
	}
	return uF;
}

float fix2float32(UnFix32 uF) {
	float fL;
	UnFloat *uFl = (UnFloat*) &fL;

	uFl->iNum = 0;
	uFl->stFloat.exponent = 127;

	if (uF.stFix32.sign) {
		uF.iNum = ~(uF.iNum - 1);
		uFl->stFloat.fraction |= uF.stFix32.fraction << 7;
		fL = fL + (float)uF.stFix32.decimal - 1.0;
		uFl->stFloat.sign = 1;
	} else {
		uFl->stFloat.fraction |= uF.stFix32.fraction << 7;
		fL = fL + (float)uF.stFix32.decimal - 1.0;
	}

	return fL;
}

float fix2float16(UnFix16 uF) {
	float fL;
	UnFloat *uFl = (UnFloat*) &fL;

	uFl->iNum = 0;
	uFl->stFloat.exponent = 127;

	if (uF.stFix16.sign) {
		uF.iNum = ~(uF.iNum - 1);
		uFl->stFloat.fraction |= uF.stFix16.fraction << 11;
		fL = fL + (float)uF.stFix16.decimal - 1.0;
		uFl->stFloat.sign = 1;
	} else {
		uFl->stFloat.fraction |= uF.stFix16.fraction << 11;
		fL = fL + (float)uF.stFix16.decimal - 1.0;
	}

	return fL;
}

float fix2float10(UnFix10 uF) {
	float fL;
	UnFloat *uFl = (UnFloat*) &fL;

	uFl->iNum = 0;
	uFl->stFloat.exponent = 127;

	if (uF.stFix10.sign) {
		uF.iNum = ~(uF.iNum - 1);
		uFl->stFloat.fraction |= uF.stFix10.fraction << 17;
		fL = fL + (float)uF.stFix10.decimal - 1.0;
		uFl->stFloat.sign = 1;
	} else {
		uFl->stFloat.fraction |= uF.stFix10.fraction << 17;
		fL = fL + (float)uF.stFix10.decimal - 1.0;
	}

	return fL;
}

float fix2float8(UnFix8 uF) {
	float fL;
	UnFloat *uFl = (UnFloat*) &fL;

	uFl->iNum = 0;
	uFl->stFloat.exponent = 127;

	if (uF.stFix8.sign) {
		uF.iNum = ~(uF.iNum - 1);
		uFl->stFloat.fraction |= uF.stFix8.fraction << 17;
		fL = fL + (float)uF.stFix8.decimal - 1.0;
		uFl->stFloat.sign = 1;
	} else {
		uFl->stFloat.fraction |= uF.stFix8.fraction << 17;
		fL = fL + (float)uF.stFix8.decimal - 1.0;
	}

	return fL;
}

float sigmoid(float fL) {
	return (float)1/(1+exp(-fL));
}


