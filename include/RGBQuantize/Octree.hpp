#ifndef _RGB_QUANTIZE_OCTREE_H_
#define _RGB_QUANTIZE_OCTREE_H_

#include <array>
#include <memory>
#include <vector>

namespace rgb
{
	typedef std::array<uint8_t, 3> pixel;

	class Octree
	{
		private:
			Octree* _parent;

			uint32_t _refs, _depth;
			uint32_t _r, _g, _b;

			std::array<std::unique_ptr<Octree>, 8> _children;

			bool isLeaf();
			void fold();

		public:
			Octree(Octree* parent = nullptr, uint32_t depth = 0);

			std::vector<Octree*> getAllChildren();
			std::vector<Octree*> getLeaves();
			std::vector<Octree*> getLeafParents();
			const pixel getMeanColor();

			void reduceDepth();
			Octree* insert(const pixel& p, uint8_t max_depth);

			uint32_t refs() const { return _refs; }
	};

	std::vector<pixel> quantizeOctree(
			const std::vector<pixel>& pixels, uint32_t num_colors, uint32_t max_depth);
};

#endif /* ifndef _RGB_QUANTIZE_OCTREE_H_ */
