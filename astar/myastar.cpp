#include "myastar.h"

constexpr int kStepValue = 10;
constexpr int kObliqueValue = 14;

MyAStar::MyAStar(BlockAllocator* allocator)
	: width_(0)
	, height_(0)
	, allocator_(allocator)
	, step_val_(kStepValue)
	, oblique_val_(kObliqueValue)
{
}

MyAStar::~MyAStar()
{
	clear();
}

constexpr int MyAStar::get_step_value() const
{
	return step_val_;
}

constexpr int MyAStar::get_oblique_value() const
{
	return oblique_val_;
}

void MyAStar::clear()
{
	int index = 0;
	const int max_size = width_ * height_;
	while ((index) < (max_size))
	{
		allocator_->free(mapping_[index++], sizeof(Node));
	}
	open_list_.clear();
	can_pass_ = nullptr;
	width_ = height_ = 0;
}

void MyAStar::init(const MyParams& param)
{
	width_ = param.width;
	height_ = param.height;
	can_pass_ = param.can_pass;
	mapping_.clear();
	mapping_.resize(width_ * height_);
	memset(&mapping_[0], 0, sizeof(Node*) * mapping_.size());
}

const bool MyAStar::is_vlid_params(const MyParams& param) const
{
	return (((param.can_pass) != nullptr)
		&& (((param.width) > 0) && ((param.height) > 0))
		&& (((param.end.x()) >= 0) && ((param.end.x()) < (param.width)))
		&& (((param.end.y()) >= 0) && ((param.end.y()) < (param.height)))
		&& (((param.start.x()) >= 0) && ((param.start.x()) < (param.width)))
		&& (((param.start.y()) >= 0) && ((param.start.y()) < (param.height)))
		);
}

const bool MyAStar::get_node_index(Node*& node, int* index)  const
{
	*index = 0;
	const int size = static_cast<int>(open_list_.size());
	while (*index < (size))
	{
		if (open_list_[*index]->pos == node->pos)
		{
			return true;
		}
		++(*index);
	}
	return false;
}

constexpr void MyAStar::percolate_up(int hole)
{
	int parent = 0;
	while ((hole) > 0)
	{
		parent = ((hole)-1) / 2;
		if (open_list_[hole]->f() < open_list_[parent]->f())
		{
			std::ranges::swap(open_list_[hole], open_list_[parent]);
			hole = parent;
		}
		else
		{
			return;
		}
	}
}

__forceinline const int MyAStar::calcul_g_value(Node* parent, const MyPoint& current) const
{
	int g_value = (current - parent->pos).manhattanLength() == 2 ? oblique_val_ : step_val_;
	return g_value += parent->g;
}

__forceinline const int MyAStar::calcul_h_value(const MyPoint& current, const MyPoint& end) const
{
	int h_value = (end - current).manhattanLength();
	return h_value * step_val_;
}

__forceinline constexpr bool MyAStar::in_open_list(const MyPoint& pos, Node*& out_node) const
{
	out_node = mapping_[pos.y() * width_ + pos.x()];
	return out_node ? (out_node->state == IN_OPENLIST) : false;
}

__forceinline constexpr bool MyAStar::in_closed_list(const MyPoint& pos) const
{
	Node* node_ptr = mapping_[pos.y() * width_ + pos.x()];
	return node_ptr ? (node_ptr->state == IN_CLOSEDLIST) : false;
}

bool MyAStar::can_pass(const MyPoint& pos)
{
	return (((pos.x()) >= 0) && ((pos.x()) < (width_)) && ((pos.y()) >= 0) && ((pos.y()) < height_)) ? can_pass_(pos) : false;
}

bool MyAStar::can_pass(const MyPoint& current, const MyPoint& destination, const bool allow_corner)
{
	if (((destination.x()) >= 0) && ((destination.x()) < (width_)) && ((destination.y()) >= 0) && ((destination.y()) < (height_)))
	{
		if (in_closed_list(destination))
		{
			return false;
		}

		if ((destination - current).manhattanLength() == 1)
		{
			return can_pass_(destination);
		}
		else if (allow_corner)
		{
			return can_pass_(destination) &&
				can_pass(MyPoint{ static_cast<int>(current.x() + destination.x() - current.x()), current.y() }) &&
				can_pass(MyPoint{ current.x(), static_cast<int>(current.y() + destination.y() - current.y()) });
		}
	}
	return false;
}

