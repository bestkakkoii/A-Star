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
#include "castar.h"

const int CAStar::_start(const std::wstring& mapid, const MyPoint& startPoint, const MyPoint& endPoint, std::vector<MyPoint>* v)
{
	auto draw = [&, this](const MyMap& map)->void
	{
		qimage img(map.width, map.height);

		MyRGB color = {};

		// draw map & path
		for (const auto& obj : map.data)
		{
			color = (std::ranges::find(*v, obj.first) != v->end()) ? (pathColor) :
				(((obj.second) == TYPE_ROAD) ? (roadColor) : (wallColor));

			img.setPixel(obj.first, color);
		}

		if (this->outputdir.empty())
		{
			WCHAR szFilePath[MAX_PATH + 1];
			GetModuleFileName(NULL, szFilePath, MAX_PATH);
			(wcsrchr(szFilePath, TEXT('\\')))[1] = '\0';//replace '\\' to '\0'

			const std::wstring f(std::format(TEXT(R"({}\{}.bmp)"), szFilePath, mapid));
			std::ofstream(f) << img;
		}
		else
		{
			const std::wstring f(std::format(TEXT(R"({}\{}.bmp)"), this->outputdir, mapid));
			std::ofstream(f) << img;
		}
	};

	do
	{
		std::shared_lock<std::shared_mutex> lck(m_mutex);
		v->clear();
		const MyMap map = global_maps.at(mapid);
		const Callback can_pass = [&map](const MyPoint& pos)->bool
		{
			if (map.data.contains(pos))
				return map.data.at(pos) == TYPE_ROAD;
			else
				return false;
		};

		MyParams param(map.width, map.height, cornerenable, startPoint, endPoint, can_pass);
		BlockAllocator allocator;
		MyAStar astar(&allocator);

		bool bret = astar.find(param, v);

		if (!bret) break;
		if (enableautoprint)
			draw(map);
		return 1;
	} while (false);
	return 0;
}

const bool CAStar::_createNewMap(const std::wstring& mapid, const int w, const int h)
{
	std::unique_lock<std::shared_mutex> lck(m_mutex);
	bool bret = false;
	do
	{
		if (((w) <= 0) || ((h) <= 0))
			break;

		global_maps.erase(mapid);
		MyMap map = {};
		map.width = w;
		map.height = h;
		int j = 0;
		for (int i = 0; i < w; ++i)
		{
			for (j = 0; j < h; ++j)
			{
				map.data.insert(std::pair<MyPoint, OBJECTTYPE>{MyPoint{ i, j }, TYPE_ROAD
				});
			}
		}

		global_maps[mapid] = map;
		bret = true;
	} while (false);
	return bret;
}

const bool CAStar::_freeMap(const std::wstring& mapid)
{
	std::unique_lock<std::shared_mutex> lck(m_mutex);
	global_maps.erase(mapid);
	return true;
}

const bool CAStar::_addCollision(const std::wstring& mapid, const int x, const int y)
{
	bool bret = false;
	do
	{
		if (((x) < 0) ||
			((y) < 0) ||
			(NULL == (global_maps.at(mapid).width)) ||
			(NULL == (global_maps.at(mapid).height)) ||
			((x) >= (global_maps.at(mapid).width)) ||
			((y) >= global_maps.at(mapid).height))
			break;

		global_maps.at(mapid).data.at(MyPoint{ x, y }) = TYPE_COLLISION;
		bret = true;
	} while (false);
	return bret;
}

const bool CAStar::_removeCollision(const std::wstring& mapid, const int x, const int y)
{
	bool bret = false;
	do
	{
		if (x < 0 ||
			y < 0 ||
			NULL == global_maps.at(mapid).width ||
			NULL == global_maps.at(mapid).height ||
			x >= global_maps.at(mapid).width ||
			y >= global_maps.at(mapid).height)
			break;

		global_maps.at(mapid).data.at(MyPoint{ x, y }) = TYPE_ROAD;
		bret = true;
	} while (false);
	return bret;
}

