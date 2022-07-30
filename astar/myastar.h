#pragma once
#ifndef MYASTAR_H
#define MYASTAR_H
#pragma execution_character_set("utf-8")
#include "mydraw.hpp"

class BlockAllocator;

class MyAStar
{
public:

public:
	explicit MyAStar(BlockAllocator* allocator);

	virtual ~MyAStar();

	// get the straight-line heuristic value
	MY_REQUIRED_RESULT constexpr int get_step_value() const;

	// get the corner-turn heuristic value
	MY_REQUIRED_RESULT constexpr int get_oblique_value() const;

	// execute the pathfinding operation
	MY_REQUIRED_RESULT bool __vectorcall find(const MyParams& param, std::vector<MyPoint>* path);

private:
	int                step_val_ = 10;
	int                oblique_val_ = 14;
	std::vector<Node*> mapping_;
	int                height_ = 0;
	int                width_ = 0;
	Callback           can_pass_ = nullptr;
	std::vector<Node*> open_list_;
	BlockAllocator* allocator_ = nullptr;

	// free data and unuse memory
	void clear();

	// initial data
	void __vectorcall init(const MyParams& param);

	// check a import parmas is valid or not
	MY_REQUIRED_RESULT const bool __vectorcall is_vlid_params(const MyParams& param) const;

	// binary heap filter
	constexpr void __vectorcall percolate_up(int hole);

	// get the index of the node
	MY_REQUIRED_RESULT const bool __vectorcall get_node_index(Node*& node, int* index) const;

	// calculate the cost of the node
	MY_REQUIRED_RESULT __forceinline const int __vectorcall calcul_g_value(Node* parent, const MyPoint& current) const;

	// calculate the heuristic value of the node
	MY_REQUIRED_RESULT __forceinline const int __vectorcall calcul_h_value(const MyPoint& current, const MyPoint& end) const;

	// check the point is in the open list or not, get node if it is
	MY_REQUIRED_RESULT __forceinline constexpr bool __vectorcall in_open_list(const MyPoint& pos, Node*& out_node)  const;

	// check the point is in the close list or not
	MY_REQUIRED_RESULT __forceinline constexpr bool __vectorcall in_closed_list(const MyPoint& pos) const;

	// check the point is passable
	MY_REQUIRED_RESULT bool __vectorcall can_pass(const MyPoint& pos);

	// check the point is passable and enable to reach the destination
	MY_REQUIRED_RESULT bool __vectorcall can_pass(const MyPoint& current, const MyPoint& destination, const bool allow_corner);

	// get the surrounding 4 or 8 nodes that can pass
	void __vectorcall find_can_pass_nodes(const MyPoint& current, const bool allow_corner, std::vector<MyPoint>* out_lists);

	// process the situation of finding the node
	void __vectorcall handle_found_node(Node*& current, Node*& destination);

	// process the situation of not finding the node
	void __vectorcall handle_not_found_node(Node*& current, Node*& destination, const MyPoint& end);
};

#endif
