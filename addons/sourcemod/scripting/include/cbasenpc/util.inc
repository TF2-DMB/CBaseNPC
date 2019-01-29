stock float fmodf(float num, float denom)
{
	return num - denom * RoundToFloor(num / denom);
}

// Available here https://github.com/alliedmodders/hl2sdk/blob/0ef5d3d482157bc0bb3aafd37c08961373f87bfd/mathlib/mathlib_base.cpp#L3516
stock float UTIL_AngleNormalize(float angle)
{
	angle = fmodf(angle, 360.0);
	if (angle > 180.0) angle -= 360.0;
	if (angle < -180.0) angle += 360.0;
	return angle;
}

// Available here https://github.com/alliedmodders/hl2sdk/blob/0ef5d3d482157bc0bb3aafd37c08961373f87bfd/mathlib/mathlib_base.cpp#L3483
stock float UTIL_AngleDiff(float firstAngle, float secondAngle)
{
	float diff = fmodf(firstAngle - secondAngle, 360.0);
	if ( firstAngle > secondAngle )
	{
		if ( diff >= 180 )
			diff -= 360;
	}
	else
	{
		if ( diff <= -180 )
			diff += 360;
	}
	return diff;
}

// Available here https://github.com/alliedmodders/hl2sdk/blob/0ef5d3d482157bc0bb3aafd37c08961373f87bfd/mathlib/mathlib_base.cpp#L3456
stock float UTIL_ApproachAngle( float target, float value, float speed )
{
	float delta = UTIL_AngleDiff(target, value);

	// Speed is assumed to be positive
	if ( speed < 0 )
		speed = -speed;

	if ( delta < -180 )
		delta += 360;
	else if ( delta > 180 )
		delta -= 360;

	if ( delta > speed )
		value += speed;
	else if ( delta < -speed )
		value -= speed;
	else 
		value = target;

	return value;
}

stock float UTIL_Clamp(float f1, float f2, float f3)
{
	return (f1 > f3 ? f3 : (f1 < f2 ? f2 : f1));
}