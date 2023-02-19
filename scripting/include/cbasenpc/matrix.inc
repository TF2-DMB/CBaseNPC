#if defined _CBASENPC_MATRIX_INC_
 #endinput
#endif
#define _CBASENPC_MATRIX_INC_

#include "util.inc"

/**
 * Retrieves a row vector of the matrix.
 *
 * @param matrix     Input matrix
 * @param row
 * @param vec        Output vector
 */
stock void MatrixGetRowVector( const float matrix[3][4], int row, float vec[3] )
{
	vec[0] = matrix[row][0];
	vec[1] = matrix[row][1];
	vec[2] = matrix[row][2];
}

/**
 * Sets the row vector of the matrix to the given vector.
 *
 * @param vec     Input vector
 * @param row     
 * @param matrix  Matrix to modify
 */
stock void MatrixSetRowVector( const float vec[3], int row, float matrix[3][4] )
{
	matrix[row][0] = vec[0];
	matrix[row][1] = vec[1];
	matrix[row][2] = vec[2];
}

/**
 * Retrieves the column of the matrix.
 *
 * @param matrix     Input matrix
 * @param column
 * @param vec        Output vector
 */
stock void MatrixGetColumn( const float matrix[3][4], int column, float vec[3] )
{
	vec[0] = matrix[0][column];
	vec[1] = matrix[1][column];
	vec[2] = matrix[2][column];
}

/**
 * Sets the column of the matrix to the given vector.
 *
 * @param vec        Input vector
 * @param column     Column
 * @param matrix     Matrix whose column will be replaced
 */
stock void MatrixSetColumn( const float vec[3], int column, float matrix[3][4] )
{
	matrix[0][column] = vec[0];
	matrix[1][column] = vec[1];
	matrix[2][column] = vec[2];
}

/**
 * Initializes a 3x4 matrix using the given translation and basis vectors.
 *
 * @param matOut      Output matrix
 * @param vecOrigin   Translation
 * @param vecXAxis    X-axis basis vector
 * @param vecYAxis    Y-axis basis vector
 * @param vecZAxis    Z-axis basis vector
 */
stock void MatrixInitialize( float matOut[3][4], const float vecOrigin[3], const float vecXAxis[3], const float vecYAxis[3], const float vecZAxis[3] )
{
	MatrixSetColumn( vecXAxis, 0, matOut );
	MatrixSetColumn( vecYAxis, 1, matOut );
	MatrixSetColumn( vecZAxis, 2, matOut );
	MatrixSetColumn( vecOrigin, 3, matOut );
}

/**
 * Sets the given matrix to the identity.
 *
 * @param matrix      Input matrix
 */
stock void SetIdentityMatrix( float matrix[3][4] )
{
	for ( int r = 0; r < 3; r++ )
	{
		for ( int c = 0; c < 4; c++ )
		{
			matrix[r][c] = 0.0;
		}
	}

	matrix[0][0] = 1.0;
	matrix[1][1] = 1.0;
	matrix[2][2] = 1.0;
}

/**
 * Copies contents of the input matrix into the output matrix.
 *
 * @param inMat      Input matrix
 * @param outMat     Output matrix
 */
stock void MatrixCopy( const float inMat[3][4], float outMat[3][4] )
{
	for ( int r = 0; r < 3; r++ )
	{
		for ( int c = 0; c < 4; c++ )
		{
			outMat[r][c] = inMat[r][c];
		}
	}
}

/**
 * Scales each component of the first three columns of the matrix by an
 * arbitrary amount.
 *
 * @param flScale     Scale amount
 * @param outMat      Output matrix
 */
stock void MatrixScaleBy( float flScale, float outMat[3][4] )
{
	outMat[0][0] *= flScale;
	outMat[1][0] *= flScale;
	outMat[2][0] *= flScale;
	outMat[0][1] *= flScale;
	outMat[1][1] *= flScale;
	outMat[2][1] *= flScale;
	outMat[0][2] *= flScale;
	outMat[1][2] *= flScale;
	outMat[2][2] *= flScale;
}

