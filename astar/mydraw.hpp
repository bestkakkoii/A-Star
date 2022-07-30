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
#ifndef MYDRAW_H
#define MYDRAW_H
#pragma execution_character_set("utf-8")
#include "mypoint.h"

class MyDraw
{
public:

	MyDraw() = default;

	inline int ToHumanRead(char* str, int size)
	{
		// Convert to Big Endian
		int l = 0;
		memcpy(&l, str, size);
		return l;
	}

	bool bmpRead(std::vector<std::vector<MyRGB>>& imageVec, int* w, int* h, std::wstring fileName)
	{
		std::ifstream bmpfile(fileName, std::ios::in | std::ios::binary); // open the file
		if (bmpfile.is_open())
		{
			bmpfile.read(reinterpret_cast<char*>(&FileHeader), sizeof(FileHeader)); // Read BITMAPFILEHEADER
			bmpfile.read(reinterpret_cast<char*>(&InfoHeader), sizeof(InfoHeader)); // Read BITMAPINFOHEADER
		}

		*w = ToHumanRead(InfoHeader.biHeight, 4);
		*h = ToHumanRead(InfoHeader.biWidth, 4);

		bmpfile.close();

		std::ifstream file(fileName, std::ios::in | std::ios::binary);
		if (!file)
			return false;

		imageVec.resize(static_cast<size_t>(*w), std::vector<MyRGB>(static_cast<size_t>(*h)));

		// skip header
		const std::ifstream::off_type headerSize = 54;
		file.seekg(headerSize, std::ios::beg);
		// read body
		for (size_t y = 0; y != imageVec.size(); ++y) {
			for (size_t x = 0; x != imageVec[0].size(); ++x) {
				char chR, chG, chB;
				file.get(chB).get(chG).get(chR);

				imageVec[y][x].b = chB;
				imageVec[y][x].g = chG;
				imageVec[y][x].r = chR;
			}
		}

		file.close();

		return true;
	}

private:
	BitMapFileHeader FileHeader;
	BitmapInfoHeader InfoHeader;
};

struct qimage
{
public:
	qimage(const int width, const int height)
		: wp(width), hp(height), rgb(wp* hp * 3)
	{}
	uint8_t& r(int x, int y) { return rgb[(x + y * wp) * 3 + 2]; }
	uint8_t& g(int x, int y) { return rgb[(x + y * wp) * 3 + 1]; }
	uint8_t& b(int x, int y) { return rgb[(x + y * wp) * 3 + 0]; }

	void setPixel(const MyPoint& p, const MyRGB& color)
	{
		if (CHECKRANGE(p.y()))
		{
			rgb.at((p.x() + (hp - p.y()) * wp) * 3 + 2) = color.r;
			rgb.at((p.x() + (hp - p.y()) * wp) * 3 + 1) = color.g;
			rgb.at((p.x() + (hp - p.y()) * wp) * 3 + 0) = color.b;
		}
	}

	constexpr uint32_t w() const
	{
		return wp;
	}

	constexpr uint32_t h() const
	{
		return hp;
	}

	const uint8_t* data() const
	{
		return rgb.data();
	}

private:
	int wp;
	int hp;
	std::vector<uint8_t> rgb;

	bool CHECKRANGE(int y) const
	{
		return (((h()) - (y)) > 0) && (((h()) - (y)) < h());
	};
};

template<class Stream>
Stream& operator<<(Stream& out, qimage const& img)
{
	uint32_t w = img.w(), h = img.h();
	uint32_t pad = w * -3 & 3;
	uint32_t total = 54 + 3 * w * h + pad * h;
	uint32_t head[13] = { total, 0, 54, 40, w, h, (24 << 16) | 1 };
	const char* rgb = reinterpret_cast<char const*>(img.data());

	out.write("BM", 2);
	out.write(reinterpret_cast<char*>(head), 52);
	for (uint32_t i = 0; i < h; i++)
	{
		out.write(rgb + (3 * w * i), 3 * w);
		out.write(reinterpret_cast<char*>(&pad), pad);
	}
	return out;
}

#endif