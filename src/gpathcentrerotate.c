#include <pebble.h>
#include "gpathcentrerotate.h"

/* helper function to produce a properly rounded int from a float */
int16_t rounded_float(float f) {
	
	int16_t rounded_int = (int16_t)f;
	float rounding = f - rounded_int;
	
	if( rounding >= 0.50) {  						
		rounded_int ++;
	} else if( rounding <= (-0.50)) { 
		rounded_int --;
	}

	return rounded_int;
}

/* manual fix where the "math" doesn't produce a pleasing answer */
/* this is very hacky, but necessary to draw well */
/* NB: currently set for pivot axis {72,84} */
GPoint manual_fix_offset(GPoint pointIn, int deg) {
	
	int secs = deg / 6;
	
	int16_t xOut = pointIn.x;
	int16_t yOut = pointIn.y;
	
	switch(secs) {
		case 1:
			yOut++;
			break;
		case 2:
			yOut++;
			break;
		case 3:
			//xOut--;
			yOut++;
			break;
		case 4:
			xOut++;
			//yOut--;
		case 5:
			xOut--;
			yOut+=2;
			break;
		case 6:
			yOut++;
			break;
		case 7:
			yOut++;
			break;
		case 8:
			yOut++;
			break;
		case 9:
			xOut--;
			yOut++;
			break;
		case 10:
			xOut--;
			break;
		case 11:
			yOut++;
			break;
		case 12:
			xOut--;
			//yOut++;
			break;
		case 13:
			//xOut++;
			yOut++;
			break;
		case 14:
			yOut++;
			break;
		case 16:
			xOut--;
			break;
		case 17:
			xOut--;
			break;
		case 18:
			xOut--;
			//yOut--;
			break;
		case 19:
			xOut--;
			yOut++;
			break;
		case 20:
			xOut-=2;
			yOut--;
			break;
		case 22:
			xOut--;
			break;
		case 23:
			xOut--;
			break;
		case 21:
			xOut--;
			break;
		case 24:
			xOut--;
			yOut--;
			break;
		case 25:
			yOut--;
			break;
		case 26:
			xOut--;
			break;
		case 27:
			//xOut --;
			yOut --;
			break;
		case 28:
			//xOut--;
			yOut++;
			break;
		case 29:
			//xOut--;
			break;
		case 31:
			yOut--;
			break;
		case 32:
			yOut--;
			break;
		case 33:
			xOut++;
			yOut--;
			break;
		case 34:
			xOut--;
			yOut--;
			break;
		case 35:
			xOut++;
			yOut-=2;
			break;
		case 36:
			yOut--;
			break;
		case 37:
			yOut--;
			break;
		case 38:
			yOut --;
			break;
		case 39:
			xOut++;
			yOut--;
			break;
		case 40:
			xOut++;
			break;
		case 41:
			yOut--;
			break;
		case 42:
			xOut++;
			//yOut--;
			break;
		case 43:
			//xOut--;
			//yOut--;
			break;
		case 44:
			//yOut--;
			break;
		case 46:
			xOut++;
			break;
		case 47:
			xOut++;
			break;
		case 48:
			xOut++;
			yOut++;
			break;
		case 49:
			xOut++;
			yOut--;
			break;
		case 50:
			xOut+=2;
			yOut++;
			break;
		case 51:
			xOut++;
			break;
		case 52:
			xOut++;
			break;
		case 53:
			xOut++;
			break;
		case 54:
			xOut++;
			yOut++;
			break;
		case 55:
			yOut++;
			break;
		case 56:
			xOut++;
			break;
		case 57:
			//xOut++;
			yOut++;
			break;
		case 58:
			xOut++;
			yOut--;
			break;
		case 59:
			xOut++;
			break;
		default:
			break;
	}
	
	GPoint pointOut = (GPoint) { xOut, yOut };
	return pointOut;
	
}


/* main function, only public function */
void gpath_centre_rotate(GPath *path, int deg, GPoint point) {
	
	// set the rotation
	int32_t angle = deg * TRIG_MAX_ANGLE / 360;
	gpath_rotate_to(path, angle);
	
	// calculate the translated point in floats
	int16_t xStart = point.x;
	int16_t yStart = point.y;
	
	float x = xStart;
	float y = yStart;
	float cosTheta = cos_lookup(angle);
	float sinTheta = sin_lookup(angle);
	float max = TRIG_MAX_RATIO;
	
	
	float xTransFloat = ( x * cosTheta - y * sinTheta ) / max;
	float yTransFloat = ( x * sinTheta + y * cosTheta ) / max;
	int16_t xTrans = rounded_float(xTransFloat);
	int16_t yTrans = rounded_float(yTransFloat);

	
	// calculate the offset
	int16_t xOff = xStart - xTrans;
	int16_t yOff = yStart - yTrans;
	
	// set the movement
	GPoint transStart = (GPoint) {xOff, yOff};
	GPoint trans = manual_fix_offset(transStart, deg);
	gpath_move_to(path, trans);

}