/**
 * Sets each component of the first three columns of the matrix to zero.
 *
 * @param outMat      Output matrix
 */
stock void MatrixScaleByZero( float outMat[3][4] )
{
	outMat[0][0] = 0.0;
	outMat[1][0] = 0.0;
	outMat[2][0] = 0.0;
	outMat[0][1] = 0.0;
	outMat[1][1] = 0.0;
	outMat[2][1] = 0.0;
	outMat[0][2] = 0.0;
	outMat[1][2] = 0.0;
	outMat[2][2] = 0.0;
}

/**
 * Initializes a matrix using the given vector.
 *
 * Reference: https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/mathlib/mathlib_base.cpp#L523
 *
 * @param fwd      Input vector
 * @param matrix   Output matrix
 */
stock void VectorMatrix( const float fwd[3], float matrix[3][4] )
{
	float left[3], up[3];
	GetVectorVectors( fwd, left, up );
	NegateVector( left );

	MatrixSetColumn( fwd, 0, matrix );
	MatrixSetColumn( left, 1, matrix );
	MatrixSetColumn( up, 2, matrix );
}

/**
 * Extracts the basis vectors of the given matrix.
 *
 * Reference: https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/mathlib/mathlib_base.cpp#L489
 *
 * @param matrix   Input matrix
 * @param fwd      Output forward vector
 * @param right    Output right vector
 * @param up       Output up vector
 */
stock void MatrixVectors( const float matrix[3][4], float fwd[3] = NULL_VECTOR, float right[3] = NULL_VECTOR, float up[3] = NULL_VECTOR )
{
	if ( !IsNullVector( fwd ) ) MatrixGetColumn( matrix, 0, fwd );
	if ( !IsNullVector( right ) )
	{
		MatrixGetColumn( matrix, 1, right );
		NegateVector( right );
	}
	if ( !IsNullVector( up ) ) MatrixGetColumn( matrix, 2, up );
}

/**
 * Extracts the position vector of the matrix.
 *
 * @param matrix   Input matrix
 * @param vec      Output vector
 */
stock void MatrixPosition( const float matrix[3][4], float vec[3] )
{
    MatrixGetColumn( matrix, 3, vec );
}

/**
 * Multiplies the first matrix with the second. It is safe to use either of the
 * input matrices as the output matrix.
 *
 * @param in1      First input matrix
 * @param in2      Second input matrix
 * @param out      Output matrix
 */
native void ConcatTransforms( const float in1[3][4], const float in2[3][4], float outMat[3][4] ); 

/**
 * Transform a set of angles in the space of parentMatrix to local space.
 * The input angles can also be the output.
 *
 * Reference: https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/mathlib/mathlib_base.cpp#L324
 *
 * @param angles            Input angles
 * @param parentMatrix      World transform matrix. This is usually filled in
 *                          with CBaseEntity::EntityToWorldTransform().
 * @param outAng            Output angles
 */
stock void TransformAnglesToLocalSpace( const float angles[3], const float parentMatrix[3][4], float outAng[3] )
{
	float angToWorld[3][4], worldToParent[3][4], localMatrix[3][4];
	MatrixInvert( parentMatrix, worldToParent );
	AngleMatrix( angles, angToWorld );
	ConcatTransforms( worldToParent, angToWorld, localMatrix );
	
	MatrixAngles( localMatrix, outAng );
}

/**
 * Transform a set of angles in the space of parentMatrix to world space.
 * The input angles can also be the output.
 *
 * Reference: https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/mathlib/mathlib_base.cpp#L337
 *
 * @param angles            Input angles
 * @param parentMatrix      World transform matrix. This is usually filled in
 *                          with CBaseEntity::EntityToWorldTransform().
 * @param outAng            Output angles
 */
