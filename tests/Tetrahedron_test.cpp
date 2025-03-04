/// Causal Dynamical Triangulations in C++ using CGAL
///
/// Copyright © 2017-2021 Adam Getchell
///
/// Tests that 3D triangulated and foliated tetrahedrons are constructed
/// correctly.
///
/// @file Tetrahedron_test.cpp
/// @brief Tests for 3D triangulated and foliated tetrahedrons
/// @author Adam Getchell

#include "Foliated_triangulation.hpp"
#include <catch2/catch.hpp>

using namespace std;

static inline double const RADIUS_2 = std::sqrt(4.0 / 3.0);  // NOLINT

SCENARIO("Construct a tetrahedron in a Delaunay triangulation", "[tetrahedron]")
{
  GIVEN("A vector of 4 vertices.")
  {
    vector<Point> Vertices{Point{0, 0, 0}, Point{0, 1, 0}, Point{0, 0, 1},
                           Point{1, 0, 0}};
    WHEN("A triangulation is constructed using the vector.")
    {
      Delaunay3 triangulation;
      triangulation.insert(Vertices.begin(), Vertices.end());

      THEN("The triangulation has dimension 3.")
      {
        REQUIRE(triangulation.dimension() == 3);
      }

      THEN("The triangulation has 4 vertices.")
      {
        REQUIRE(triangulation.number_of_vertices() == 4);
      }

      THEN("The triangulation has 6 edges.")
      {
        REQUIRE(triangulation.number_of_finite_edges() == 6);
      }

      THEN("The triangulation has 4 faces.")
      {
        REQUIRE(triangulation.number_of_finite_facets() == 4);
      }

      THEN("The triangulation has 1 cell.")
      {
        REQUIRE(triangulation.number_of_finite_cells() == 1);
      }

      THEN("The triangulation is Delaunay.")
      {
        REQUIRE(triangulation.is_valid());
      }

      THEN("The triangulation data structure is valid.")
      {
        REQUIRE(triangulation.tds().is_valid());
      }
    }
  }
}

SCENARIO("Find distances between points of the tetrahedron", "[tetrahedron]")
{
  GIVEN("Points in a tetrahedron.")
  {
    auto origin = Point{0, 0, 0};
    // These points have a radius of 1
    auto            v1 = Point{1, 0, 0};
    auto            v2 = Point{0, 1, 0};
    auto            v3 = Point{0, 0, 1};
    auto            v4 = Point{RADIUS_2, RADIUS_2, RADIUS_2};
    Causal_vertices cv;
    cv.emplace_back(make_pair(v1, 1));
    cv.emplace_back(make_pair(v2, 1));
    cv.emplace_back(make_pair(v3, 1));
    cv.emplace_back(make_pair(v4, 2));
    WHEN("The Foliated triangulation is constructed with these points.")
    {
      FoliatedTriangulation3                   ft(cv);
      FoliatedTriangulation3::squared_distance r2;
      THEN("The triangulation is initialized correctly.")
      {
        REQUIRE(ft.is_initialized());
      }
      THEN("The squared distances of vertices from origin are correct.")
      {
        fmt::print("v1 is {}\n", v1);
        fmt::print("v2 is {}\n", v2);
        fmt::print("v3 is {}\n", v3);
        fmt::print("v4 is {}\n", v4);

        auto d1 = r2(origin, v1);
        fmt::print("The squared distance between v1 and the origin is {}\n",
                   d1);
        CHECK(d1 == 1);

        auto d2 = r2(origin, v2);
        fmt::print("The squared distance between v2 and the origin is {}\n",
                   d2);
        CHECK(d2 == 1);

        auto d3 = r2(origin, v3);
        fmt::print("The squared distance between v3 and the origin is {}\n",
                   d3);
        CHECK(d3 == 1);

        auto d4 = r2(origin, v4);
        fmt::print("The squared distance between v4 and the origin is {}\n",
                   d4);
        CHECK(d4 == 4);
      }
      THEN("The squared distance between radius=1 vertices are 2.")
      {
        auto d1 = r2(v1, v2);
        CHECK(d1 == 2);
        fmt::print("The squared distance between v1 and v2 is {}\n", d1);
        auto d2 = r2(v1, v3);
        CHECK(d2 == 2);
        fmt::print("The squared distance between v1 and v3 is {}\n", d2);
        auto d3 = r2(v2, v3);
        CHECK(d3 == 2);
        fmt::print("The squared distance between v2 and v3 is {}\n", d3);
      }
      THEN("All vertices have correct timevalues.")
      {
        auto vertices = ft.get_vertices();
        CHECK(ft.check_vertices(vertices));
        // Human verification
        for (auto& vertex : vertices)
        {
          fmt::print(
              "Vertex ({}) with timevalue of {} has a squared radius of {} and "
              "a "
              "squared expected radius of {} with an expected timevalue of "
              "{}.\n",
              vertex->point(), vertex->info(),
              FoliatedTriangulation3::squared_radius(vertex),
              std::pow(ft.expected_radius(vertex), 2),
              ft.expected_timevalue(vertex));
        }
      }
    }
  }
}

