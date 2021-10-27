#ifndef NATIVES_UTIL_H_INCLUDED_
#define NATIVES_UTIL_H_INCLUDED_

#include "helpers.h"

cell_t Util_ConcatTransforms(IPluginContext* pContext, const cell_t* params)
{
	cell_t * inMat1;
	cell_t * inMat2;
	cell_t * outMat;
	pContext->LocalToPhysAddr(params[1], &inMat1);
	pContext->LocalToPhysAddr(params[2], &inMat2);
	pContext->LocalToPhysAddr(params[3], &outMat);

	matrix3x4_t in1;
	matrix3x4_t in2;
	matrix3x4_t out;

	PawnMatrixToMatrix(pContext, inMat1, in1);
	PawnMatrixToMatrix(pContext, inMat2, in2);
	ConcatTransforms(in1, in2, out);
	MatrixToPawnMatrix(pContext, outMat, out);

	return 0;
}

#endif