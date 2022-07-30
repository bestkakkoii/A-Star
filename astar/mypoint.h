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
#ifndef MYPOINT_H
#define MYPOINT_H
#include "myglobal.hpp"

struct MyPoint
{
public:

	MyPoint() : xp(0), yp(0) {};
	MyPoint(int xpos, int ypos);

	bool isNull() const;

	int x() const;
	int y() const;

	void reset(int x1, int y1);

	int manhattanLength() const;

	POINT toPoint() const;

	bool operator== (const MyPoint& other) const
	{
		return x() == other.x() && y() == other.y();
	}

	bool operator!= (const MyPoint& other) const
	{
		return x() != other.x() || y() != other.y();
	}

	MyPoint operator- (const MyPoint& other) const
	{
		return MyPoint(x() - other.x(), y() - other.y());
	}

private:

	int xp;
	int yp;
};

struct KeyHash {
	std::size_t operator()(const MyPoint& k) const
	{
		return std::hash<size_t>()(k.x() * 10000 + k.y());
	}
};

struct KeyEqual {
	bool operator()(const MyPoint& lhs, const MyPoint& rhs) const
	{
		return lhs == rhs;
	}
};

typedef struct tagMyMap
{
	int width = 0;
	int height = 0;
	std::unordered_map<MyPoint, OBJECTTYPE, KeyHash, KeyEqual> data = {};
}MyMap;

// path node state
typedef enum
{
	NOTEXIST,               // not exist
	IN_OPENLIST,            // in the open list
	IN_CLOSEDLIST           // in the closed list
}NodeState;

// path node
struct Node
{
	int    g;               // distance from startpoint
	int    h;               // distance to endpoint
	MyPoint     pos;        // node position
	NodeState   state;      // node state in openlist or closelist
	Node* parent;           // parent node ptr

	// calculate f value
	MY_REQUIRED_RESULT constexpr int f() const
	{
		return g + h;
	}

	explicit Node()
		: g(0)
		, h(0)
		, pos(MyPoint{ 0, 0 })
		, parent(nullptr)
		, state(NodeState::NOTEXIST)
	{}

	explicit Node(const MyPoint& pos)
		: g(0)
		, h(0)
		, pos(pos)
		, parent(nullptr)
		, state(NodeState::NOTEXIST)
	{}
};

using Callback = std::function<bool(const MyPoint&)>;
struct MyParams
{
	bool corner;		 // either 8-dir or 4 dir
	int height;			 // map height
	int width;			 // map width
	MyPoint start;       // start point
	MyPoint end;         // end point
	Callback can_pass;   // std::function pointer check the sepicific point is whether passable or non-passable

	explicit MyParams()
		: height(0)
		, width(0)
		, corner(true)
		, start(MyPoint{ 0,0 })
		, end(MyPoint{ 0,0 })
		, can_pass(nullptr)
	{}

	explicit MyParams(const int w, const int h, bool cor, const MyPoint& start_point, const MyPoint& end_point, const Callback& fun)
		: height(h)
		, width(w)
		, corner(cor)
		, start(start_point)
		, end(end_point)
		, can_pass(fun)
	{}
};

// std::vector<MyPoint> to TC array format
static inline void MAKETCARRAY(const std::vector<MyPoint>& v, wchar_t*& path)
{
	const size_t len = v.size();
	std::wstring ss(TEXT("\0"));
	for (size_t i = 0; i < len; ++i)
	{
		if (i == 0)
			ss += TEXT("array(");

		if (i == (len - 1))// "0"=array("x"= 0,"y"= 0))
		{
			ss += std::format(TEXT(R"("{}"=array("x"={},"y"={})))"), i, v.at(i).x(), v.at(i).y());
		}

		else// "0" = array("x" = 0, "y" = 0),
		{
			ss += std::format(TEXT(R"("{}"=array("x"={},"y"={}),)"), i, v.at(i).x(), v.at(i).y());
		}
	}

	_snwprintf_s(path, (ss.size() * 2) + 1, _TRUNCATE, TEXT("%s\0"), ss.c_str());
}

// std::vector<MyPoint> to plain text format
static inline void MAKESTRARRAY(const std::vector<MyPoint>& v, wchar_t* path)
{
	const size_t len = v.size();
	std::wstring ss(TEXT("\0"));
	for (size_t i = 0; i < len; ++i)
	{
		if (i == (len - 1))
		{
			ss += std::format(TEXT("{},{}"), v.at(i).x(), v.at(i).y());
		}
		else
		{
			ss += std::format(TEXT("{},{}|"), v.at(i).x(), v.at(i).y());
		}
	}

	_snwprintf_s(path, (ss.size() * 2) + 1, _TRUNCATE, TEXT("%s\0"), ss.c_str());
}

#endif