stock void TransformAnglesToWorldSpace( const float angles[3], const float parentMatrix[3][4], float outAng[3] )
{
	float angToParent[3][4], angToWorld[3][4];
	AngleMatrix( angles, angToParent );
	ConcatTransforms( parentMatrix, angToParent, angToWorld );
	MatrixAngles( angToWorld, outAng );
}

/**
 * Extracts the rotation component of the given matrix.
 *
 * Reference: https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/mathlib/mathlib_base.cpp#L208
 *
 * @param matrix   Matrix to extract angles from
 * @param ang      Output angles
 * @param pos      Output translation
 */
stock void MatrixAngles( const float matrix[3][4], float ang[3], float pos[3] = NULL_VECTOR )
{
	float fwd[3], left[3];
	float up[3] = { 0.0, 0.0, 1.0 };
	MatrixGetColumn( matrix, 0, fwd );
	MatrixGetColumn( matrix, 1, left );
	up[2] = matrix[2][2];
	
	float xyDist = SquareRoot( fwd[0] * fwd[0] + fwd[1] * fwd[1] );

	if ( xyDist > 0.001 )
	{
		ang[1] = RadToDeg( ArcTangent2( fwd[1], fwd[0] ) );
		ang[0] = RadToDeg( ArcTangent2( -fwd[2], xyDist ) );
		ang[2] = RadToDeg( ArcTangent2( left[2], up[2] ) );
	}
	else
	{
		ang[1] = RadToDeg( ArcTangent2( -left[0], left[1] ) );
		ang[0] = RadToDeg( ArcTangent2( -fwd[2], xyDist ) );
		ang[2] = 0.0;
	}

	if ( !IsNullVector( pos ) )
	{
		MatrixGetColumn( matrix, 3, pos );
	}
}

/**
 * Creates a rotation matrix using the given angles.
 *
 * Reference: https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/mathlib/mathlib_base.cpp#L1194
 * 
 * @param angles     Angles
 * @param matrix     Output matrix
 */
stock void AngleMatrix( const float angles[3], float matrix[3][4] )
{
	float sr, sp, sy, cr, cp, cy;

	SinCos( DegToRad( angles[1] ), sy, cy );
	SinCos( DegToRad( angles[0] ), sp, cp );
	SinCos( DegToRad( angles[2] ), sr, cr );

	// matrix = (YAW * PITCH) * ROLL
	matrix[0][0] = cp*cy;
	matrix[1][0] = cp*sy;
	matrix[2][0] = -sp;

	float crcy = cr*cy;
	float crsy = cr*sy;
	float srcy = sr*cy;
	float srsy = sr*sy;
	matrix[0][1] = sp*srcy-crsy;
	matrix[1][1] = sp*srsy+crcy;
	matrix[2][1] = sr*cp;

	matrix[0][2] = (sp*crcy+srsy);
	matrix[1][2] = (sp*crsy-srcy);
	matrix[2][2] = cr*cp;

	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
}

/**
 * Rotates a vector using the given rotation matrix. The input vector can also
 * be the output vector.
 *
 * Reference: https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/mathlib/mathlib_base.cpp#L286
 *
 * @param vecIn    Input vector
 * @param matrix   Input rotation matrix
 * @param vecOut   Output vector
 */
stock void VectorRotate( const float vecIn[3], const float matrix[3][4], float vecOut[3] )
{
	float temp[3]; float row[3];
	temp[0] = vecIn[0];
	temp[1] = vecIn[1];
	temp[2] = vecIn[2];
	MatrixGetRowVector( matrix, 0, row );
	vecOut[0] = GetVectorDotProduct( temp, row );
	MatrixGetRowVector( matrix, 1, row );
	vecOut[1] = GetVectorDotProduct( temp, row );
	MatrixGetRowVector( matrix, 2, row );
	vecOut[2] = GetVectorDotProduct( temp, row );
}

/**
 * Builds a rotation matrix for a counterclockwise rotation about an arbitrary axis.
 *
 * Reference: https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/mathlib/mathlib_base.cpp#L1107
 *
 * @param vAxisOfRot       The axis to rotate around.
 * @param angleDegrees     Amount of rotation
 * @param dst              Output matrix
 */
