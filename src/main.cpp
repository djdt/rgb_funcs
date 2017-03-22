#include <iostream>
#include <string>

#include "RGBImage.hpp"
#include "RGBQuantize.hpp"

#include <gnuplot-iostream.h>
/* #include "RGBImageGPext.hpp" */

std::string GetColorHex(const RGBPixel& p)
{
	std::stringstream ss;
	for (const auto v : p) {
		ss << std::setfill('0') << std::setw(2)
			 << std::hex << static_cast<int>(v);
	}
	return ss.str();
}

void PrintUsage(const std::string& name)
{
	std::cerr << "Usage: " << name << " [OPTION]... FILE\n"
		<< "Where FILE is a png or jpeg image.\n"
		<< "Options:\n"
		<< "\t-a, --algorithm ALGORITHM[,PARAM]\tthe quantisation method.\n"
		<< "\t-c, --colors NUM\t\t\tnumber of colors returned.\n"
		<< "\t-h, --help\t\t\t\tprints this usage message.\n"
		<< "\t-i, --plot-image\t\t\tplots the input image.\n"
		<< "\t-p, --plot\t\t\t\tplots the extracted colors.\n"
		<< "\t-o, --output FILENAME\n"
		<< "\t-s, --sort\t\t\t\tsorts the image data before plotting.\n"
		<< "\t-t, --terminal GP_TERM\t\t\tthe gnuplot terminal.\n"
		<< "\t-w, --write\t\t\t\twrites to output filename.\n"
		<< "ALGORITHM: eg. mmc|mmc,4|hist|octree,8\n"
		<< "\tValid algorithms are mmc, hist, octree and kmeans.\n"
		<< "\tPass aditional params such as median cut iterations,\n"
		<< "\thistogram partitions and octree depth using comma.\n"
		<< std::endl;
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		PrintUsage(argv[0]);
		return 1;
	}

	// Defaults
	bool plot_colors       = false;
	bool plot_image        = false;
	bool sort_image        = false;
	uint32_t num_colors    = 8;
	uint32_t algorithm_var = 5; // mmc iters, hist partions, etc.
	std::string algorithm  = "octree";
	std::string output     = "";
	std::string terminal   = "pngcairo";
	std::vector<std::string> sources;

	// Arguments
	for (int i=1; i < argc; ++i) {
		std::string arg(argv[i]);

		if (arg == "-h" || arg == "--help") {
			PrintUsage(argv[0]);
			return 0;
		} else if (arg == "-a" || arg == "--algorithm") {
			if (i + 1 < argc) {
				algorithm = std::string(argv[++i]);
				auto comma = algorithm.find_first_of(',');
				if (comma < algorithm.length()) {
					algorithm_var = std::stoi(algorithm.substr(comma + 1));
					algorithm = algorithm.substr(0, comma);
				}
			} else {
				std::cerr << "--algorithm requires argument!" << std::endl;
				return 1;
			}
		} else if (arg == "-c" || arg == "--colors") {
			if (i + 1 < argc) {
				num_colors = std::stoi(argv[++i]);
			} else {
				std::cerr << "--colors requires argument!" << std::endl;
				return 1;
			}
		} else if (arg == "-o" || arg == "--output") {
			if (i + 1 < argc) {
				output = std::string(argv[++i]);
			} else {
				std::cerr << "--output requires argument!" << std::endl;
				return 1;
			}
		} else if (arg == "-i" || arg == "--plot-image") {
			plot_image = true;
		} else if (arg == "-p" || arg == "--plot") {
			plot_colors = true;
		} else if (arg == "-s" || arg == "--sort") {
			sort_image = true;
		} else if (arg == "-t" || arg == "--terminal") {
			if (i + 1 < argc) {
				terminal = std::string(argv[++i]);
			} else {
				std::cerr << "--terminal requires argument!" << std::endl;
				return 1;
			}
		} else {
			sources.push_back(arg);
		}
	}

	if (output == "") output = algorithm + ".png";

	// Load image file
	RGBImage img;
	std::string img_file(sources[0]);
	std::string file_ext = img_file.substr(
			img_file.find_last_of('.') + 1);

	if (file_ext == "png") {
		if(!img.LoadPngFile(img_file)) return 1;
	} else if (file_ext == "jpg" || file_ext == "jpeg") {
		if(!img.LoadJpegFile(img_file)) return 1;
	} else {
		std::cerr << "Invalied file extenison!" << std::endl;
		PrintUsage(argv[0]);
		return 1;
	}

	std::cout << img << std::endl;

	// Extract colors
	std::cout << "Extracting colors..." << std::endl;
	std::vector<RGBPixel> colors;

	if (algorithm == "mmc") {
		if (num_colors > std::pow(2, algorithm_var)) {
			std::cerr << "Warning: can only return "
				        << std::pow(2, algorithm_var)
				        << " (2 ^ mmc iterations) colors!" << std::endl;
		}
		colors = rgbq::ExtractColors_MedianCut(img, num_colors, algorithm_var);
	} else if (algorithm == "hist") {
		if (num_colors > std::pow(algorithm_var, 3)) {
			std::cerr << "Warning: can only return "
				        << std::pow(algorithm_var, 3)
				        << " (hist partitions ^ 3) colors!" << std::endl;
		}
		colors = rgbq::ExtractColors_Histogram(img, num_colors, algorithm_var);
	} else if (algorithm == "kmeans") {
		colors = rgbq::ExtractColors_KMeans(img, num_colors);
	} else if (algorithm == "octree") {
		if (num_colors > std::pow(algorithm_var, 8)) {
			std::cerr << "Warning: can only return "
				        << std::pow(algorithm_var, 8)
				        << " (octree depth ^ 8) colors!" << std::endl;
		}
		colors = rgbq::ExtractColors_Octree(img, num_colors, algorithm_var);
	} else {
		std::cerr << "Invalid algorithm!" << std::endl;
		PrintUsage(argv[0]);
		return 1;
	}

	// Sorting
	std::vector<RGBPixel> pix = img.pixels();

	if (sort_image) {
		std::cout << "Sorting pixles..." << std::endl;
		std::sort(pix.begin(), pix.end(),
				[] (const RGBPixel& a, const RGBPixel& b) {
					return (a[0] != b[0]) ? a[0] < b[0] : a[1] < b[1];
				});
	}

	if (plot_colors || plot_image) {
		std::cout << "Plotting..." << std::endl;

		Gnuplot gp;
		gp << "set terminal " << terminal << " size 1000, 1000\n"
			 << "set output '" << output << "'\n"
			 << "set xlabel 'Red'\nset ylabel 'Green'\nset zlabel 'Blue'\n"
			 << "set xrange [0:255]\nset yrange [0:255]\nset zrange [0:255]\n"
			 << "set xyplane 0\n"
			 << "rgb(r,g,b) = int(r)*65536 + int(g)*256 + int(b)\n"
			 << "splot ";

		if (plot_image) {
			gp << gp.binFile1d(pix, "record")
				 << "u 1:2:3:(rgb($1,$2,$3)) with points pt 5"
				 <<	"lc rgb var notitle, ";
		}
		if (plot_colors) {
			gp << gp.binFile1d(colors, "record") << "u 1:2:3:(rgb($1,$2,$3)) "
				 << "w points pt 7 ps 7 lc rgb var notitle, ";
			gp << gp.binFile1d(colors, "record") << "u 1:2:3 "
				 << "w points pt 6 ps 7 lc 0 lw 2 notitle, ";
			gp << gp.binFile1d(colors, "record") << "u 1:2:3:(sprintf("
				 << "'0x%x',rgb($1,$2,$3))) w labels left offset 3 notitle";
		}
		gp << "\n";
	}

	/* for (auto c : colors) { */
	/* 	std::cout << GetColorHex(c) << std::endl; */
	/* } */

	return 0;
}
