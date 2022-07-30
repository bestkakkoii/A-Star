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
#pragma once
#ifndef ASTAR_H
#define ASTAR_H

#ifdef ASTAR_EXPORTS
#define ASTAR_API __declspec(dllexport)
#else
#define ASTAR_API __declspec(dllimport)
#endif

#include "myglobal.hpp"

ASTAR_API const int WINAPI startEx(

	IN const wchar_t* mapid,
	IN const int x1,
	IN const int y1,
	IN const int x2,
	IN const int y2,
	OUT std::vector<POINT>* path
);

ASTAR_API const int WINAPI start(

	IN const wchar_t* mapid,
	IN const int x1,
	IN const int y1,
	IN const int x2,
	IN const int y2,
	OUT wchar_t* path,
	IN const unsigned char retmode
);

ASTAR_API const int WINAPI createNewMap(

	IN const wchar_t* mapid,
	IN const int w,
	IN const int h
);

ASTAR_API const int WINAPI freeMap(

	IN const wchar_t* mapid
);

ASTAR_API const int WINAPI addCollision(
	IN const wchar_t* mapid,
	IN const int x,
	IN const int y
);

ASTAR_API const int WINAPI removeCollision(
	IN const wchar_t* mapid,
	IN const int x,
	IN const int y
);

ASTAR_API const int WINAPI printMap(

	IN const wchar_t* mapid,
	IN const wchar_t* fileName
);

ASTAR_API const int WINAPI setWallColor(IN const int rgb);

ASTAR_API const int WINAPI setRoadColor(IN const int rgb);

ASTAR_API const int WINAPI setPathColor(IN const int rgb);

ASTAR_API const int WINAPI enableAutoPrint(IN const bool b);

ASTAR_API const int WINAPI enableCorner(IN const bool b);

ASTAR_API const int WINAPI setOutputDirectory(IN const wchar_t* dir);

ASTAR_API const int WINAPI mapSaveAs(IN const wchar_t* mapid, IN const wchar_t* fileName);

ASTAR_API const int WINAPI mapLoadFrom(IN const wchar_t* mapid, IN const wchar_t* fileName);

ASTAR_API const int WINAPI isRoad(IN const wchar_t* mapid, IN const int x, IN const int y);

ASTAR_API const int WINAPI isCollision(IN const wchar_t* mapid, IN const int x, IN const int y);

ASTAR_API const int WINAPI readBitmap(IN const wchar_t* mapid, IN const wchar_t* fileName);

#endif // !ASTAR_H