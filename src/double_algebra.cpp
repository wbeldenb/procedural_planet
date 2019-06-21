#include "double_algebra.h"


const double_vec_ operator+(double_vec_ const& lhs, double_vec_ const& rhs)
	{
	/* Erzeugen eines neuen Objektes, dessen Attribute gezielt einzeln gesetzt werden. Oder: */
	double_vec_ tmp; //Kopie des linken Operanden 
	tmp.x = lhs.x + rhs.x;
	tmp.y = lhs.y + rhs.y;
	tmp.z = lhs.z + rhs.z;
	return tmp;
	}
const double_vec_ operator*(double_vec_ const& lhs, bigFloat const& rhs)
	{
	/* Erzeugen eines neuen Objektes, dessen Attribute gezielt einzeln gesetzt werden. Oder: */
	double_vec_ tmp; //Kopie des linken Operanden 
	tmp.x = lhs.x*rhs;
	tmp.y = lhs.y*rhs;
	tmp.z = lhs.z*rhs;
	return tmp;
	}
const double_vec_ operator*(bigFloat const& rhs, double_vec_ const& lhs)
	{
	/* Erzeugen eines neuen Objektes, dessen Attribute gezielt einzeln gesetzt werden. Oder: */
	double_vec_ tmp; //Kopie des linken Operanden 
	tmp.x = lhs.x*rhs;
	tmp.y = lhs.y*rhs;
	tmp.z = lhs.z*rhs;
	return tmp;
	}
const double_vec_ operator/(double_vec_ const& lhs, bigFloat const& rhs)
	{
	/* Erzeugen eines neuen Objektes, dessen Attribute gezielt einzeln gesetzt werden. Oder: */
	double_vec_ tmp; //Kopie des linken Operanden 
	tmp.x = lhs.x / rhs;
	tmp.y = lhs.y / rhs;
	tmp.z = lhs.z / rhs;
	return tmp;
	}
const double_vec_ operator-(double_vec_ const& lhs, double_vec_ const& rhs)
	{
	/* Erzeugen eines neuen Objektes, dessen Attribute gezielt einzeln gesetzt werden. Oder: */
	double_vec_ tmp; //Kopie des linken Operanden 
	tmp.x = lhs.x - rhs.x;
	tmp.y = lhs.x - rhs.y;
	tmp.z = lhs.x - rhs.z;
	return tmp;
	}

bigFloat distance_vec(double_vec_ a, double_vec_ b)
	{
	a = a - b;
	bigFloat l = a.getlen();
	return l;
	}

bigFloat winkel_vec(double_vec_ *v1, double_vec_ *v2)
	{
	bigFloat  z = v1->dot_(v2);
	//D3DXVECTOR3 cross;
	//D3DXVec3Cross(&cross,&v1,&v2);
	bigFloat f = ttmath::ACos(z / (v1->getlen()*v2->getlen()));
	return f;
	}
//----------------------
bigFloat winkel_vec_z(double_vec_ *v1, double_vec_ *v2)
	{
	bigFloat  z = v1->dot_(v2);
	double_vec_ cross = *v1;
	cross.cross(v1, v2);
	bigFloat f = ttmath::ACos(z / (v1->getlen()*v2->getlen()));
	if (cross.z < 0.0) f *= -1.;
	return f;
	}
bigFloat winkel_vec_y(double_vec_ *v1, double_vec_ *v2)
	{
	bigFloat  z = v1->dot_(v2);
	double_vec_ cross = *v1;
	cross.cross(v1, v2);
	bigFloat f = ttmath::ACos(z / (v1->getlen()*v2->getlen()));
	if (cross.y < 0.0) f *= -1.;
	return f;
	}
/*bigFloat winkel_vec_y(D3DXVECTOR3 v1, D3DXVECTOR3 v2)
	{
	bigFloat  z = dot(v1,v2);
	D3DXVECTOR3 cross = v1;
	cross = Vec3Cross(v1, v2);
	bigFloat f = acos(z / (Vec3Length(v1)*Vec3Length(v2)));
	if (cross.y < 0.0) f *= -1.;
	return f;
	}
*/
bigFloat winkel_vec_x(double_vec_ *v1, double_vec_ *v2)
	{
	bigFloat  z = v1->dot_(v2);
	double_vec_ cross = *v1;
	cross.cross(v1, v2);
	bigFloat f = ttmath::ACos(z / (v1->getlen()*v2->getlen()));
	if (cross.x < 0.0) f *= -1.;
	return f;
	}
#ifdef USE_DIRECTX
double winkel_vec(D3DXVECTOR3 v_xy)
	{
	double_vec_ vec;
	vec.setvec(v_xy);
	vec.normalize();
	double w = acos(vec.y);
	//wenn die vectoren knapp nicht normiert sind:
	/*if(v_xy.y<(-1.0)) w=PIe;
	if(v_xy.y>1.0) w=0;

	if(w!=0.0 && v_xy.x<0.)	w=(PIe*2.0)-w;
	if(w>PIe)	w-=2.*PIe;*/
	if (w != 0.0 && vec.x<0.)	w = (PIe*2.0) - w;
	//if(w>PIe)	w-=2.*PIe;
	return w;
	}
double winkel_vec_xz(D3DXVECTOR3 v)
	{
	double_vec_ vec;
	vec.setvec(v);
	vec.normalize();
	double w = acos(vec.z);
	if (w != 0.0 && vec.x<0.)	
		w = (PIe*2.0) - w;
	return w;
	}
double winkel_vecx1(D3DXVECTOR3 v_xy)
	{
	double_vec_ vec;
	vec.setvec(v_xy);
	vec.normalize();

	double w = acos(vec.x);
	if (abs(w)<0.0005) return 0.0;
	if (w != 0.0 && vec.y<0.)	w = (PIe*2.0) - w;
	return w;
	}
#endif