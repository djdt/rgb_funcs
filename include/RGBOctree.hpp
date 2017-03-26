#ifndef _RGB_OCTREE_H_
#define _RGB_OCTREE_H_

#include <cstdint>
#include <vector>
#include <memory>

typedef std::array<uint8_t, 3> RGBPixel;

class RGBOctree
{
	private:
		RGBOctree* _parent;

		uint32_t _refs, _depth;
		uint32_t _r, _g, _b;

		std::array<std::unique_ptr<RGBOctree>, 8> _children;

		bool IsLeaf();
		void Fold();

	public:
		RGBOctree(RGBOctree* parent = nullptr, uint32_t depth = 0);

		std::vector<RGBOctree*> GetAllChildren();
		std::vector<RGBOctree*> GetLeaves();
		std::vector<RGBOctree*> GetLeafParents();
		const RGBPixel GetMeanColor();

		void ReduceDepth();
		RGBOctree* Insert(const RGBPixel& p, uint8_t max_depth);

		uint32_t refs() const { return _refs; }
};

#endif /* ifndef _RGB_OCTREE_H_ */
