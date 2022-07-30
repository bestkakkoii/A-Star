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
#ifndef MYGLOBAL_H
#define MYGLOBAL_H
#pragma execution_character_set("utf-8")
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <cassert>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>

#include <math.h>
#include <cmath>
#include <algorithm>

#include <stdexcept>
#include <vector>
#include <format>
#include <ranges>
#include <memory>
#include <functional>

#include <condition_variable>
#include <mutex>
#include <shared_mutex>

//3rd party
#include "blockallocator.h"

typedef unsigned char       uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;

#define MY_DISABLE_COPY(Class) \
    Class(const Class &) = delete;\
    Class &operator=(const Class &) = delete;

#define MY_DISABLE_MOVE(Class) \
    Class(Class &&) = delete; \
    Class &operator=(Class &&) = delete;

#define MY_DISABLE_COPY_MOVE(Class) \
    MY_DISABLE_COPY(Class) \
    MY_DISABLE_MOVE(Class)

#if __has_cpp_attribute(nodiscard)
#  undef MY_REQUIRED_RESULT
#  define MY_REQUIRED_RESULT [[nodiscard]]
#endif

#  define MY_CHECK_PTR(p) do { if (!(p)) throw std::bad_alloc{}; } while (false)

template <typename T>
inline T* my_check_ptr(T* p) { MY_CHECK_PTR(p); return p; }

struct MyRGB
{
	uint8_t r;
	uint8_t g;
	uint8_t b;

	bool operator==(const MyRGB& rhs) const
	{
		return (r == rhs.r) && (g == rhs.g) && (b == rhs.b);
	}
};

typedef struct tagBitmapFileHeader
{
	char bfType[2]; // must be "BM"
	char bfSize[4]; // the size of the bmp file
	char bfReserved1[2];
	char bfReserved2[2];
	char bfOffBits[4]; // the offset to the bitmap data
} BitMapFileHeader;

typedef struct tagBitmapInfoHeader
{
	char biSize[4]; // the size of BITMAPINFOHEADER
	char biWidth[4]; // width (pixels)
	char biHeight[4]; // height (pixels)
	char biPlanes[2]; // color planes
	char biBitCount[2]; // bits per pixel
	char biCompression[4]; // type of compression (0 is no compression)
	char biSizeImage[4]; // the origin size of the bitmap data (before compression)
	char biXPelsPerMeter[4]; // horizontal pixels per meter
	char biYPelsPerMeter[4]; // vertical pixels per meter
	char biClrUsed[4]; // the number of colors used
	char biClrImportant[4]; // "important" colors, usually 0
} BitmapInfoHeader;

typedef enum
{
	TYPE_COLLISION,
	TYPE_ROAD,
}OBJECTTYPE;

#endif