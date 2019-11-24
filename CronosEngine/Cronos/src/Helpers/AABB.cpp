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
#include "AABB.h"

namespace Cronos {

	AABB::AABB()
	{
		reset();
	}

	AABB::AABB(const glm::vec3& min, const glm::vec3& max)
	{
		set(min, max);
	}

	const glm::vec3 AABB::getCenter() const
	{
		glm::vec3 center;
		center.x = 0.5f*(_min.x + _max.x);
		center.y = 0.5f*(_min.y + _max.y);
		center.z = 0.5f*(_min.z + _max.z);

		return center;
	}

	void AABB::getCorners(glm::vec3 *dst) const
	{
		assert(dst);

		// Near face, specified counter-clockwise looking towards the origin from the positive z-axis.
		// Left-top-front.
		dst[0] = glm::vec3(_min.x, _max.y, _max.z);
		// Left-bottom-front.
		dst[1] = glm::vec3(_min.x, _min.y, _max.z);
		// Right-bottom-front.
		dst[2] = glm::vec3(_max.x, _min.y, _max.z);
		// Right-top-front.
		dst[3] = glm::vec3(_max.x, _max.y, _max.z);

		// Far face, specified counter-clockwise looking towards the origin from the negative z-axis.
		// Right-top-back.
		dst[4] = glm::vec3(_max.x, _max.y, _min.z);
		// Right-bottom-back.
		dst[5] = glm::vec3(_max.x, _min.y, _min.z);
		// Left-bottom-back.
		dst[6] = glm::vec3(_min.x, _min.y, _min.z);
		// Left-top-back.
		dst[7] = glm::vec3(_min.x, _max.y, _min.z);
	}

	bool AABB::intersects(const AABB& aabb) const
	{
		return ((_min.x >= aabb._min.x && _min.x <= aabb._max.x) || (aabb._min.x >= _min.x && aabb._min.x <= _max.x)) &&
			((_min.y >= aabb._min.y && _min.y <= aabb._max.y) || (aabb._min.y >= _min.y && aabb._min.y <= _max.y)) &&
			((_min.z >= aabb._min.z && _min.z <= aabb._max.z) || (aabb._min.z >= _min.z && aabb._min.z <= _max.z));
	}

	bool AABB::containPoint(const glm::vec3& point) const
	{
		if (point.x < _min.x) return false;
		if (point.y < _min.y) return false;
		if (point.z < _min.z) return false;
		if (point.x > _max.x) return false;
		if (point.y > _max.y) return false;
		if (point.z > _max.z) return false;
		return true;
	}

	void AABB::merge(const AABB& box)
	{
		// Calculate the new minimum point.
		_min.x = MIN(_min.x, box._min.x);
		_min.y = MIN(_min.y, box._min.y);
		_min.z = MIN(_min.z, box._min.z);

		// Calculate the new maximum point.
		_max.x = MAX(_max.x, box._max.x);
		_max.y = MAX(_max.y, box._max.y);
		_max.z = MAX(_max.z, box._max.z);
	}

	void AABB::set(const glm::vec3& min, const glm::vec3& max)
	{
		this->_min = min;
		this->_max = max;
	}

	void AABB::reset()
	{
		_min = glm::vec3(99999.0f, 99999.0f, 99999.0f);
		_max = glm::vec3(-99999.0f, -99999.0f, -99999.0f);
	}

	bool AABB::isEmpty() const
	{
		return _min.x > _max.x || _min.y > _max.y || _min.z > _max.z;
	}

	void AABB::updateMinMax(const glm::vec3* point, size_t num)
	{
		for (size_t i = 0; i < num; i++)
		{
			// Leftmost point.
			if (point[i].x < _min.x)
				_min.x = point[i].x;

			// Lowest point.
			if (point[i].y < _min.y)
				_min.y = point[i].y;

			// Farthest point.
			if (point[i].z < _min.z)
				_min.z = point[i].z;

			// Rightmost point.
			if (point[i].x > _max.x)
				_max.x = point[i].x;

			// Highest point.
			if (point[i].y > _max.y)
				_max.y = point[i].y;

			// Nearest point.
			if (point[i].z > _max.z)
				_max.z = point[i].z;
		}
	}

	void AABB::transform(const glm::mat4& mat)
	{
		glm::vec3 corners[8];
		// Near face, specified counter-clockwise
	   // Left-top-front.
		corners[0] = glm::vec3(_min.x, _max.y, _max.z);
		// Left-bottom-front.
		corners[1] = glm::vec3(_min.x, _min.y, _max.z);
		// Right-bottom-front.
		corners[2] = glm::vec3(_max.x, _min.y, _max.z);
		// Right-top-front.
		corners[3] = glm::vec3(_max.x, _max.y, _max.z);

		// Far face, specified clockwise
		// Right-top-back.
		corners[4] = glm::vec3(_max.x, _max.y, _min.z);
		// Right-bottom-back.
		corners[5] = glm::vec3(_max.x, _min.y, _min.z);
		// Left-bottom-back.
		corners[6] = glm::vec3(_min.x, _min.y, _min.z);
		// Left-top-back.
		corners[7] = glm::vec3(_min.x, _max.y, _min.z);

		// Transform the corners, recalculate the min and max points along the way.
		for (int i = 0; i < 8; i++)
			corners[i] = glm::vec3(mat * glm::vec4(corners[i], 1.0f));
		//mat.transformPoint(&corners[i]);

	//glm::vec3 transformed = glm::vec3(matrix * glm::vec4(point, 0.0))

		reset();

		updateMinMax(corners, 8);
	}

}