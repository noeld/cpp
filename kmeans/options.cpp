#include "options.h"
#include <exception>
#include <iostream>

#include "point.h"
#include <boost/program_options.hpp>
#include<iterator>



namespace po = boost::program_options;

int get_options(int argn, char const * argv[], runtime_options& ro)
{
	try {
	  po::options_description desc("kmeans options");
	  desc.add_options()
	    ("help,h", "show this help")
	  	("file,f", po::value<std::string>(), "load from image file")
	  	("centers-random", "randomize number of centers")
	  	("centers-count", po::value<unsigned>(), "number of centers, or max number if randomized")
	  	("centers-radius", po::value<elt_type>(), "max distance of centers from 0,0")
	  	("points-count", po::value<unsigned>(), "number of points")
	  	("points-sort", "sort points before computation")
	  	("points-radius", po::value<float>(), "max radius of point around center point")
	  	("no-draw", "do not draw after each iteration")
	  	("sleep-ms", po::value<unsigned>(), "sleep (pause) after each iteration in milliseconds. (0 = none)")
	  	("stop-pcnt", po::value<float>(), "stop iterating when no more than this percentage of points changed their cluster")
	  	("parallel-partitions", po::value<unsigned>(), "parallel computation partition size (default 100000)")
	  	("parallel-draw-partitions", po::value<unsigned>(), "parallel drawing partition size (default 100000)")
		("clusters,k", po::value<unsigned>(), "k in k-means; the number of clusters")
		("init-kpp", "initialize with kmeans k-plus-plus, random initialization otherwise")
	  	;

	  	po::variables_map vm;       
	  	po::store(po::parse_command_line(argn, argv, desc), vm);
	  	po::notify(vm);   

	  	if (vm.count("help")) {
	  	    std::cout << desc << "\n";
	  	    return 1;
	  	}
		if (vm.count("file") > 0)
		{
			ro.load_filename = vm["file"].as<std::string>();
		}
		if (vm.count("centers-random") > 0)
		{
			ro.generator_centers_randomize = true;
		}
		if (vm.count("centers-count") > 0)
		{
			ro.generator_centers_randomize = false;
			ro.generator_centers_count = vm["centers-count"].as<unsigned>();
		}
		if (vm.count("centers-radius") > 0)
		{
			ro.generator_centers_radius = vm["centers-radius"].as<elt_type>();
		}
		if (vm.count("points-count") > 0)
		{
			ro.generator_points_count = vm["points-count"].as<unsigned>();
		}
		if (vm.count("points-sort") > 0)
		{
			ro.generator_points_sort = true;
		}
		if (vm.count("points-radius") > 0)
		{
			ro.generator_points_radius = vm["points-radius"].as<float>();
		}
		if (vm.count("no-draw") > 0)
		{
			ro.kmeans_draw = false;
		}
		if (vm.count("sleep-ms") > 0)
		{
			ro.kmeans_sleep_ms = vm["sleep-ms"].as<unsigned>();
		}
		if (vm.count("stop-pcnt") > 0)
		{
			float pcnt = vm["stop-pcnt"].as<float>();
			pcnt = std::clamp(pcnt, 0.0f, 100.0f);
			ro.kmeans_stop_fct = pcnt / 100.0f;
		}
		if (vm.count("parallel-partitions") > 0)
		{
			ro.kmeans_parallel_partitions = vm["parallel-partitions"].as<unsigned>();
		}
		if (vm.count("parallel-draw-partitions") > 0)
		{
			ro.draw_parallel_partitions = vm["parallel-draw-partitions"].as<unsigned>();
		}
		if (vm.count("clusters") > 0)
		{
			ro.kmeans_clusters = vm["clusters"].as<unsigned>();
		}
		if (vm.count("init-kpp") > 0)
		{
			ro.kmeans_kpp = true;
		}
	} catch (std::exception & e) {
		std::cout << "Parsing exception " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