stock void MatrixBuildRotationAboutAxis( const float vAxisOfRot[3], float angleDegrees, float dst[3][4] )
{
	float fSin; float fCos;
	SinCos( DegToRad( angleDegrees ), fSin, fCos );

	float axisXSquared = vAxisOfRot[0] * vAxisOfRot[0];
	float axisYSquared = vAxisOfRot[1] * vAxisOfRot[1];
	float axisZSquared = vAxisOfRot[2] * vAxisOfRot[2];

	// Column 0:
	dst[0][0] = axisXSquared + (1 - axisXSquared) * fCos;
	dst[1][0] = vAxisOfRot[0] * vAxisOfRot[1] * (1 - fCos) + vAxisOfRot[2] * fSin;
	dst[2][0] = vAxisOfRot[2] * vAxisOfRot[0] * (1 - fCos) - vAxisOfRot[1] * fSin;

	// Column 1:
	dst[0][1] = vAxisOfRot[0] * vAxisOfRot[1] * (1 - fCos) - vAxisOfRot[2] * fSin;
	dst[1][1] = axisYSquared + (1 - axisYSquared) * fCos;
	dst[2][1] = vAxisOfRot[1] * vAxisOfRot[2] * (1 - fCos) + vAxisOfRot[0] * fSin;

	// Column 2:
	dst[0][2] = vAxisOfRot[2] * vAxisOfRot[0] * (1 - fCos) + vAxisOfRot[1] * fSin;
	dst[1][2] = vAxisOfRot[1] * vAxisOfRot[2] * (1 - fCos) - vAxisOfRot[0] * fSin;
	dst[2][2] = axisZSquared + (1 - axisZSquared) * fCos;

	// Column 3:
	dst[0][3] = 0.0;
	dst[1][3] = 0.0;
	dst[2][3] = 0.0;
}

/**
 * Returns the transpose of the given orthogonal matrix. The input matrix can
 * be the output matrix.
 *
 * NOTE: This is not a general inverse function.
 *
 * @param matIn      Input matrix
 * @param matOut     Output matrix
 */
stock void MatrixInvert( const float matIn[3][4], float matOut[3][4] )
{
	float buffer[3][4];
	MatrixCopy( matIn, buffer );
	MatrixInvertEx( buffer, matOut );
}

/**
 * Returns the transpose of the given orthogonal matrix. The same as
 * MatrixInvert() except this does not copy the input matrix into a buffer.
 *
 * NOTE: This is not a general inverse function.
 *
 * Reference: https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/mathlib/mathlib_base.cpp#L380
 *
 * @param matIn      Input matrix
 * @param matOut     Output matrix
 */
stock void MatrixInvertEx( const float matIn[3][4], float matOut[3][4] )
{
	matOut[0][0] = matIn[0][0];
	matOut[0][1] = matIn[1][0];
	matOut[0][2] = matIn[2][0];

	matOut[1][0] = matIn[0][1];
	matOut[1][1] = matIn[1][1];
	matOut[1][2] = matIn[2][1];

	matOut[2][0] = matIn[0][2];
	matOut[2][1] = matIn[1][2];
	matOut[2][2] = matIn[2][2];

	// now fix up the translation to be in the other space
	float tmp[3];
	tmp[0] = matIn[0][3];
	tmp[1] = matIn[1][3];
	tmp[2] = matIn[2][3];

	float rowVec[3];
	MatrixGetRowVector( matOut, 0, rowVec );
	matOut[0][3] = -GetVectorDotProduct( tmp, rowVec );
	MatrixGetRowVector( matOut, 1, rowVec );
	matOut[1][3] = -GetVectorDotProduct( tmp, rowVec );
	MatrixGetRowVector( matOut, 2, rowVec );
	matOut[2][3] = -GetVectorDotProduct( tmp, rowVec );
}