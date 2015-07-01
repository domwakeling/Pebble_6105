#pragma once

/*	GPATH CENTRE ROTATE
 *	
 *	This function provides the ability to specify a gpath and a point of rotation, and will
 *	carry out a combined gpath_rotate_to and gpath_move_to such that the gpath is rotated to
 *	the specified angle of rotation (in degrees) about the nominated rotation point
 */
	
void gpath_centre_rotate(GPath *path, int deg, GPoint point);