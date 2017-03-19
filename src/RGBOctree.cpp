#include "RGBOctree.hpp"

RGBOctree::RGBOctree(uint32_t _depth)
{
	depth = _depth;

	refs = 0;
	r = 0;
	g = 0;
	b = 0;
	parent = nullptr;
}

uint32_t RGBOctree::CountLeaves()
{
	auto leaves = GetLeaves();
	return leaves.size();
}

std::vector<RGBOctree*> RGBOctree::GetLeaves()
{
	std::vector<RGBOctree*> leaves;

	for (const auto& c : children) {
		if (c != nullptr) {
			leaves.emplace_back(c->GetLeaves());
		}
	}
	return leaves;
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
		if (c == nullptr) {
			c->Fold();
			c.release();
		} else {
			c->ReduceDepth();
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

	uint8_t i = (0x01 & (p[0] & bit_mask)) * 4 +
							(0x01 & (p[1] & bit_mask)) * 2 +
							(0x01 & (p[2] & bit_mask));

	if (children[i] == nullptr) {
			children[i] = std::make_unique<RGBOctree>(depth + 1);
			children[i]->parent = this;
	}
	return children[i]->Insert(p, max_depth);
}