void MyAStar::find_can_pass_nodes(const MyPoint& current, const bool corner, std::vector<MyPoint>* out_lists)
{
	MyPoint destination = {};
	int row_index = current.y() - 1;
	const int max_row = current.y() + 1;
	const int max_col = current.x() + 1;

	if (row_index < 0)
	{
		row_index = 0;
	}

	while ((row_index) <= (max_row))
	{
		int col_index = current.x() - 1;

		if (col_index < 0)
		{
			col_index = 0;
		}

		while ((col_index) <= (max_col))
		{
			destination.reset(col_index, row_index);
			if (can_pass(current, destination, corner))
			{
				out_lists->push_back(destination);
			}
			++col_index;
		}
		++row_index;
	}
}

void MyAStar::handle_found_node(Node*& current, Node*& destination)
{
	int g_value = calcul_g_value(current, destination->pos);
	if ((g_value) < (destination->g))
	{
		destination->g = g_value;
		destination->parent = current;

		int index = 0;
		if (get_node_index(destination, &index)) [[likely]]
		{
			percolate_up(index);
		}
		else [[unlikely]]
		{
			assert(false);
		}
	}
}

void MyAStar::handle_not_found_node(Node*& current, Node*& destination, const MyPoint& end)
{
	destination->parent = current;
	destination->h = calcul_h_value(destination->pos, end);
	destination->g = calcul_g_value(current, destination->pos);

	Node*& reference_node = mapping_[destination->pos.y() * width_ + destination->pos.x()];
	reference_node = destination;
	reference_node->state = IN_OPENLIST;

	open_list_.push_back(destination);
	std::ranges::push_heap(open_list_, [](const Node* a, const Node* b)->bool
		{
			return a->f() > b->f();
		});
}

bool MyAStar::find(const MyParams& param, std::vector<MyPoint>* path)
{
	if (!is_vlid_params(param))
	{
		return false;
	}

	// initialize
	init(param);

	std::vector<MyPoint> nearby_nodes;
	nearby_nodes.reserve(param.corner ? 8 : 4);

	// put the start node into the open list
	Node* start_node = new (allocator_->allocate(sizeof(Node))) Node(param.start);
	open_list_.push_back(start_node);
	Node*& reference_node = mapping_[((start_node->pos.y()) * (width_)) + start_node->pos.x()];
	reference_node = start_node;
	reference_node->state = IN_OPENLIST;

	// searching for the path
	while (!open_list_.empty())
	{
		// pop the node with the lowest f value
		Node* current = open_list_.front();
		std::ranges::pop_heap(open_list_, [](const Node* a, const Node* b)->bool
			{
				return a->f() > b->f();
			});

		open_list_.pop_back();
		mapping_[((current->pos.y()) * (width_)) + (current->pos.x())]->state = NodeState::IN_CLOSEDLIST;

		// is the destination found?
		if ((current->pos) == (param.end))
		{
			while (current->parent)
			{
				path->push_back(current->pos);
				current = current->parent;
			}
			std::ranges::reverse(*path);
			clear();
			return true;
		}

		// find the nearby nodes that can be passed
		nearby_nodes.clear();
		find_can_pass_nodes(current->pos, param.corner, &nearby_nodes);

		// calculate the cost value of the nearby nodes
		size_t index = 0;
		const size_t size = nearby_nodes.size();
		while ((index) < (size))
		{
			Node* next_node = nullptr;
			if (in_open_list(nearby_nodes[index], next_node))
			{
				handle_found_node(current, next_node);
			}
			else
			{
				next_node = new (allocator_->allocate(sizeof(Node))) Node(nearby_nodes[index]);
				handle_not_found_node(current, next_node, param.end);
			}
			++index;
		}
	}

	clear();
	return false;
}