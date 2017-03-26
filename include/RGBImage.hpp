#ifndef _RGB_IMAGE_H_
#define _RGB_IMAGE_H_

#include <array>
#include <cstdint>
#include <string>
#include <vector>

typedef std::array<uint8_t, 3> RGBPixel;

class RGBImage
{
	private:
		std::vector<RGBPixel> _pixels;

		std::string _file_path;

		uint32_t _width;
		uint32_t _height;
		uint32_t _bit_depth;
		uint32_t _channels;

	public:
		RGBImage();
		~RGBImage();

		friend std::ostream& operator<<(
				std::ostream& os, const RGBImage& img);

		bool LoadPngFile (const std::string& file);
		bool LoadJpegFile(const std::string& file);

		const std::vector<RGBPixel>& pixels() { return _pixels; }
		const std::string& file_path() { return _file_path; }
		uint32_t width() { return _width; }
		uint32_t height() { return _height; }
		uint32_t bit_depth() { return _bit_depth; }
		uint32_t channels() { return _channels; }
};
#endif /* ifndef _RGB_IMAGE_H_ */
