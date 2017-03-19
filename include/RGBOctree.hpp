#ifndef _RGB_OCTREE_H_
#define _RGB_OCTREE_H_

#include <cstdint>
#include <memory>

#include "RGBImage.hpp"

struct RGBOctree
{
	uint32_t refs;
	uint32_t depth;
	uint32_t r, g, b;
	RGBOctree* parent;
	std::array<std::unique_ptr<RGBOctree>, 8> children;

	RGBOctree(uint32_t _depth = 0);

	uint32_t CountLeaves();
	std::vector<RGBOctree*> GetLeaves();
	void Fold();
	void ReduceDepth();

	RGBOctree* Insert(const RGBPixel& p, uint8_t max_depth);
};

#endif /* ifndef _RGB_OCTREE_H_ */
