#ifndef _RGB_IMAGE_H_
#define _RGB_IMAGE_H_

#include <array>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

/* struct RGBPixel { */
/* 	RGBPixel() : r(0), g(0), b(0) {} */
/* 	RGBPixel(uint8_t _r, uint8_t _g, uint8_t _b) */
/* 		: r(_r), g(_g), b(_b) {} */

/* 	uint8_t r, g, b; */
/* }; */

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

		bool loadPngFile (const std::string& file);
		bool loadJpegFile(const std::string& file);

		const std::vector<RGBPixel>& pixels() { return _pixels; }

		const std::string& filePath() { return _file_path; }

		uint32_t width() { return _width; }
		uint32_t height() { return _height; }
		uint32_t bitDepth() { return _bit_depth; }
		uint32_t channels() { return _channels; }
};
#endif /* ifndef _RGB_IMAGE_H_ */
