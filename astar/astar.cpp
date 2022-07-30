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
#include "astar.h"
#include "castar.h"

BOOL APIENTRY DllMain(HMODULE hModule,
	int  ul_reason_for_call,
	LPVOID lpReserved
)
{
	std::ignore = hModule;
	std::ignore = lpReserved;
	if (DLL_PROCESS_ATTACH == ul_reason_for_call) [[likely]]
	{
		CASTAR_INS;
	}
	else if (DLL_PROCESS_DETACH == ul_reason_for_call) [[unlikely]]
	{
		;
	}
	return TRUE;
}

ASTAR_API const int WINAPI startEx(

	IN const wchar_t* mapid,
	IN const int x1,
	IN const int y1,
	IN const int x2,
	IN const int y2,
	OUT std::vector<POINT>* path
)
{
	CAStar& a = CASTAR_INS;
	std::vector<MyPoint> v;

	int ret = a._start(mapid, MyPoint{ x1, y1 }, MyPoint{ x2, y2 }, &v);
	if (ret)
	{
		ret = static_cast<int>(v.size());
		*path = std::vector<POINT>();
		for (const auto& it : v)
		{
			path->push_back(it.toPoint());
		}
	}

	return ret;
}

ASTAR_API const int WINAPI start(

	IN const wchar_t* mapid,
	IN const int x1,
	IN const int y1,
	IN const int x2,
	IN const int y2,
	OUT wchar_t* path,
	IN const unsigned char retmode
)
{
	do
	{
		CAStar& a = CASTAR_INS;
		std::vector<MyPoint> v;
		const int ret = a._start(mapid, MyPoint{ x1, y1 }, MyPoint{ x2, y2 }, &v);
		if (!ret) break;

		switch (retmode)
		{
		case 0: MAKETCARRAY(v, path); return ret;
		case 1: MAKESTRARRAY(v, path); return ret;
		default: break;
		}
	} while (0);
	return 0;
}

ASTAR_API const int WINAPI createNewMap(IN const wchar_t* mapid, IN const int w, IN const int h)
{
	CAStar& a = CASTAR_INS;
	return a._createNewMap(mapid, w, h);
}

ASTAR_API const int WINAPI freeMap(IN const wchar_t* mapid)
{
	CAStar& a = CASTAR_INS;
	return a._freeMap(mapid);
}

ASTAR_API const int WINAPI addCollision(IN const wchar_t* mapid, IN const int x, IN const int y)
{
	CAStar& a = CASTAR_INS;
	return a._addCollision(mapid, x, y);
}

ASTAR_API const int WINAPI removeCollision(IN const wchar_t* mapid, IN const int x, IN const int y)
{
	CAStar& a = CASTAR_INS;
	return a._removeCollision(mapid, x, y);
}

ASTAR_API const int WINAPI printMap(IN const wchar_t* mapid, IN const wchar_t* fileName)
{
	CAStar& a = CASTAR_INS;
	return a._printMap(mapid, fileName);
}

ASTAR_API const int WINAPI setWallColor(IN const int rgb)
{
	CAStar& a = CASTAR_INS;
	const MyRGB c = { static_cast<uint8_t>(rgb >> 16), static_cast<uint8_t>(rgb >> 8), static_cast<uint8_t>(rgb) };
	a._setWallColor(c);
	return 1;
}

ASTAR_API const int WINAPI setRoadColor(IN const int rgb)
{
	CAStar& a = CASTAR_INS;
	const MyRGB c = { static_cast<uint8_t>(rgb >> 16), static_cast<uint8_t>(rgb >> 8), static_cast<uint8_t>(rgb) };
	a._setRoadColor(c);
	return 1;
}

ASTAR_API const int WINAPI setPathColor(IN const int rgb)
{
	CAStar& a = CASTAR_INS;
	const MyRGB c = { static_cast<uint8_t>(rgb >> 16), static_cast<uint8_t>(rgb >> 8), static_cast<uint8_t>(rgb) };
	a._setPathColor(c);
	return 1;
}

ASTAR_API const int WINAPI enableAutoPrint(IN const bool b)
{
	CAStar& a = CASTAR_INS;
	return a._enableAutoPrint(b);
}

ASTAR_API const int WINAPI enableCorner(IN const bool b)
{
	CAStar& a = CASTAR_INS;
	return a._enableCorner(b);
}

ASTAR_API const int WINAPI setOutputDirectory(IN const wchar_t* dir)
{
	CAStar& a = CASTAR_INS;
	return a._setOutputDirectory(dir);
}

ASTAR_API const int WINAPI mapSaveAs(IN const wchar_t* mapid, IN const wchar_t* fileName)
{
	CAStar& a = CASTAR_INS;
	return a._mapSaveAs(mapid, fileName);
}

ASTAR_API const int WINAPI mapLoadFrom(IN const wchar_t* mapid, IN const wchar_t* fileName)
{
	CAStar& a = CASTAR_INS;
	return a._mapLoadFrom(mapid, fileName);
}

ASTAR_API const int WINAPI isRoad(IN const wchar_t* mapid, IN const int x, IN const int y)
{
	CAStar& a = CASTAR_INS;
	std::vector<MyPoint> v;
	const int ret = a._getRoads(mapid, &v);
	if (NULL == ret)
		return -1;

	return std::ranges::find(v, MyPoint{ x, y }) != v.end();
}

ASTAR_API const int WINAPI isCollision(IN const wchar_t* mapid, IN const int x, IN const int y)
{
	CAStar& a = CASTAR_INS;
	std::vector<MyPoint> v;
	const int ret = a._getCollisions(mapid, &v);
	if (NULL == ret)
		return -1;

	return std::ranges::find(v, MyPoint{ x, y }) != v.end();
}

ASTAR_API const int WINAPI readBitmap(IN const wchar_t* mapid, IN const wchar_t* fileName)
{
	CAStar& a = CASTAR_INS;
	return a._readBMPToBinary(mapid, fileName);
}