#include "RGBOctree.hpp"
#include <iostream>

RGBOctree::RGBOctree(uint32_t _depth)
{
	depth = _depth;

	refs = 0;
	r = 0;
	g = 0;
	b = 0;
	parent = nullptr;
}

bool RGBOctree::IsLeaf()
{
	for (auto&c : children) {
		if (c != nullptr) {
			return false;
		}
	}
	return true;
}

std::vector<RGBOctree*> RGBOctree::GetAllNodes()
{
	std::vector<RGBOctree*> nodes;
	nodes.push_back(this);

	for (auto& c : children) {
		if (c != nullptr) {
			auto v = c->GetAllNodes();
			std::move(v.begin(), v.end(), std::back_inserter(nodes));
		}
	}
	return nodes;
}

uint32_t RGBOctree::CountLeaves()
{
	return GetLeaves().size();
}

std::vector<RGBOctree*> RGBOctree::GetLeaves()
{
	if (IsLeaf()) {
		return {this};
	}
	std::vector<RGBOctree*> leaves;

	for (const auto& c : children) {
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

	for (auto& c : children) {
		if (c != nullptr && !c->IsLeaf()) {
			auto v = c->GetLeafParents();
			std::move(v.begin(), v.end(), std::back_inserter(leaf_parents));
		}
	}
	return leaf_parents;
}

void RGBOctree::Fold()
{
	parent->r += r;
	parent->g += g;
	parent->b += b;
	parent->refs += refs;
}

void RGBOctree::ReduceDepth()
{
	for (auto& c : children) {
		if (c != nullptr) {
			if (c->IsLeaf()) {
				c->Fold();
				c.release();
			} else {
				c->ReduceDepth();
			}
		}
	}
}

RGBOctree* RGBOctree::Insert(const RGBPixel& p, uint8_t max_depth)
{
	if (depth >= max_depth || depth >= 7) {
		r += p[0];
		g += p[1];
		b += p[2];
		refs++;
		return this;
	}

	uint8_t bit_mask = 1 << (7 - depth);

	uint8_t i = (!!(p[0] & bit_mask)) * 4 +
							(!!(p[1] & bit_mask)) * 2 +
							(!!(p[2] & bit_mask));

	if (children[i] == nullptr) {
			children[i] = std::make_unique<RGBOctree>(depth + 1);
			children[i]->parent = this;
	}
	return children[i]->Insert(p, max_depth);
}
