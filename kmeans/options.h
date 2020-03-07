#pragma once

#include <string>
#include "point.h"

struct runtime_options
{
    std::string load_filename               =     "";
    // elt_type    generator_radius            =   1.0f;
    bool        generator_centers_randomize =   false;
    unsigned    generator_centers_count     =     15;
    elt_type    generator_centers_radius    =   3.0f;
    unsigned    generator_points_count      =  40000;
    bool        generator_points_sort       =   true;
    elt_type    generator_points_radius     =   1.0f;
    bool        kmeans_draw                 =   true;
    unsigned    kmeans_sleep_ms             =    1000;
    unsigned    kmeans_clusters             =      3;
    bool        kmeans_kpp = false;
    float       kmeans_stop_fct            =  0.01f;
    unsigned    kmeans_parallel_partitions  = 100000;
    unsigned    draw_parallel_partitions    = 100000;
};

int get_options(int argn, char const * argv[], runtime_options& ro);
