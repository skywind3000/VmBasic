#include "i_math.h"

void i_normalize_vector(float *vect3)
{
	float length=i_vect_length(vect3);
	length=(length==0)?1:(1/length);
	vect3[0]*=length;
	vect3[1]*=length;
	vect3[2]*=length;
}

void i_cross_product(const float *x,const float *y,float *z)
{
	float t1,t2,t3;
	t1=x[1]*y[2]-x[2]*y[1];
	t2=x[2]*y[0]-x[0]*y[2];
	t3=x[0]*y[1]-x[1]*y[0];
	z[0]=t1;
	z[1]=t2;
	z[2]=t3;
}

static iMatrix IdentityMatrix =
{
	1.0,	0,		0,		0 ,
	0,		1.0,	0,		0 ,
	0,		0,		1.0,	0 ,
	0,		0,		0,		1.0 ,
};

void i_vector_product(const float *v1,const iMatrix *m,float *v2)
{
	float vect[3]={ v1[0], v1[1], v1[2] };
	v2[0] = vect[0] * m->_11 + vect[1] * m->_21 + vect[2] * m->_31 + m->_41;
	v2[1] = vect[0] * m->_12 + vect[1] * m->_22 + vect[2] * m->_32 + m->_42;
	v2[2] = vect[0] * m->_13 + vect[1] * m->_23 + vect[2] * m->_33 + m->_43;
}

void i_matrix_add(const iMatrix *m1,const iMatrix *m2,iMatrix *out)
{
	out->_11=m1->_11+m2->_11; out->_12=m1->_12+m2->_12;
	out->_13=m1->_13+m2->_13; out->_14=m1->_14+m2->_14;
	out->_21=m1->_21+m2->_21; out->_22=m1->_22+m2->_22;
	out->_23=m1->_23+m2->_23; out->_24=m1->_24+m2->_24;
	out->_31=m1->_31+m2->_31; out->_32=m1->_32+m2->_32;
	out->_33=m1->_33+m2->_33; out->_34=m1->_34+m2->_34;
	out->_41=m1->_41+m2->_41; out->_42=m1->_42+m2->_42;
	out->_43=m1->_43+m2->_43; out->_44=m1->_44+m2->_44;
}

void i_matrix_sub(const iMatrix *m1,const iMatrix *m2,iMatrix *out)
{
	out->_11=m1->_11-m2->_11; out->_12=m1->_12-m2->_12;
	out->_13=m1->_13-m2->_13; out->_14=m1->_14-m2->_14;
	out->_21=m1->_21-m2->_21; out->_22=m1->_22-m2->_22;
	out->_23=m1->_23-m2->_23; out->_24=m1->_24-m2->_24;
	out->_31=m1->_31-m2->_31; out->_32=m1->_32-m2->_32;
	out->_33=m1->_33-m2->_33; out->_34=m1->_34-m2->_34;
	out->_41=m1->_41-m2->_41; out->_42=m1->_42-m2->_42;
	out->_43=m1->_43-m2->_43; out->_44=m1->_44-m2->_44;
}

void i_matrix_mul(const iMatrix *m1,const iMatrix *m2,iMatrix *out)
{
	iMatrix temp;
	int i,j;
	if (m1==out) { temp=*m1; m1=&temp; }
	else if (m2==out) { temp=*m2; m2=&temp; }
	for (i=0;i<4;i++) 
		for (j=0;j<4;j++) 
			out->v[j][i]=(m1->v[j][0]*m2->v[0][i])+
						 (m1->v[j][1]*m2->v[1][i])+
						 (m1->v[j][2]*m2->v[2][i])+
						 (m1->v[j][3]*m2->v[3][i]);
}

void i_translation_matrix(iMatrix *m,const float *pos)
{
	*m=IdentityMatrix;
	m->_41=pos[0];
	m->_42=pos[1];
	m->_43=pos[2];
}

void i_scaling_matrix(iMatrix *m,const float *pos)
{
	*m = IdentityMatrix;
	m->_11 = pos[0];
	m->_22 = pos[1];
	m->_33 = pos[2];
}

void i_vectrotation_matrix(iMatrix *m,const float *vv,const float a)
{
	float vect[3]={ vv[0],vv[1],vv[2] };
	float w,x,y,z;		// quaternion
	float qsin = (float)sin(a/2.0);
	float qcos = (float)cos(a/2.0);
	i_normalize_vector(vect);
	w = qcos;			// x*x+y*y+z*z+w*w=1
	x = vect[0] * qsin;
	y = vect[1] * qsin;
	z = vect[2] * qsin; 
	m->_11 = 1 - 2 * y * y - 2 * z * z;
	m->_21 = 2 * x * y - 2 * w * z;
	m->_31 = 2 * x * z + 2 * w * y;
	m->_12 = 2 * x * y + 2 * w * z;
	m->_22 = 1 - 2 * x * x - 2 * z * z;
	m->_32 = 2 * y * z - 2 * w * x;
	m->_13 = 2 * x * z - 2 * w * y;
	m->_23 = 2 * y * z + 2 * w * x;
	m->_33 = 1 - 2 * x * x - 2 * y * y;
	m->_14 = m->_24 = m->_34 = 0;
	m->_41 = m->_42 = m->_43 = 0;	
	m->_44 = 1;
}

void i_matrix_identity(iMatrix *out)
{
	if (out) *out = IdentityMatrix;
}

iMatrix* i_lookat(iMatrix *pOut,const float *pEye,const float *pAt,const float *pUp)
{
	float XAxis[3];
	float YAxis[3];
	float ZAxis[3];

	ZAxis[0]=pAt[0]-pEye[0];	
	ZAxis[1]=pAt[1]-pEye[1];
	ZAxis[2]=pAt[2]-pEye[2];

	i_normalize_vector(ZAxis);

	i_cross_product(pUp,ZAxis,XAxis);
	i_normalize_vector(XAxis);
	i_cross_product(ZAxis,XAxis,YAxis);

    pOut->_11 = XAxis[0];
    pOut->_21 = XAxis[1];
    pOut->_31 = XAxis[2];
	pOut->_41 = -(XAxis[0]*pEye[0]+XAxis[1]*pEye[1]+XAxis[2]*pEye[2]);
    pOut->_12 = YAxis[0];
    pOut->_22 = YAxis[1];
    pOut->_32 = YAxis[2];
	pOut->_42 = -(YAxis[0]*pEye[0]+YAxis[1]*pEye[1]+YAxis[2]*pEye[2]);
    pOut->_13 = ZAxis[0];
    pOut->_23 = ZAxis[1];
    pOut->_33 = ZAxis[2];
	pOut->_43 = -(ZAxis[0]*pEye[0]+ZAxis[1]*pEye[1]+ZAxis[2]*pEye[2]);
    pOut->_14 = 0.0f;
    pOut->_24 = 0.0f;
    pOut->_34 = 0.0f;
    pOut->_44 = 1.0f;

    return pOut;
}
