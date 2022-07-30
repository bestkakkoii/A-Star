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
#ifndef CASTAR_H
#define CASTAR_H
#include "myastar.h"

class CAStar
{
	MY_DISABLE_COPY_MOVE(CAStar) // make sure it is a singleton pattern
private:
	// function locker for multi-thread
	mutable std::shared_mutex m_mutex;
	//map data
	std::unordered_map<std::wstring, MyMap> global_maps = {};

	// default color
	MyRGB wallColor = { 0, 0, 0 };
	MyRGB roadColor = { 255, 255, 255 };
	MyRGB pathColor = { 234, 103, 105 };

	// enable output bitmap when path found
	bool enableautoprint;

	// enable 8-dir otherwise 4-dir
	bool cornerenable;

	// the path where you save the bitmap with path highlight
	std::wstring outputdir;

	explicit CAStar()
		: cornerenable(true)
		, enableautoprint(false)
		, outputdir(TEXT("\0"))
	{
	}

public:
	virtual ~CAStar() {
	}

#define CASTAR_INS CAStar::get_instance();
	static CAStar& get_instance() {
		static CAStar instance;
		return instance;
	}

	// set enable or disable auto print
	const int __vectorcall _enableAutoPrint(const bool b)
	{
		enableautoprint = b;
		return 1;
	}

	// set enable or disable corner allow
	MY_REQUIRED_RESULT const int __vectorcall _enableCorner(const bool b)
	{
		std::unique_lock<std::shared_mutex> lck(m_mutex);
		cornerenable = b;
		return 1;
	}

	// start finding path
	MY_REQUIRED_RESULT const int __vectorcall _start(const std::wstring& mapid, const MyPoint& startPoint, const MyPoint& endPoint, std::vector<MyPoint>* v);

	// insert a new empty map in to unordered_map pretent all points are passable
	const bool __vectorcall _createNewMap(const std::wstring& mapid, const int w, const int h);

	// erase map from unordered_map
	MY_REQUIRED_RESULT const bool __vectorcall _freeMap(const std::wstring& mapid);

	// mark as non-passable to the sepcific point
	const bool __vectorcall _addCollision(const std::wstring& mapid, const int x, const int y);

	// mark as passable to the sepcific point
	const bool __vectorcall _removeCollision(const std::wstring& mapid, const int x, const int y);

	// output the map to bitmap file
	MY_REQUIRED_RESULT const int __vectorcall _printMap(const std::wstring& mapid, const std::wstring& fileName);

	// set the color of the wall
	void __vectorcall _setWallColor(const MyRGB& rbg);

	// set the color of the road
	void __vectorcall _setRoadColor(const MyRGB& rbg);

	// set the color of the path
	void __vectorcall _setPathColor(const MyRGB& rbg);

	// set the output directory
	MY_REQUIRED_RESULT const int __vectorcall _setOutputDirectory(const std::wstring& dir);

	// save the map to the binary(.dat) file
	MY_REQUIRED_RESULT const int __vectorcall _mapSaveAs(const std::wstring& mapid, const std::wstring& fileName);

	// load the map from the binary(.dat) file
	MY_REQUIRED_RESULT const int __vectorcall _mapLoadFrom(const std::wstring& mapid, const std::wstring& fileName);

	// get all passable points
	MY_REQUIRED_RESULT const int __vectorcall _getRoads(const std::wstring& mapid, std::vector<MyPoint>* v);

	// get all non-passable points
	MY_REQUIRED_RESULT const int __vectorcall _getCollisions(const std::wstring& mapid, std::vector<MyPoint>* v);

	// load map from the bitmap file
	MY_REQUIRED_RESULT const int __vectorcall _readBMPToBinary(const std::wstring& mapid, const std::wstring& fileName);
};

#endif