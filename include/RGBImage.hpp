#ifndef _RGB_IMAGE_H_
#define _RGB_IMAGE_H_

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace rgb
{
	typedef std::array<uint8_t, 3> pixel;

	class Image
	{
		private:
			std::vector<pixel> _pixels;

			std::string _file_path;

			uint32_t _width;
			uint32_t _height;
			uint32_t _bit_depth;
			uint32_t _channels;

		public:
			Image();
			~Image();

			friend std::ostream& operator<<(
					std::ostream& os, const Image& img);

			bool loadPngFile (const std::string& file);
			bool loadJpegFile(const std::string& file);
			void loadPixelData(const std::vector<pixel>& pixels) {
				_pixels = pixels; }

			const std::vector<pixel>& pixels() { return _pixels; }
			const std::string& file_path() { return _file_path; }
			uint32_t width() { return _width; }
			uint32_t height() { return _height; }
			uint32_t bit_depth() { return _bit_depth; }
			uint32_t channels() { return _channels; }
	};
} /* namespace rgb */

#endif /* ifndef _RGB_IMAGE_H_ */
