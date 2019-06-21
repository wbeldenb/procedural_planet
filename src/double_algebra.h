#pragma once
#include "ttmath/ttmath.h"



#define EPSXS             0.001
#define EPS             0.00001
#define ABS(x) (x < 0 ? -(x) : (x))

#define PI 3.14159

#define DOT(v1,v2) (v1.x*v2.x + v1.y*v2.y+v1.z*v2.z)
#define CROSS(rez,v1,v2) \
	rez.x = v1.y*v2.z - v1.z*v2.y; \
	rez.y = v1.z*v2.x - v1.x*v2.z; \
	rez.z = v1.x*v2.y - v1.y*v2.x;

#define SUB(rez,v1,v2) \
	rez.x = v1.x - v2.x; \
	rez.y = v1.y - v2.y; \
	rez.z = v1.z - v2.z;


#define LENGTH(v) (sqrtf(v.x* v.x + v.y*v.y + v.z*v.z))

#define NORMALIZE(v) \
	v.l = LENGTH(v); \
	v.x = v.x / v.l; \
	v.y = v.y / v.l; \
	v.z = v.z / v.l;

typedef ttmath::Big<1, 2> bigFloat;


#define bfvec3 double_vec_
#define bfmat4 double_mat_

class double_vec_
{
public:
bigFloat x,y,z;
double_vec_()
	{
	x=y=z=0;	
	}
~double_vec_()
	{
	
	}
double_vec_(bigFloat x_, bigFloat y_, bigFloat z_) {
	x=x_;y=y_;z=z_;	
}

bigFloat dot_(double_vec_ *v) {
	bigFloat dat=v->x*x+v->y*y+v->z*z;
	return dat;
}

float downScale(bigFloat n, bigFloat scale) {
	bigFloat x = n;
	x = x / scale;
	return x.ToFloat();
}

void normalize() {
	bigFloat len=x*x +y*y + z*z;
	len=ttmath::Sqrt(len);
	x/=len;
	y/=len;
	z/=len;
}
bigFloat getlen() {
	bigFloat len=x*x +y*y + z*z;
	len = ttmath::Sqrt(len);
	return len;
}
double_vec_ cross(double_vec_ *a,double_vec_ *b)
	{
	if(!b)return double_vec_(0, 0, 0);
	if(!a)return double_vec_(0, 0, 0);
	x = a->y * b->z - a->z * b->y;
	y = a->z * b->x - a->x * b->z;
	z = a->x * b->y - a->y * b->x;
	return double_vec_(x, y, z);
	}
double_vec_& operator =(const double_vec_& P2)
	 {
	   if (this!=&P2)
		{
			x=P2.x;
			y=P2.y;
			z=P2.z;
		}
	   return *this;
	 }
double_vec_& operator +=(const double_vec_& P2)
	{
	if (this != &P2)
		{
		x += P2.x;
		y += P2.y;
		z += P2.z;
		}
	return *this;
	}
double_vec_& operator -=(const double_vec_& P2)
	{
	if (this != &P2)
		{
		x -= P2.x;
		y -= P2.y;
		z -= P2.z;
		}
	return *this;
	}
double_vec_& operator /=(const double d)
	{
	x /= d;
	y /= d;
	z /= d;	
	return *this;
	}
double_vec_& operator *=(const double d)
	{
	x *= d;
	y *= d;
	z *= d;
	return *this;
	}
const double_vec_ operator+(double_vec_ const& rhs) 
	{ 
	  /* Erzeugen eines neuen Objektes, dessen Attribute gezielt einzeln gesetzt werden. Oder: */ 
	  double_vec_ tmp; //Kopie des linken Operanden 
	  tmp.x=x+rhs.x;
	  tmp.y=y+rhs.y;
	  tmp.z=z+rhs.z;
	  return tmp; 
	}

const double_vec_ operator*(bigFloat const& rhs)
	{ 
	  /* Erzeugen eines neuen Objektes, dessen Attribute gezielt einzeln gesetzt werden. Oder: */ 
	  double_vec_ tmp; //Kopie des linken Operanden 
	  tmp.x=x*rhs;
	  tmp.y=y*rhs;
	  tmp.z=z*rhs;
	  return tmp; 
	}
const double_vec_ operator/(bigFloat const& rhs)
	{ 
	/* Erzeugen eines neuen Objektes, dessen Attribute gezielt einzeln gesetzt werden. Oder: */ 
	double_vec_ tmp; //Kopie des linken Operanden 
	tmp.x=x/rhs;
	tmp.y=y/rhs;
	tmp.z=z/rhs;
	return tmp; 
	}
const double_vec_ operator-(double_vec_ const& rhs) 
	{ 
	  /* Erzeugen eines neuen Objektes, dessen Attribute gezielt einzeln gesetzt werden. Oder: */ 
	  double_vec_ tmp; //Kopie des linken Operanden 
	  tmp.x=x-rhs.x;
	  tmp.y=y-rhs.y;
	  tmp.z=z-rhs.z;
	  return tmp; 
	}

bigFloat winkel_vec_010()
{
	double_vec_ vec;
	vec=*this;
	vec.normalize();
	bigFloat w = ttmath::ACos(vec.y);
	if (w != 0.0 && vec.x < 0.0) { w = bigFloat(3.14159 * 2) - w; }
	return w;
}
bigFloat winkel_vec_101()
	{
	double_vec_ vec;
	vec = *this;
	vec.normalize();
	bigFloat w = ttmath::ACos(vec.z);
	if (w != 0.0 && vec.x < 0.) { w = bigFloat(3.14159 * 2) - w; }
	if (w < 0) { w += (3.14159 * 2); }
	return w;
	}

};