const int CAStar::_printMap(const std::wstring& mapid, const std::wstring& fileName)
{
	std::shared_lock<std::shared_mutex> lck(m_mutex);
	int nret = 0;
	do
	{
		const MyMap map = global_maps.at(mapid);
		qimage img(map.width, map.height);

		MyRGB color = {};
		for (const auto& obj : map.data)
		{
			color = obj.second == TYPE_ROAD ? roadColor : wallColor;
			img.setPixel(obj.first, color);
		}

		nret = 1;
		std::ofstream(fileName) << img;
	} while (false);
	return nret;
}

void CAStar::_setWallColor(const MyRGB& rbg)
{
	wallColor = rbg;
}

void CAStar::_setRoadColor(const MyRGB& rbg)
{
	roadColor = rbg;
}

void CAStar::_setPathColor(const MyRGB& rbg)
{
	pathColor = rbg;
}

const int CAStar::_setOutputDirectory(const std::wstring& dir)
{
	this->outputdir = dir;
	return !dir.empty();
}

const int CAStar::_mapSaveAs(const std::wstring& mapid, const std::wstring& fileName)
{
	const MyMap map = global_maps.at(mapid);

	std::ofstream ofs(fileName, std::ios::binary);
	if (!ofs.is_open())
	{
		return -1;
	}

	ofs.write(reinterpret_cast<const char*>(&map.width), sizeof(map.width));
	ofs.write(reinterpret_cast<const char*>(&map.height), sizeof(map.height));

	int y = 0;
	for (int x = 0; x < map.width; ++x)
	{
		for (y = 0; y < map.height; ++y)
		{
			ofs.write(reinterpret_cast<const char*>(&map.data.at(MyPoint{ x, y })), sizeof(uint8_t));
		}
	}
	ofs.close();

	return 1;
}

const int CAStar::_mapLoadFrom(const std::wstring& mapid, const std::wstring& fileName)
{
	std::ifstream ifs(fileName, std::ios::binary);
	if (!ifs.is_open())
	{
		return -1;
	}

	int width = 0;
	int height = 0;

	ifs.read(reinterpret_cast<char*>(&width), sizeof(width));
	ifs.read(reinterpret_cast<char*>(&height), sizeof(height));

	std::ignore = this->_createNewMap(mapid, width, height);

	int y = 0;
	for (int x = 0; x < width; ++x)
	{
		for (y = 0; y < height; ++y)
		{
			ifs.read(reinterpret_cast<char*>(&global_maps[mapid].data.at(MyPoint{ x, y })), sizeof(uint8_t));
		}
	}
	ifs.close();

	return 1;
}

const int CAStar::_getRoads(const std::wstring& mapid, std::vector<MyPoint>* v)
{
	std::shared_lock<std::shared_mutex> lck(m_mutex);
	const MyMap map = global_maps.at(mapid);

	for (const auto& obj : map.data)
	{
		if (TYPE_ROAD == obj.second)
		{
			v->push_back(obj.first);
		}
	}
	return v->size();
}

const int CAStar::_getCollisions(const std::wstring& mapid, std::vector<MyPoint>* v)
{
	std::shared_lock<std::shared_mutex> lck(m_mutex);
	const MyMap map = global_maps.at(mapid);

	for (const auto& obj : map.data)
	{
		if (TYPE_COLLISION == obj.second)
		{
			v->push_back(obj.first);
		}
	}

	return v->size();
}

const int CAStar::_readBMPToBinary(const std::wstring& mapid, const std::wstring& fileName)
{
	int width = 0, height = 0;
	std::vector<std::vector<MyRGB>> vec;
	MyDraw d{};
	if (!d.bmpRead(vec, &width, &height, fileName))
		return 0;

	std::ignore = _createNewMap(mapid, width, height);

	auto CHECKRANGE = [&height](int y)->bool
	{
		return (((height)-(y)) > 0) && (((height)-(y)) < height);
	};

	int y = 0;
	for (int x = 0; x < width; ++x)
	{
		for (y = 0; y < height; ++y)
		{
			if ((vec.at(x).at(y)) == (wallColor))
			{
				if (CHECKRANGE(y))//upside-down
					std::ignore = _addCollision(mapid, x, height - y);
			}
			else if ((vec.at(x).at(y)) == (roadColor))
			{
				if (CHECKRANGE(y))//upside-down
					std::ignore = _removeCollision(mapid, x, height - y);
			}
		}
	}

	return 1;
}