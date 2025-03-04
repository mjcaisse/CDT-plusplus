/// Causal Dynamical Triangulations in C++ using CGAL
///
/// Copyright © 2015-2020 Adam Getchell
///
/// A program that visualizes spacetimes
///
/// Inspired by:
/// http://doc.cgal.org/latest/Geomview/Geomview_2gv_terrain_8cpp-example.html
///
/// @todo Parse file and read in points only
/// @todo Colorize/improve output

/// @file cdt-gv.cpp
/// @brief A pipeline from CDT++ to <a href="http://www.geomview.org">Geomview
/// </a>, a visualizer
/// @author Adam Getchell

// CGAL headers
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/Geomview_stream.h>
#include <CGAL/IO/Triangulation_geomview_ostream_3.h>
#include <CGAL/Projection_traits_xy_3.h>

// C++ headers
#include <fstream>
#include <gsl/gsl>

// Docopt
#include <docopt.h>

// V. Zverovich's {fmt} library
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "Settings.hpp"

using K        = CGAL::Exact_predicates_inexact_constructions_kernel;
using Delaunay = CGAL::Delaunay_triangulation_3<K>;
using Gt3      = CGAL::Projection_traits_xy_3<K>;
using Point3   = Gt3::Point;

using namespace std;

/// Help message parsed by docopt into options
static constexpr auto USAGE{
    R"(Causal Dynamical Triangulations in C++ using CGAL.

Copyright (c) 2015-2020 Adam Getchell

A program that loads files containing d-dimensional triangulated spacetimes
with a defined causal structure generated by cdt.cpp into a GeomView
pipeline for visualization.

The files should be in OFF (Object File Format).

Note that the standard output of CDT++ includes cell neighbors, and should
be truncated to just include points.

Usage:./cdt-gv --file FILE

Example:
./cdt --file points.off
./cdt --f points.off

Options:
  -h --help             Show this message
  --version             Show program version
  -f --file FILENAME    The file to load into GeomView
)"};

/// @brief The main path of the cdt-gv program
///
/// @param argc Argument count = 1 + number of arguments
/// @param argv Argument vector (array) to be passed to docopt
/// @return Integer value 0 if successful, 1 on failure
auto main(int argc, char const* argv[]) -> int
try
{
  // https://stackoverflow.com/questions/9371238/why-is-reading-lines-from-stdin-much-slower-in-c-than-python?rq=1
  ios_base::sync_with_stdio(false);
  // docopt option parser
  gsl::cstring_span<>        usage_string = gsl::ensure_z(USAGE);
  map<string, docopt::value> args =
      docopt::docopt(gsl::to_string(usage_string), {argv + 1, argv + argc},
                     true,           // print help message automatically
                     "cdt-gv 1.0");  // Version

  // Debugging docopt values
  // for (auto const& arg : args) {
  //   std::cout << arg.first << " " << arg.second << "\n";
  // }

  // Parse docopt::values in args map
  auto file = args["--file"].asString();

  // Test
  fmt::print("File to be loaded is {}\n", file);

  CGAL::Geomview_stream gv(CGAL::Bbox_3(
      -GV_BOUNDING_BOX_SIZE, -GV_BOUNDING_BOX_SIZE, -GV_BOUNDING_BOX_SIZE,
      GV_BOUNDING_BOX_SIZE, GV_BOUNDING_BOX_SIZE, GV_BOUNDING_BOX_SIZE));
  // Semmle doesn't like this line
  //  gv.set_line_width(4);
  int constexpr GV_RED_VALUE   = 0;
  int constexpr GV_GREEN_VALUE = 200;
  int constexpr GV_BLUE_VALUE  = 200;
  gv.set_bg_color(CGAL::Color(GV_RED_VALUE, GV_GREEN_VALUE, GV_BLUE_VALUE));

  Delaunay D;
  ifstream iFile(file, ios::in);
  Point3   p;

  // Insert points from file into Delaunay triangulation
  while (iFile >> p) { D.insert(p); }

  fmt::print("Drawing 3D Delaunay triangulation in wired mode.\n");
  gv.set_wired(true);
  gv << D;

  fmt::print("Enter any key to exit.\n");
#ifdef _WIN32
  system("pause")
#else
  system("read");
#endif

      return 0;
}
catch (invalid_argument& InvalidArgument)
{
  fmt::print(cerr, "{}\n", InvalidArgument.what());
  fmt::print(cerr, "Invalid parameter ... exiting.\n");
  return 1;
}
catch (...)
{
  fmt::print(cerr, "Something went wrong ... Exiting.\n");
  return 1;
}