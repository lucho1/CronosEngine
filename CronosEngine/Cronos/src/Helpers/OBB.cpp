/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "Providers/cnpch.h"
#include "OBB.h"
//#include <cmath>

namespace Cronos {

	//#define ROTATE(a,i,j,k,l) g=a.m[i + 4 * j]; h=a.m[k + 4 * l]; a.m[i + 4 * j]=(float)(g-s*(h+g*tau)); a.m[k + 4 * l]=(float)(h+s*(g-h*tau));

	glm::mat4 OBB::getConvarianceMatrix(const glm::vec3* vertPos, int vertCount)
	{
		int i;
		glm::mat4 Cov;

		double S1[3];
		double S2[3][3];

		S1[0] = S1[1] = S1[2] = 0.0;
		S2[0][0] = S2[1][0] = S2[2][0] = 0.0;
		S2[0][1] = S2[1][1] = S2[2][1] = 0.0;
		S2[0][2] = S2[1][2] = S2[2][2] = 0.0;

		// get center of mass
		for (i = 0; i < vertCount; i++)
		{
			S1[0] += vertPos[i].x;
			S1[1] += vertPos[i].y;
			S1[2] += vertPos[i].z;

			S2[0][0] += vertPos[i].x * vertPos[i].x;
			S2[1][1] += vertPos[i].y * vertPos[i].y;
			S2[2][2] += vertPos[i].z * vertPos[i].z;
			S2[0][1] += vertPos[i].x * vertPos[i].y;
			S2[0][2] += vertPos[i].x * vertPos[i].z;
			S2[1][2] += vertPos[i].y * vertPos[i].z;
		}

		float n = (float)vertCount;

		// now get covariances
		Cov[0][0] = (float)(S2[0][0] - S1[0] * S1[0] / n) / n;
		Cov[1][1] = (float)(S2[1][1] - S1[1] * S1[1] / n) / n;
		Cov[2][2] = (float)(S2[2][2] - S1[2] * S1[2] / n) / n;
		Cov[1][0] = (float)(S2[0][1] - S1[0] * S1[1] / n) / n;
		Cov[2][1] = (float)(S2[1][2] - S1[1] * S1[2] / n) / n;
		Cov[2][0] = (float)(S2[0][2] - S1[0] * S1[2] / n) / n;
		Cov[0][1] = Cov[1][0];
		Cov[0][2] = Cov[2][0];
		Cov[1][2] = Cov[2][1];

		return Cov;
	}

	float& OBB::getElement(glm::vec3& point, int index)
	{
		if (index == 0)
			return point.x;
		if (index == 1)
			return point.y;
		if (index == 2)
			return point.z;

		CRONOS_ASSERT(0, "");
		return point.x;
	}

	void OBB::getEigenVectors(glm::mat4* vout, glm::vec3* dout, glm::mat4 a)
	{
		int n = 3;
		int j, iq, ip, i;
		double tresh, theta, tau, t, sm, s, h, g, c;
		int nrot;
		glm::vec3 b;
		glm::vec3 z;
		glm::mat4 v;
		glm::vec3 d;

		v = glm::mat4(1.0f);
		for (ip = 0; ip < n; ip++)
		{
			getElement(b, ip) = a[ip][ip];
			getElement(d, ip) = a[ip][ip];
			getElement(z, ip) = 0.0;
		}

		nrot = 0;

		for (i = 0; i < 50; i++)
		{
			sm = 0.0;
			for (ip = 0; ip < n; ip++)
				for (iq = ip + 1; iq < n; iq++)
					sm += std::fabs(a[iq][ip]);


			if (fabs(sm) < FLT_EPSILON)
			{
				v = glm::transpose(v);
				*vout = v;
				*dout = d;
				return;
			}

			if (i < 3)
				tresh = 0.2 * sm / (n*n);
			else
				tresh = 0.0;

			for (ip = 0; ip < n; ip++)
			{
				for (iq = ip + 1; iq < n; iq++)
				{
					g = 100.0 * std::fabs(a[iq][ip]);
					float dmip = getElement(d, ip);
					float dmiq = getElement(d, iq);

					if (i > 3 && std::fabs(dmip) + g == std::fabs(dmip) && std::fabs(dmiq) + g == std::fabs(dmiq))
					{
						a[iq][ip] = 0.0;
					}
					else if (std::fabs(a[iq][ip] > tresh))
					{
						h = dmiq - dmip;
						if (fabs(h) + g == fabs(h))
						{
							t = (a[iq][ip]) / h;
						}
						else
						{
							theta = 0.5 * h / (a[iq][ip]);
							t = 1.0 / (fabs(theta) + sqrt(1.0 + theta * theta));
							if (theta < 0.0) t = -t;
						}
						c = 1.0 / sqrt(1 + t * t);
						s = t * c;
						tau = s / (1.0 + c);

						h = t * a[iq][ip];
						getElement(z, ip) -= (float)h;
						getElement(z, iq) += (float)h;
						getElement(d, ip) -= (float)h;
						getElement(d, iq) += (float)h;

						a[iq][ip] = 0.0;
						//for (j = 0; j < ip; j++) { ROTATE(a, j, ip, j, iq); }
						//for (j = ip + 1; j < iq; j++) { ROTATE(a, ip, j, j, iq); }
						//for (j = iq + 1; j < n; j++) { ROTATE(a, ip, j, iq, j); }
						//for (j = 0; j < n; j++) { ROTATE(v, j, ip, j, iq); }
						nrot++;
					}
				}
			}

			for (ip = 0; ip < n; ip++)
			{
				getElement(b, ip) += getElement(z, ip);
				getElement(d, ip) = getElement(b, ip);
				getElement(z, ip) = 0.0f;
			}
		}

		v = glm::transpose(v);
		*vout = v;
		*dout = d;
	}

