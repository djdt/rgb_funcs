#include "RGBQuantize/Octree.hpp"

#include <algorithm>

bool rgb::Octree::isLeaf()
{
	for (const auto& c : _children) {
		if (c != nullptr) {
			return false;
		}
	}
	return true;
}

void rgb::Octree::fold()
{
	_parent->_r += _r;
	_parent->_g += _g;
	_parent->_b += _b;
	_parent->_refs += _refs;
}

rgb::Octree::Octree(rgb::Octree* parent, uint32_t depth)
	:_parent(parent), _depth(depth)
{
	_refs = 0;
	_r = 0;
	_g = 0;
	_b = 0;
}

std::vector<rgb::Octree*> rgb::Octree::getAllChildren()
{
	std::vector<rgb::Octree*> nodes;
	nodes.push_back(this);

	for (const auto& c : _children) {
		if (c != nullptr) {
			auto v = c->getAllChildren();
			std::move(v.begin(), v.end(), std::back_inserter(nodes));
		}
	}
	return nodes;
}

std::vector<rgb::Octree*> rgb::Octree::getLeaves()
{
	if (this->isLeaf()) {
		return {this};
	}
	std::vector<rgb::Octree*> leaves;

	for (const auto& c : _children) {
		if (c != nullptr) {
			auto v = c->getLeaves();
			std::move(v.begin(), v.end(), std::back_inserter(leaves));
		}
	}
	return leaves;
}

std::vector<rgb::Octree*> rgb::Octree::getLeafParents()
{
	std::vector<rgb::Octree*> leaf_parents = {this};

	for (const auto& c : _children) {
		if (c != nullptr && !c->isLeaf()) {
			auto v = c->getLeafParents();
			std::move(v.begin(), v.end(), std::back_inserter(leaf_parents));
		}
	}
	return leaf_parents;
}

const rgb::pixel rgb::Octree::getMeanColor()
{
	return {{static_cast<uint8_t>(_r / _refs),
	         static_cast<uint8_t>(_g / _refs),
	         static_cast<uint8_t>(_b / _refs)}};
}

void rgb::Octree::reduceDepth()
{
	for (auto& c : _children) {
		if (c != nullptr) {
			if (c->isLeaf()) {
				c->fold();
				c.reset();
			} else {
				c->reduceDepth();
			}
		}
	}
}

rgb::Octree* rgb::Octree::insert(const rgb::pixel& p, uint8_t max_depth)
{
	if (_depth >= max_depth || _depth >= 7) {
		_r += p[0];
		_g += p[1];
		_b += p[2];
		_refs++;
		return this;
	}

	uint8_t bit_mask = 1 << (7 - _depth);
	uint8_t i = (!!(p[0] & bit_mask)) * 4 +
							(!!(p[1] & bit_mask)) * 2 +
							(!!(p[2] & bit_mask));

	if (_children[i] == nullptr) {
			_children[i] = std::make_unique<rgb::Octree>(this, _depth + 1);
	}
	return _children[i]->insert(p, max_depth);
}

std::vector<rgb::pixel> rgb::quantizeOctree(
		const std::vector<rgb::pixel>& pixels, uint32_t num_colors, uint32_t max_depth)
{
	rgb::Octree octree;

	for (auto p : pixels) {
		octree.insert(p, max_depth);
	}

	while (octree.getLeafParents().size() > num_colors) {
		octree.reduceDepth();
	}

	// Sort octree nodes by number of refs
	std::vector<rgb::Octree*> buckets = octree.getLeaves();
	if (buckets.size() > num_colors) {
		std::sort(buckets.begin(), buckets.end(),[]
				(const rgb::Octree* a, const rgb::Octree* b) {
					return (a->refs() < b->refs());
				});
		buckets.erase(buckets.begin(), buckets.end() - num_colors);
	}

	// Return average of remaining leaves
	std::vector<rgb::pixel> colors;
	for (auto b : buckets) {
		colors.push_back(b->getMeanColor());
	}

	return colors;
}
