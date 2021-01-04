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

#include <Manifold.hpp>
#include <catch2/catch.hpp>

using namespace std;

SCENARIO("Construct a tetrahedron in a Delaunay triangulation", "[tetrahedron]")
{
  GIVEN("A vector of 4 vertices.")
  {
    vector<Delaunay3::Point> Vertices{
        Delaunay3::Point{0, 0, 0}, Delaunay3::Point{0, 1, 0},
        Delaunay3::Point{0, 0, 1}, Delaunay3::Point{1, 0, 0}};
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

SCENARIO("Construct a foliated tetrahedron in a foliated triangulation",
         "[tetrahedron]")
{
  GIVEN("A vector of vertices and a vector of timevalues.")
  {
    vector<Delaunay3::Point> Vertices{
        Delaunay3::Point{0, 0, 0}, Delaunay3::Point{0, 1, 0},
        Delaunay3::Point{1, 0, 0}, Delaunay3::Point{0, 0, 1}};
    vector<std::size_t> timevalue{1, 1, 1, 2};

    WHEN("A foliated triangulation is constructed using the vectors.")
    {
      Causal_vertices causal_vertices;
      causal_vertices.reserve(Vertices.size());
      std::transform(Vertices.begin(), Vertices.end(), timevalue.begin(),
                     std::back_inserter(causal_vertices),
                     [](Delaunay3::Point a, std::size_t b) {
                       return std::make_pair(a, b);
                     });
      Delaunay3 triangulation(causal_vertices.begin(), causal_vertices.end());
      FoliatedTriangulation3 ft(triangulation);

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
        for (auto& vertex : checked_vertices)
        {
          REQUIRE(FoliatedTriangulation3::is_vertex_timevalue_correct(vertex));
        }
      }

      THEN("The cell info is correct.")
      {
        for (auto&& cit = ft.finite_cells_begin(); cit != ft.finite_cells_end();
             ++cit)
        {
          fmt::print("Simplex type is {}\n", cit->info());
          REQUIRE(cit->info() == static_cast<int>(Cell_type::THREE_ONE));
        }
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