	glm::mat4 OBB::getOBBOrientation(const glm::vec3* vertPos, int num)
	{
		glm::mat4 Cov;

		if (num <= 0)
			return glm::mat4(1.0f);

		Cov = getConvarianceMatrix(vertPos, num);

		// now get eigenvectors
		glm::mat4 Evecs;
		glm::vec3 Evals;
		getEigenVectors(&Evecs, &Evals, Cov);

		Evecs = glm::transpose(Evecs);
		//Change ret value to Evecs
		return Evecs;
	}

	OBB::OBB()
	{
		reset();
	}

	OBB::OBB(const AABB& aabb)
	{
		reset();

		_center = (aabb._min + aabb._max);
		_center = _center * 0.5f;

		_xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
		_yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		_zAxis = glm::vec3(0.0f, 0.0f, 1.0f);

		_extents = aabb._max - aabb._min;
		_extents = _extents * 0.5f;

		computeExtAxis();
	}

	OBB::OBB(const glm::vec3* verts, int num)
	{
		if (!verts) return;

		reset();
		glm::mat4 matTransform = getOBBOrientation(verts, num);

		//	For matTransform is orthogonal, so the inverse matrix is just rotate it;
		matTransform = glm::transpose(matTransform);

		glm::vec3 vecMax = matTransform * glm::vec4(verts[0].x, verts[0].y, verts[0].z, 1.0f);
		glm::vec3 vecMin = vecMax;

		for (int i = 1; i < num; i++)
		{
			glm::vec3 vect = matTransform * glm::vec4(verts[i].x, verts[i].y, verts[i].z, 1.0f);

			vecMax.x = vecMax.x > vect.x ? vecMax.x : vect.x;
			vecMax.y = vecMax.y > vect.y ? vecMax.y : vect.y;
			vecMax.z = vecMax.z > vect.z ? vecMax.z : vect.z;

			vecMin.x = vecMin.x < vect.x ? vecMin.x : vect.x;
			vecMin.y = vecMin.y < vect.y ? vecMin.y : vect.y;
			vecMin.z = vecMin.z < vect.z ? vecMin.z : vect.z;
		}

		matTransform = glm::transpose(matTransform);

		_xAxis = glm::vec3(matTransform[0][0], matTransform[0][1], matTransform[0][2]);
		_yAxis = glm::vec3(matTransform[1][0], matTransform[1][1], matTransform[1][2]);
		_zAxis = glm::vec3(matTransform[2][0], matTransform[2][1], matTransform[2][2]);

		_center = 0.5f * (vecMax + vecMin);
		_center = matTransform * glm::vec4(_center, 1.0f);

		_xAxis = glm::normalize(_xAxis);
		_yAxis = glm::normalize(_yAxis);
		_zAxis = glm::normalize(_zAxis);

		_extents = 0.5f * (vecMax - vecMin);

		computeExtAxis();
	}

	bool OBB::containPoint(const glm::vec3& point) const
	{
		glm::vec3 vd = point - _center;

		float d = glm::dot(vd, _xAxis);
		if (d > _extents.x || d < -_extents.x)
			return false;

		d = glm::dot(vd, _yAxis);
		if (d > _extents.y || d < -_extents.y)
			return false;

		d = glm::dot(vd, _zAxis);
		if (d > _extents.z || d < -_extents.z)
			return false;

		return true;
	}

	void OBB::set(const glm::vec3& center, const glm::vec3& xAxis, const glm::vec3& yAxis, const glm::vec3& zAxis, const glm::vec3& extents)
	{
		_center = center;
		_xAxis = xAxis;
		_yAxis = yAxis;
		_zAxis = zAxis;
		_extents = extents;
	}

	void OBB::reset()
	{
		memset(this, 0, sizeof(OBB));
	}