SCENARIO("Construct a foliated tetrahedron in a foliated triangulation",
         "[tetrahedron]")
{
  GIVEN("A vector of vertices and a vector of timevalues.")
  {
    vector<Point>       Vertices{Point{1, 0, 0}, Point{0, 1, 0}, Point{0, 0, 1},
                           Point{RADIUS_2, RADIUS_2, RADIUS_2}};
    vector<std::size_t> timevalue{1, 1, 1, 2};

    WHEN("A foliated triangulation is constructed using the vectors.")
    {
      // This is a complicated way to make Causal_vertices but is left
      // here for reference
      Causal_vertices cv;
      cv.reserve(Vertices.size());
      std::transform(Vertices.begin(), Vertices.end(), timevalue.begin(),
                     std::back_inserter(cv), [](Point a, std::size_t b) {
                       return std::make_pair(a, b);
                     });
      FoliatedTriangulation3 ft(cv);

      THEN("The triangulation is initialized correctly.")
      {
        REQUIRE(ft.is_initialized());
      }

      THEN("The triangulation has dimension 3.")
      {
        REQUIRE(ft.dimension() == 3);
      }

      THEN("The triangulation has 4 vertices.")
      {
        REQUIRE(ft.number_of_vertices() == 4);
      }

      THEN("The triangulation has 6 edges.")
      {
        REQUIRE(ft.number_of_finite_edges() == 6);
      }

      THEN("The triangulation has 4 faces.")
      {
        REQUIRE(ft.number_of_finite_facets() == 4);
      }

      THEN("The triangulation has 1 cell.")
      {
        REQUIRE(ft.number_of_finite_cells() == 1);
      }

      THEN("Timevalues are correct.")
      {
        auto checked_vertices = ft.get_vertices();
        REQUIRE(ft.check_vertices(checked_vertices));
        for (auto& vertex : checked_vertices)
        {
          CHECK(ft.is_vertex_timevalue_correct(vertex));
        }
      }

      THEN("The cell info is correct.")
      {
        for (auto&& cit = ft.finite_cells_begin(); cit != ft.finite_cells_end();
             ++cit)
        {
          REQUIRE(cit->info() == static_cast<int>(Cell_type::THREE_ONE));
        }
        // Human verification
        ft.print_cells();
      }

      THEN("There is one (3,1) simplex.")
      {
        REQUIRE(ft.get_three_one().size() == 1);
      }

      THEN("There are no (2,2) simplices.")
      {
        REQUIRE(ft.get_two_two().empty());
      }

      THEN("There are no (1,3) simplices.")
      {
        REQUIRE(ft.get_one_three().empty());
      }

      THEN("There are 3 timelike edges.") { REQUIRE(ft.N1_TL() == 3); }

      THEN("There are 3 spacelike edges.") { REQUIRE(ft.N1_SL() == 3); }
    }
  }
}
