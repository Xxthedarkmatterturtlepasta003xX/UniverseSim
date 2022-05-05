#pragma once
#include <unordered_map>
#include <memory>
#include "Body.h"
#include "SpatialPartitioning.h"

class QuadTree : public SpatialPartitioning {
public:

	const float x;
	const float y;
	const float end_x;
	const float end_y;

	QuadTree(float size) : x(-size), y(-size), end_x(size), end_y(size)
	{}

	QuadTree(float x, float y, float end_x, float end_y) : x(x), y(y), end_x(end_x), end_y(end_y)
	{}

	void collision_check(std::vector<Body*>& to_remove);


	void add_body(Body& body);
	bool rem_body(const Body& body);
	bool is_leaf() const { return UL == nullptr; } // Non-leaf nodes always have all 4 quads.

	bool in_bounds(int check_x, int check_y) const {
		return check_x >= x && check_x < end_x&&
			check_y >= y && check_y < end_y;
	}

	const std::array<QuadTree*, 4> get_quads() const;

	float get_width() const { return end_x - x + 1; }
	float get_height() const { return end_y - y + 1; }

	void update();
	void draw_debug(const Camera2D& camera) const;

	~QuadTree() = default;

	//void update_removed(const std::vector<int> &indices_removed);


private:

	

	static constexpr int MAX_BODIES = 10;

	std::vector<Body*> quad_bodies;
	int cur_size = 0; // The number of bodies in this quad and its children.c

	QuadTree* parent = nullptr;
	std::unique_ptr<QuadTree> UL = nullptr;
	std::unique_ptr<QuadTree> UR = nullptr;
	std::unique_ptr<QuadTree> LL = nullptr;
	std::unique_ptr<QuadTree> LR = nullptr;

	int update_internal(); // returns change in number of bodies in a quad due to movement.
	void handle_collision(std::vector<Body*>::iterator& it, std::vector<Body*>& to_remove);

	bool is_root() const { return parent == nullptr; }

	int width() const { return end_x - x + 1; }
	int height() const { return end_y - y + 1; }


	// leaf node methods (doesn't really make sense for parent nodes)
	bool is_empty() const { return cur_size == 0; }
	bool has_room() const { return cur_size < MAX_BODIES; }
	bool is_full() const { return cur_size >= MAX_BODIES; }

	bool contains_point(Vector2 point) const;
	bool contains_fully(const Body& body) const;
	bool contains_partially(const Body& body) const;


	bool in_more_than_one_child(Body& body);
	void selective_add(Body& new_body); // Chooses how to add body to the quad.
	void add_to_child(Body& body);
	//void rem_from_child(const Body& body);

	void concatenate();
	void split();

	void reinsert(Body& body);

};