	void OBB::getCorners(glm::vec3* verts) const
	{
		verts[0] = _center - _extentX + _extentY + _extentZ;     // left top front
		verts[1] = _center - _extentX - _extentY + _extentZ;     // left bottom front
		verts[2] = _center + _extentX - _extentY + _extentZ;     // right bottom front
		verts[3] = _center + _extentX + _extentY + _extentZ;     // right top front

		verts[4] = _center + _extentX + _extentY - _extentZ;     // right top back
		verts[5] = _center + _extentX - _extentY - _extentZ;     // right bottom back
		verts[6] = _center - _extentX - _extentY - _extentZ;     // left bottom back
		verts[7] = _center - _extentX + _extentY - _extentZ;     // left top back
	}

	float OBB::projectPoint(const glm::vec3& point, const glm::vec3& axis)const
	{
		float dot = glm::dot(axis, point);
		float ret = dot * point.length();
		return ret;
	}

	void OBB::getInterval(const OBB& box, const glm::vec3& axis, float &min, float &max)const
	{
		glm::vec3 corners[8];
		box.getCorners(corners);
		float value;
		min = max = projectPoint(axis, corners[0]);
		for (int i = 1; i < 8; i++)
		{
			value = projectPoint(axis, corners[i]);
			min = MIN(min, value);
			max = MAX(max, value);
		}
	}

	glm::vec3 OBB::getEdgeDirection(int index)const
	{
		glm::vec3 corners[8];
		getCorners(corners);

		glm::vec3 tmpLine;
		switch (index)
		{
		case 0:// edge with x axis
			tmpLine = corners[5] - corners[6];
			tmpLine = glm::normalize(tmpLine);
			break;
		case 1:// edge with y axis
			tmpLine = corners[7] - corners[6];
			tmpLine = glm::normalize(tmpLine);
			break;
		case 2:// edge with z axis
			tmpLine = corners[1] - corners[6];
			tmpLine = glm::normalize(tmpLine);
			break;
		default:
			CRONOS_ASSERT(0, "Invalid index!");
			break;
		}
		return tmpLine;
	}

	glm::vec3 OBB::getFaceDirection(int index) const
	{
		glm::vec3 corners[8];
		getCorners(corners);

		glm::vec3 faceDirection, v0, v1;
		switch (index)
		{
		case 0:// front and back
			v0 = corners[2] - corners[1];
			v1 = corners[0] - corners[1];
			faceDirection = glm::cross(v0, v1);
			faceDirection = glm::normalize(faceDirection);
			break;
		case 1:// left and right
			v0 = corners[5] - corners[2];
			v1 = corners[3] - corners[2];
			faceDirection = glm::cross(v0, v1);
			faceDirection = glm::normalize(faceDirection);
			break;
		case 2:// top and bottom
			v0 = corners[1] - corners[2];
			v1 = corners[5] - corners[2];
			faceDirection = glm::cross(v0, v1);
			faceDirection = glm::normalize(faceDirection);
			break;
		default:
			CRONOS_ASSERT(0, "Invalid index!");
			break;
		}
		return faceDirection;
	}

	bool OBB::intersects(const OBB& box) const
	{
		float min1, max1, min2, max2;
		for (int i = 0; i < 3; i++)
		{
			getInterval(*this, getFaceDirection(i), min1, max1);
			getInterval(box, getFaceDirection(i), min2, max2);
			if (max1 < min2 || max2 < min1) return false;
		}

		for (int i = 0; i < 3; i++)
		{
			getInterval(*this, box.getFaceDirection(i), min1, max1);
			getInterval(box, box.getFaceDirection(i), min2, max2);
			if (max1 < min2 || max2 < min1) return false;
		}

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				glm::vec3 axis;
				axis = glm::cross(getEdgeDirection(i), box.getEdgeDirection(j));
				getInterval(*this, axis, min1, max1);
				getInterval(box, axis, min2, max2);
				if (max1 < min2 || max2 < min1) return false;
			}
		}

		return true;
	}


	void OBB::transform(const glm::mat4& mat)
	{
		glm::vec4 newcenter = mat * glm::vec4(_center.x, _center.y, _center.z, 1.0f);// center;
		_center.x = newcenter.x;
		_center.y = newcenter.y;
		_center.z = newcenter.z;

		_xAxis = mat * glm::vec4(_xAxis, 1.0f);
		_yAxis = mat * glm::vec4(_yAxis, 1.0f);
		_zAxis = mat * glm::vec4(_zAxis, 1.0f);

		_xAxis = glm::normalize(_xAxis);
		_yAxis = glm::normalize(_yAxis);
		_zAxis = glm::normalize(_zAxis);

		glm::vec3 scale, trans, skew;
		glm::quat quat;
		glm::vec4 projection;
		glm::decompose(mat, scale, quat, trans, skew, projection);

		_extents.x *= scale.x;
		_extents.y *= scale.y;
		_extents.z *= scale.z;

		computeExtAxis();
	}

}