const double_vec_ operator+(double_vec_ const& lhs,double_vec_ const& rhs);
const double_vec_ operator*(double_vec_ const& lhs, bigFloat const& rhs);
const double_vec_ operator*(bigFloat const& rhs,double_vec_ const& lhs);
const double_vec_ operator/(double_vec_ const& lhs, bigFloat const& rhs);
const double_vec_ operator-(double_vec_ const& lhs,double_vec_ const& rhs);
bigFloat distance_vec(double_vec_ a,double_vec_ b);
bigFloat winkel_vec(double_vec_ *v1, double_vec_ *v2);
bigFloat winkel_vec_z(double_vec_ *v1, double_vec_ *v2);
bigFloat winkel_vec_y(double_vec_ *v1, double_vec_ *v2);
bigFloat winkel_vec_x(double_vec_ *v1, double_vec_ *v2);

class double_mat_
{
private:
	void set_to_array(bigFloat feld[4][4])
	{
	feld[0][0] = _11;		feld[0][1] = _12;		feld[0][2] = _13;		feld[0][3] = _14;
	feld[1][0] = _21;		feld[1][1] = _22;		feld[1][2] = _23;		feld[1][3] = _24;
	feld[2][0] = _31;		feld[2][1] = _32;		feld[2][2] = _33;		feld[2][3] = _34;
	feld[3][0] = _41;		feld[3][1] = _42;		feld[3][2] = _43;		feld[3][3] = _44;
	}
void set_from_array(bigFloat feld[4][4])
	{
	_11 = feld[0][0];		_12 = feld[0][1];		_13 = feld[0][2];		_14 = feld[0][3];
	_21 = feld[1][0];		_22 = feld[1][1];		_23 = feld[1][2];		_24 = feld[1][3];
	_31 = feld[2][0];		_32 = feld[2][1];		_33 = feld[2][2];		_34 = feld[2][3];
	_41 = feld[3][0];		_42 = feld[3][1];		_43 = feld[3][2];		_44 = feld[3][3];
	}
public:
bigFloat
_11, _12, _13, _14,
_21, _22, _23, _24,
_31, _32, _33, _34,
_41, _42, _43, _44;

bigFloat get(int x, int y)
	{
	if ( x > 3 || x < 0 || y>3 || y < 0 )
		return 0.0;
	/*float* z = (float*)&_11;
	float f = z[x * 4 + y];*/
	if ( x == 0 && y == 0 )	return _11;
	if ( x == 0 && y == 1 )	return _12;
	if ( x == 0 && y == 2 )	return _13;
	if ( x == 0 && y == 3 )	return _14;
	if ( x == 1 && y == 0 )	return _21;
	if ( x == 1 && y == 1 )	return _22;
	if ( x == 1 && y == 2 )	return _23;
	if ( x == 1 && y == 3 )	return _24;
	if ( x == 2 && y == 0 )	return _31;
	if ( x == 2 && y == 1 )	return _32;
	if ( x == 2 && y == 2 )	return _33;
	if ( x == 2 && y == 3 )	return _34;
	if ( x == 3 && y == 0 )	return _41;
	if ( x == 3 && y == 1 )	return _42;
	if ( x == 3 && y == 2 )	return _43;
	if ( x == 3 && y == 3 )	return _44;


	return 0.0;
	}
double_mat_()
	{
	set_identity();
	}
void set_identity()
	{
	_12 = _13 = _14 = 0.0;
	_21 = _23 = _24 = 0.0;
	_31 = _32 = _34 = 0.0;
	_41 = _42 = _43 = 0.0;
	_11 = _22 = _33 = _44 = 1.0;
	}
void set_transform_matrix(double_vec_ v)
	{
	set_identity();
	set_transform_part(v);
	}
void set_transform_part(double_vec_ v)
	{
	_41 = v.x;_42 = v.y;_43 = v.z;
	}
void set_rotation_matrix_x(bigFloat w)
	{
	set_identity();
	_22 = ttmath::Cos(w);	_23 = -ttmath::Sin(w);
	_32 = ttmath::Sin(w);	_33 = ttmath::Cos(w);
	}
void set_rotation_matrix_y(bigFloat w)
	{
	set_identity();
	_11 = ttmath::Cos(w);	_13 = ttmath::Sin(w);
	_31 = -ttmath::Sin(w);	_33 = ttmath::Cos(w);
	}
void set_rotation_matrix_z(bigFloat w)
	{
	set_identity();
	_11 = ttmath::Cos(w);	_12 = -ttmath::Sin(w);
	_21 = ttmath::Sin(w);	_22 = ttmath::Cos(w);
	}
void multiplicate(double_mat_ mat)
	{
	bigFloat a[4][4], b[4][4], c[4][4];
	set_to_array(a);
	mat.set_to_array(b);
	for (int x = 0; x<4; x++)
		{
		for (int j = 0; j<4; j++)
			{
			bigFloat  result = 0;
			for (int y = 0; y<4; y++)
				{
				result += a[x][y] * b[y][j];
				}
			c[x][j] = result;
			}
		}
	set_from_array(c);
	}

double_vec_ multiplicate(double_vec_ vec)
	{
	double_vec_ erg;
	erg.x = _11*vec.x + _21*vec.y + _31*vec.z + _41*1.0;
	erg.y = _12*vec.x + _22*vec.y + _32*vec.z + _42*1.0;
	erg.z = _13*vec.x + _23*vec.y + _33*vec.z + _43*1.0;
	return erg;
	}
/*D3DXVECTOR3 multiplicate(D3DXVECTOR3 vec_dx)
	{
	double_vec_ vec;
	vec = vec_dx;
	double_vec_ erg;
	erg.x = _11*vec.x + _21*vec.y + _31*vec.z + _41*1.0;
	erg.y = _12*vec.x + _22*vec.y + _32*vec.z + _42*1.0;
	erg.z = _13*vec.x + _23*vec.y + _33*vec.z + _43*1.0;
	erg.convert(&vec_dx);
	return vec_dx;
	}
D3DXVECTOR3 &operator*(D3DXVECTOR3 &rhs)
	{
	D3DXVECTOR3 temp;
	temp = multiplicate(rhs);
	return temp;
	}
*/
double_mat_ & operator= (double_mat_ rhs)
	{
	_11 = rhs._11;	_12 = rhs._12;	_13 = rhs._13;	_14 = rhs._14;
	_21 = rhs._21;	_22 = rhs._22;	_23 = rhs._23;	_24 = rhs._24;
	_31 = rhs._31;	_32 = rhs._32;	_33 = rhs._33;	_34 = rhs._34;
	_41 = rhs._41;	_42 = rhs._42;	_43 = rhs._43;	_44 = rhs._44;
	return *this;
	}
double_mat_ operator*(double_mat_ rhs)
	{
	double_mat_ temp;
	temp=*this;
	temp.multiplicate(rhs);
	return temp;
	}
double_vec_ operator*(double_vec_ rhs)
	{
	double_vec_ temp;
	temp = multiplicate(rhs);
	return temp;
	}
};
