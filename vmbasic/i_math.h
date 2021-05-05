/*======================================================================
//
// i_math.h
//
// NOTE: Written by Linwei 
//
//======================================================================*/


#ifndef __I_MATH_H__
#define __I_MATH_H__


typedef struct iMATRIX	
{
	union {
		struct {
		float	_11, _12, _13, _14;
		float	_21, _22, _23, _24;
		float	_31, _32, _33, _34;
		float	_41, _42, _43, _44;
        };
        float v[4][4];
    };
}	iMatrix;

#include <math.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define i_dot_product(x,y) ((x[0]*y[0])+(x[1]*y[1])+(x[2]*y[2]))
#define i_vect_length(x)   ((float)sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]))

void i_normalize_vector(float *vect3);
void i_cross_product(const float *x,const float *y,float *z);
void i_vector_product(const float *v1,const iMatrix *m,float *v2);
void i_matrix_add(const iMatrix *m1,const iMatrix *m2,iMatrix *out);
void i_matrix_sub(const iMatrix *m1,const iMatrix *m2,iMatrix *out);
void i_matrix_mul(const iMatrix *m1,const iMatrix *m2,iMatrix *out);
void i_matrix_identity(iMatrix *out);
void i_translation_matrix(iMatrix *m,const float *pos);
void i_scaling_matrix(iMatrix *m,const float *pos);
void i_vectrotation_matrix(iMatrix *m,const float *v,const float a);

/* in the result: Z Axis is point into screen Y Axis is point up X Axis is point right */
iMatrix* i_lookat(iMatrix *pOut,const float *pEye,const float *pAt,const float *pUp);

#ifdef  __cplusplus
}
#endif

#endif

