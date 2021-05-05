/**********************************************************************
 *
 * ibitmap.h
 *
 * the definition of the bitmap, by linwei Feb.6 2002
 *
 **********************************************************************/

#ifndef __IBITMAP_H__
#define __IBITMAP_H__


/**********************************************************************
 * Definition of struct IBITMAP
 **********************************************************************/

struct IBITMAP
{
	unsigned long bmWidth;		/* Width of the bitmap */
	unsigned long bmHeight;		/* Height of the bitmap */
	unsigned long bmDepth;		/* Color depth of bitmap */
	unsigned long bmPitch;		/* Pitch, same as linear size */
	unsigned long bmCode;		/* Code for mode */
	unsigned long bmInfo1;		/* additional data 1 */
	unsigned long bmInfo2;		/* additional data 2 */
	void *bmPixel;				/* pixels data in bitmap */
	void *bmData;				/* additional pointer */
	void**bmLines;				/* pointer to each line in bitmap */
};

#endif

