#include "RGBOctree.hpp"
#include <iostream>

bool RGBOctree::IsLeaf()
{
	for (const auto& c : _children) {
		if (c != nullptr) {
			return false;
		}
	}
	return true;
}

void RGBOctree::Fold()
{
	_parent->_r += _r;
	_parent->_g += _g;
	_parent->_b += _b;
	_parent->_refs += _refs;
}

RGBOctree::RGBOctree(RGBOctree* parent, uint32_t depth)
	:_parent(parent), _depth(depth)
{
	_refs = 0;
	_r = 0;
	_g = 0;
	_b = 0;
}

std::vector<RGBOctree*> RGBOctree::GetAllChildren()
{
	std::vector<RGBOctree*> nodes;
	nodes.push_back(this);

	for (const auto& c : _children) {
		if (c != nullptr) {
			auto v = c->GetAllChildren();
			std::move(v.begin(), v.end(), std::back_inserter(nodes));
		}
	}
	return nodes;
}

std::vector<RGBOctree*> RGBOctree::GetLeaves()
{
	if (IsLeaf()) {
		return {this};
	}
	std::vector<RGBOctree*> leaves;

	for (const auto& c : _children) {
		if (c != nullptr) {
			auto v = c->GetLeaves();
			std::move(v.begin(), v.end(), std::back_inserter(leaves));
		}
	}
	return leaves;
}

std::vector<RGBOctree*> RGBOctree::GetLeafParents()
{
	std::vector<RGBOctree*> leaf_parents = {this};

	for (const auto& c : _children) {
		if (c != nullptr && !c->IsLeaf()) {
			auto v = c->GetLeafParents();
			std::move(v.begin(), v.end(), std::back_inserter(leaf_parents));
		}
	}
	return leaf_parents;
}

const RGBPixel RGBOctree::GetMeanColor()
{
	return {{static_cast<uint8_t>(_r / _refs),
	         static_cast<uint8_t>(_g / _refs),
	         static_cast<uint8_t>(_b / _refs)}};
}

void RGBOctree::ReduceDepth()
{
	for (auto& c : _children) {
		if (c != nullptr) {
			if (c->IsLeaf()) {
				c->Fold();
				c.reset();
			} else {
				c->ReduceDepth();
			}
		}
	}
}

RGBOctree* RGBOctree::Insert(const RGBPixel& p, uint8_t max_depth)
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
			_children[i] = std::make_unique<RGBOctree>(this, _depth + 1);
	}
	return _children[i]->Insert(p, max_depth);
}
