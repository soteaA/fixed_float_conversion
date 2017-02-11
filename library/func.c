#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef union UnFix32_ {
	signed int iNum;
	struct StFix32 {
		unsigned int fraction: 23;
		unsigned int decimal :  8;
		unsigned int sign    :  1;
	} stFix32;
} UnFix32;

typedef union UnFix16_ {
	signed int iNum;
	struct StFix16 {
		unsigned int fraction: 12;
		unsigned int decimal :  3;
		unsigned int sign    :  1;
	} stFix16;
} UnFix16;

typedef union UnFloat_ {
	signed int iNum;
	struct StFloat {
		unsigned int fraction: 23;
		unsigned int exponent:  8;
		unsigned int sign    :  1;
	} stFloat;
} UnFloat;

UnFix32 float2fix32(float fL);
UnFix16 float2fix16(float fL);
float fix2float32(UnFix32 uF);
float fix2float16(UnFix16 uF);


int main(int argc, char* argv[])
{
/*
 * argv[1]: input value to be converted either to float or to fixed-point
 * argv[2]: how many bits? 32 or 16
 * argv[3]: to what data-type do you wish to convert? fl or fixed
 */
	if (argc != 4) {
		printf ("Improper argument numbers\r\n");
		exit(EXIT_FAILURE);
	}

	if (!strcmp(argv[3],"fixed")) {
		switch (atoi(argv[2])) {
			case 32:
				printf("float32 to fixed32 conversion initiated..\r\n");
				UnFix32 fix32;
				float fl32;
				fix32 = float2fix32(atof(argv[1]));
				printf("fixed-point %f => %x\r\n", atof(argv[1]), fix32.iNum);
				printf("fixed-point %f => %d\r\n", atof(argv[1]), fix32.iNum);
				fl32  = fix2float32(fix32);
				printf("Compare %f => %f\r\n", atof(argv[1]), fl32);
				break;
			case 16:
				printf("float16 to fixed16 conversion initiated..\r\n");
				UnFix16 fix16;
				float fl16;
				fix16 = float2fix16(atof(argv[1]));
				printf("fixed-point %f => %x\r\n", atof(argv[1]), fix16.iNum);
				printf("fixed-point %f => %d\r\n", atof(argv[1]), fix16.iNum);
				fl16  = fix2float16(fix16);
				printf("Compare %f => %f\r\n", atof(argv[1]), fl16);
				break;
			default:
				printf("argv[2] is an unacceptable number.\r\n");
				printf("Here is the potential options: 32 or 16\r\n");
				break;
		}
	} else if (!strcmp(argv[3],"fl")) {
		switch (atoi(argv[2])) {
			case 32:
				printf("fixed32 to float32 conversion initiated..\r\n");
				UnFix32 fix32;
				float fl32;
				fl32 = fix2float32((UnFix32)atoi(argv[1]));
				printf("float %d => %f\r\n", atoi(argv[1]), fl32);
				printf("float %x => %f\r\n", atoi(argv[1]), fl32);
				fix32 = float2fix32(fl32);
				printf("Compare %x => %x\r\n", atoi(argv[1]), fix32.iNum);
				break;
			case 16:
				printf("fixed16 to float16 conversion initiated..\r\n");
				UnFix16 fix16;
				float fl16;
				fl16 = fix2float16((UnFix16)atoi(argv[1]));
				printf("fixed-point %d => %f\r\n", atoi(argv[1]), fl16);
				printf("fixed-point %x => %f\r\n", atoi(argv[1]), fl16);
				fix16  = float2fix16(fl16);
				printf("Compare %x => %x\r\n", atoi(argv[1]), fix16.iNum);
				break;
			default:
				printf("argv[2] is an unacceptable number.\r\n");
				printf("Here is the potential options: 32 or 16\r\n");
				break;
		}
	} else {
		printf("Wrong. Your options are: fl or fixed\r\n");
	}

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
		uF.stFix32.fraction |= uFl->iNum;
		uF.iNum = (~uF.iNum + 1);
	} else {
		uF.stFix32.decimal = (unsigned int)fL;
		fL = (fL - (unsigned int)fL + 1.0);
		uF.stFix32.fraction |= uFl->iNum;
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

float fix2float32(UnFix32 uF) {
	float fL;
	UnFloat *uFl = (UnFloat*) &fL;

	uFl->iNum = 0;
	uFl->stFloat.exponent = 127;

	if (uF.stFix32.sign) {
		uF.iNum = ~(uF.iNum - 1);
		uFl->stFloat.fraction |= uF.stFix32.fraction;
		fL = fL + (float)uF.stFix32.decimal - 1.0;
		uFl->stFloat.sign = 1;
	} else {
		uFl->stFloat.fraction |= uF.stFix32.fraction;
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
