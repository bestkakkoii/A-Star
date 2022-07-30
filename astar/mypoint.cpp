/*
* Copyright (c) 2019-2022 bestkakkoii llc.
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/
#include "mypoint.h"

MyPoint::MyPoint(int xpos, int ypos)
	: xp(xpos)
	, yp(ypos)
{
}

int MyPoint::x() const
{
	return xp;
}

int MyPoint::y() const
{
	return yp;
}

void MyPoint::reset(int x1, int y1)
{
	xp = x1;
	yp = y1;
}

bool MyPoint::isNull() const
{
	return (xp == 0 && yp == 0);
}

int MyPoint::manhattanLength() const
{
	return static_cast<int>(abs(x()) + abs(y()));
}

POINT MyPoint::toPoint() const
{
	POINT p;
	p.x = xp;
	p.y = yp;
	return p;
}