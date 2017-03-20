#include <iostream>
#include <string>
#include <getopt.h>

#include "RGBImage.hpp"
#include "RGBQuantize.hpp"

#include <gnuplot-iostream.h>
/* #include "RGBImageGPext.hpp" */

void PrintUsage(const std::string& name)
{
	std::cerr << "Usage: " << name << " [OPTION]... FILE\n"
		<< "Where FILE is a png or jpeg image.\n"
		<< "Options:\n"
		<< "\t-a, --algorithm ALGORITHM[,PARAM]\tthe quantisation method.\n"
		<< "\t-c, --colors NUM\t\t\tnumber of colors returned.\n"
		<< "\t-h, --help\t\t\t\tprints this usage message.\n"
		<< "\t-p, --plot\t\t\t\tplots the input image.\n"
		<< "\t-o, --output FILENAME\n"
		<< "\t-t, --terminal GP_TERM\t\t\tthe gnuplot terminal.\n"
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
	bool plot_image = false;
	uint32_t num_colors = 8;
	uint32_t algorithm_var = 4; // mmc iters, hist partions, etc.
	std::string algorithm = "mmc";
	std::string output = "gnuplot.png";
	std::string terminal = "pngcairo";

	// Arguments
	for (int i=1; i < argc - 1; ++i) {
		std::string arg = argv[i];
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
		} else if (arg == "-p" || arg == "--plot") {
			plot_image = true;
		} else if (arg == "-t" || arg == "--terminal") {
			if (i + 1 < argc) {
				terminal = std::string(argv[++i]);
			} else {
				std::cerr << "--terminal requires argument!" << std::endl;
				return 1;
			}
		} else {
			std::cerr << "Invalid option '" << arg << "'!" << std::endl;
			return 1;
		}
	}

	// Load image file
	RGBImage img;
	std::string img_file(argv[argc - 1]);
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
		colors = rgbq::ExtractColors_Octree(img, num_colors, algorithm_var);
	} else {
		std::cerr << "Invalid algorithm!" << std::endl;
		PrintUsage(argv[0]);
		return 1;
	}

	// Plot
	Gnuplot gp;
	gp << "set terminal " << terminal << " size 1000, 1000\n";
	gp << "set output '" << output << "'\n";
	gp << "set xlabel 'Red'\nset ylabel 'Green'\nset zlabel 'Blue'\n";
	gp << "set xrange [0:255]\nset yrange [0:255]\nset zrange [0:255]\n";
	gp << "set xyplane 0\n";
	gp << "rgb(r,g,b) = int(r)*65536 + int(g)*256 + int(b)\n";
	gp << "splot ";
	if (plot_image) {
		gp << "'-' binary" << gp.binFmt1d(img.pixels(), "record")
		   << "using 1:2:3:(rgb($1,$2,$3)) with points pt 5 lc rgb var notitle, ";
	}
	gp << "'-' binary" << gp.binFmt1d(colors, "record")
		 << "using 1:2:3:(rgb($1,$2,$3)) with points pt 7 ps 8 lc rgb var notitle\n";

	if (plot_image) {
		gp.sendBinary1d(img.pixels());
	}
	gp.sendBinary1d(colors);

	return 0;
}
