/// Causal Dynamical Triangulations in C++ using CGAL
///
/// Copyright © 2018-2021 Adam Getchell
///
/// Extends CGAL's Delaunay_triangulation_3 and Triangulation_3 classes
/// to create foliated spherical triangulations of a given dimension.
///
/// The dimensionality, number of desired simplices, and number of desired
/// timeslices is given. Successive spheres are created with increasing radii,
/// parameterized by INITIAL_RADIUS and RADIAL_FACTOR. Each vertex at a given
/// radius is assigned a timeslice so that the entire triangulation will have a
/// defined foliation of time.

/// @file Foliated_triangulation.hpp
/// @brief Create foliated spherical triangulations

#ifndef CDT_PLUSPLUS_FOLIATEDTRIANGULATION_HPP
#define CDT_PLUSPLUS_FOLIATEDTRIANGULATION_HPP

/// Toggles detailed per-simplex debugging output
#define DETAILED_DEBUGGING
#undef DETAILED_DEBUGGING

#include "Utilities.hpp"
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_cell_base_with_info_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/squared_distance_3.h>
#include <optional>

using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
// using Triangulation3 = CGAL::Triangulation_3<Kernel>;
// Each vertex may be assigned a time value
using Vertex_base =
    CGAL::Triangulation_vertex_base_with_info_3<Int_precision, Kernel>;
// Each cell may be assigned a type based on time values
using Cell_base =
    CGAL::Triangulation_cell_base_with_info_3<Int_precision, Kernel>;
// Parallel operations
#ifdef CGAL_LINKED_WITH_TBB
using Tds = CGAL::Triangulation_data_structure_3<Vertex_base, Cell_base,
                                                 CGAL::Parallel_tag>;
#else
using Tds = CGAL::Triangulation_data_structure_3<Vertex_base, Cell_base,
                                                 CGAL::Sequential_tag>;
#endif

// Delaunay triangulation dimensionality
using Delaunay3 = CGAL::Delaunay_triangulation_3<Kernel, Tds>;
// using Delaunay4 = CGAL::Triangulation<CGAL::Epick_d<CGAL::Dimension_tag<4>>>;
using Point           = Delaunay3::Point;
using Causal_vertices = std::vector<std::pair<Point, Int_precision>>;
// using Simplex         = Triangulation3::Simplex; // incompatible with
// Triangulation_{cell,vertex}_base_with_info_3.h
using Cell_handle   = Delaunay3::Cell_handle;
using Face_handle   = std::pair<Cell_handle, Int_precision>;
using Facet         = Delaunay3::Facet;
using Edge_handle   = CGAL::Triple<Cell_handle, Int_precision, Int_precision>;
using Vertex_handle = Delaunay3::Vertex_handle;

/// (n,m) is number of vertices on (lower, higher) timeslice
enum class Cell_type
{
  THREE_ONE = 31,  // (3,1)
  TWO_TWO   = 22,  // (2,2)
  ONE_THREE = 13,  // (1,3)
  ERROR     = 0    // An error happened classifying cell
};

/// @brief Compare time values of vertices
/// @tparam VertexType The type of Vertex to compare
/// @return True if timevalue of lhs is less than rhs
template <typename VertexType>
auto const compare_v_info =
    [](VertexType const& lhs, VertexType const& rhs) -> bool {
  return lhs->info() < rhs->info();
};

/// FoliatedTriangulation class template
/// @tparam dimension Dimensionality of triangulation
template <size_t dimension>
class FoliatedTriangulation;

/// 3D Triangulation
template <>
class FoliatedTriangulation<3>  // NOLINT
{
  /// Data members initialized in order of declaration (Working Draft, Standard
  /// for C++ Programming Language, 12.6.2 section 13.3)
  Delaunay3                           m_triangulation{Delaunay3{}};
  std::vector<Cell_handle>            m_cells;
  std::vector<Cell_handle>            m_three_one;
  std::vector<Cell_handle>            m_two_two;
  std::vector<Cell_handle>            m_one_three;
  std::vector<Face_handle>            m_faces;
  std::multimap<Int_precision, Facet> m_spacelike_facets;
  std::vector<Edge_handle>            m_edges;
  std::vector<Edge_handle>            m_timelike_edges;
  std::vector<Edge_handle>            m_spacelike_edges;
  std::vector<Vertex_handle>          m_points;
  Int_precision                       m_max_timevalue{0};
  Int_precision                       m_min_timevalue{0};
  double                              m_initial_radius{INITIAL_RADIUS};
  double                              m_foliation_spacing{FOLIATION_SPACING};

 public:
  /// @brief Default dtor
  ~FoliatedTriangulation() = default;

  /// @brief Default ctor
  FoliatedTriangulation() = default;

  /// @brief Copy Constructor
  FoliatedTriangulation(FoliatedTriangulation const& other)
      : FoliatedTriangulation(
            static_cast<Delaunay3 const&>(other.get_delaunay()))
  {}

  /// @brief Copy/Move Assignment operator
  auto operator=(FoliatedTriangulation other) noexcept -> FoliatedTriangulation&
  {
    swap(*this, other);
    return *this;
  }

  /// @brief Move ctor
  FoliatedTriangulation(FoliatedTriangulation&& other) noexcept
      : FoliatedTriangulation{}
  {
    swap(*this, other);
  }

  /// @brief Non-member swap function for Foliated Triangulations.
  /// Note that this function calls swap() from CGAL's Triangulation_3 base
  /// class, which assumes that the first triangulation is discarded after
  /// it is swapped into the second one.
  /// @param swap_from The value to be swapped from. Assumed to be discarded.
  /// @param swap_into The value to be swapped into.
  friend void swap(FoliatedTriangulation<3>& swap_from,
                   FoliatedTriangulation<3>& swap_into) noexcept
  {
#ifndef NDEBUG
    fmt::print("{} called.\n", __PRETTY_FUNCTION__);
#endif
    // Uses the triangulation swap method in CGAL
    // This assumes that the first triangulation is not used afterwards!
    // See
    // https://doc.cgal.org/latest/Triangulation_3/classCGAL_1_1Triangulation__3.html#a767066a964b4d7b14376e5f5d1a04b34
    swap_into.m_triangulation.swap(swap_from.m_triangulation);
    using std::swap;
    swap(swap_from.m_cells, swap_into.m_cells);
    swap(swap_from.m_three_one, swap_into.m_three_one);
    swap(swap_from.m_two_two, swap_into.m_two_two);
    swap(swap_from.m_one_three, swap_into.m_one_three);
    swap(swap_from.m_faces, swap_into.m_faces);
    swap(swap_from.m_spacelike_facets, swap_into.m_spacelike_facets);
    swap(swap_from.m_edges, swap_into.m_edges);
    swap(swap_from.m_timelike_edges, swap_into.m_timelike_edges);
    swap(swap_from.m_spacelike_edges, swap_into.m_spacelike_edges);
    swap(swap_from.m_points, swap_into.m_points);
    swap(swap_from.m_max_timevalue, swap_into.m_max_timevalue);
    swap(swap_from.m_min_timevalue, swap_into.m_min_timevalue);
    swap(swap_from.m_initial_radius, swap_into.m_initial_radius);
    swap(swap_from.m_foliation_spacing, swap_into.m_foliation_spacing);
  }  // swap

  /// @brief Constructor using delaunay triangulation
  /// Pass-by-value-then-move.
  /// Delaunay3 is the ctor for the Delaunay triangulation.
  /// @param triangulation Delaunay triangulation
  explicit FoliatedTriangulation(Delaunay3 triangulation)
      : m_triangulation{std::move(triangulation)}
      , m_cells{classify_cells(collect_cells())}
      , m_three_one{filter_cells(m_cells, Cell_type::THREE_ONE)}
      , m_two_two{filter_cells(m_cells, Cell_type::TWO_TWO)}
      , m_one_three{filter_cells(m_cells, Cell_type::ONE_THREE)}
      , m_faces{collect_faces()}
      , m_spacelike_facets{volume_per_timeslice(m_faces)}
      , m_edges{collect_edges()}
      , m_timelike_edges{filter_edges(m_edges, true)}
      , m_spacelike_edges{filter_edges(m_edges, false)}
      , m_points{collect_vertices()}
      , m_max_timevalue{find_max_timevalue(m_points)}
      , m_min_timevalue{find_min_timevalue(m_points)}
  {}

  /// @brief Constructor with parameters
  /// @param t_simplices Number of desired simplices
  /// @param t_timeslices Number of desired timeslices
  /// @param t_initial_radius Radius of first timeslice
  /// @param t_foliation_spacing Radial separation between timeslices
  [[maybe_unused]] FoliatedTriangulation(
      Int_precision const t_simplices, Int_precision const t_timeslices,
      double const t_initial_radius    = INITIAL_RADIUS,
      double const t_foliation_spacing = FOLIATION_SPACING)
      : m_triangulation{make_triangulation(
            t_simplices, t_timeslices, t_initial_radius, t_foliation_spacing)}
      , m_cells{classify_cells(collect_cells())}
      , m_three_one{filter_cells(m_cells, Cell_type::THREE_ONE)}
      , m_two_two{filter_cells(m_cells, Cell_type::TWO_TWO)}
      , m_one_three{filter_cells(m_cells, Cell_type::ONE_THREE)}
      , m_faces{collect_faces()}
      , m_spacelike_facets{volume_per_timeslice(m_faces)}
      , m_edges{collect_edges()}
      , m_timelike_edges{filter_edges(m_edges, true)}
      , m_spacelike_edges{filter_edges(m_edges, false)}
      , m_points{collect_vertices()}
      , m_max_timevalue{find_max_timevalue(m_points)}
      , m_min_timevalue{find_min_timevalue(m_points)}
      , m_initial_radius{t_initial_radius}
      , m_foliation_spacing{t_foliation_spacing}
  {}

  /// @brief Constructor from Causal_vertices
  /// @param cv Causal_vertices to place into the FoliatedTriangulation
  explicit FoliatedTriangulation(
      Causal_vertices cv, double const t_initial_radius = INITIAL_RADIUS,
      double const t_foliation_spacing = FOLIATION_SPACING)
      : m_triangulation{Delaunay3(cv.begin(), cv.end())}
      , m_cells{classify_cells(collect_cells())}
      , m_three_one{filter_cells(m_cells, Cell_type::THREE_ONE)}
      , m_two_two{filter_cells(m_cells, Cell_type::TWO_TWO)}
      , m_one_three{filter_cells(m_cells, Cell_type::ONE_THREE)}
      , m_faces{collect_faces()}
      , m_spacelike_facets{volume_per_timeslice(m_faces)}
      , m_edges{collect_edges()}
      , m_timelike_edges{filter_edges(m_edges, true)}
      , m_spacelike_edges{filter_edges(m_edges, false)}
      , m_points{collect_vertices()}
      , m_max_timevalue{find_max_timevalue(m_points)}
      , m_min_timevalue{find_min_timevalue(m_points)}
      , m_initial_radius{t_initial_radius}
      , m_foliation_spacing{t_foliation_spacing}
  {}

  /// @brief Verifies the triangulation is properly foliated
  ///
  /// Can not be called until after Foliated_triangulation has been constructed
  /// (i.e. not in make_triangulation)
  ///
  /// @return True if foliated correctly
  [[nodiscard]] auto is_foliated() const -> bool
  {
    return !static_cast<bool>(check_timeslices(get_delaunay()));
  }  // is_foliated

  /// @return True if the triangulation is Delaunay
  [[nodiscard]] auto is_delaunay() const -> bool
  {
    return get_delaunay().is_valid();
  }  // is_delaunay

  /// @return True if the triangulation data structure is valid
  [[nodiscard]] auto is_tds_valid() const -> bool
  {
    return get_delaunay().tds().is_valid();
  }  // is_tds_valid

  /// @return True if the Foliated Triangulation class invariants hold
  /// @todo Fix fatal error that occurs if check_all_cells() is added
  [[nodiscard]] auto is_correct() const -> bool
  {
    return is_foliated() && is_tds_valid() && check_all_vertices();
  }  // is_correct

  /// @return True if the Foliated Triangulation has been initialized correctly
  [[nodiscard]] auto is_initialized() const -> bool
  {
    return is_correct() && is_delaunay();
  }  // is_initialized

  /// @return A mutable reference to the Delaunay base class
  [[nodiscard]] auto delaunay() -> Delaunay3& { return m_triangulation; }

  /// @return A read-only reference to the Delaunay base class
  [[nodiscard]] auto get_delaunay() const -> Delaunay3 const&
  {
    return std::cref(m_triangulation);
  }  // get_delaunay

  /// @return An InputIterator to the beginning of the finite cells stored in
  /// https://doc.cgal.org/latest/STL_Extension/classCGAL_1_1Compact__container.html
  [[nodiscard]] auto finite_cells_begin()
  {
    return m_triangulation.finite_cells_begin();
  }  // finite_cells_begin

  /// @return An InputIterator to the end of the finite cells stored in
  /// https://doc.cgal.org/latest/STL_Extension/classCGAL_1_1Compact__container.html
  [[nodiscard]] auto finite_cells_end()
  {
    return m_triangulation.finite_cells_end();
  }  // finite_cells_end

  /// @return Number of 3D simplices in triangulation data structure
  [[nodiscard]] auto number_of_finite_cells() const
  {
    return m_triangulation.number_of_finite_cells();
  }  // number_of_finite_cells

  /// @return Number of 2D faces in triangulation data structure
  [[nodiscard]] auto number_of_finite_facets() const
  {
    return m_triangulation.number_of_finite_facets();
  }  // number_of_finite_facets

  /// @return Number of 1D edges in triangulation data structure
  [[nodiscard]] auto number_of_finite_edges() const
  {
    return m_triangulation.number_of_finite_edges();
  }  // number_of_finite_edges

  /// @return Number of vertices in triangulation data structure
  [[nodiscard]] auto number_of_vertices() const
  {
    return m_triangulation.number_of_vertices();
  }  // number_of_vertices

  /// @return If a cell or vertex contains or is the infinite vertex
  /// Forward parameters (see F.19 of C++ Core Guidelines)
  template <typename VertexHandle>
  [[nodiscard]] auto is_infinite(VertexHandle&& t_vertex) const
  {
    return m_triangulation.is_infinite(std::forward<VertexHandle>(t_vertex));
  }  // is_infinite

  /// @brief Call one of the TSD3.flip functions
  ///
  /// See
  /// https://doc.cgal.org/latest/Triangulation_3/classCGAL_1_1Triangulation__3.html#a883fed00b53cae9e85feb20230f54dd9
  ///
  /// @tparam Ts Variadic template of types of the arguments
  /// @param args Parameter pack of arguments to TDS3.flip
  /// @return True if the flip occurred
  template <typename... Ts>
  [[nodiscard]] auto flip(Ts&&... args)
  {
    return m_triangulation.flip(std::forward<Ts>(args)...);
  }  // flip

  /// @return Returns the infinite vertex in the triangulation
  [[maybe_unused]] [[nodiscard]] auto infinite_vertex() const
  {
    return m_triangulation.infinite_vertex();
  }  // infinite_vertex

  /// @return Dimensionality of triangulation data structure (int)
  [[nodiscard]] auto dimension() const { return m_triangulation.dimension(); }

  /// @return Container of spacelike facets indexed by time value
  [[nodiscard]] auto N2_SL() const -> std::multimap<Int_precision, Facet> const&
  {
    return m_spacelike_facets;
  }  // N2_SL

  /// @return Number of timelike edges
  [[nodiscard]] auto N1_TL() const
  {
    return static_cast<Int_precision>(m_timelike_edges.size());
  }  // N1_TL

  /// @return Number of spacelike edges
  [[nodiscard]] auto N1_SL() const
  {
    return static_cast<Int_precision>(m_spacelike_edges.size());
  }  // N1_SL

  /// @return Container of timelike edges
  [[nodiscard]] auto get_timelike_edges() const
      -> std::vector<Edge_handle> const&
  {
    return m_timelike_edges;
  }  // get_timelike_edges

  /// @return Container of spacelike edges
  [[nodiscard]] auto get_spacelike_edges() const
      -> std::vector<Edge_handle> const&
  {
    return m_spacelike_edges;
  }  // get_spacelike_edges

  /// @return Container of vertices
  [[nodiscard]] auto get_vertices() const -> std::vector<Vertex_handle> const&
  {
    return m_points;
  }  // get_vertices

  /// @return Maximum time value in triangulation
  [[nodiscard]] auto max_time() const { return m_max_timevalue; }

  /// @return Minimum time value in triangulation
  [[nodiscard]] auto min_time() const { return m_min_timevalue; }

  /// @return The initial radius for timeslice = 1
  [[nodiscard]] auto initial_radius() const { return m_initial_radius; }

  /// @return The spacing between timeslices
  [[nodiscard]] auto foliation_spacing() const { return m_foliation_spacing; }

  /// @brief See
  /// https://doc.cgal.org/latest/TDS_3/classTriangulationDataStructure__3.html#a51fce32aa7abf3d757bcabcebd22f2fe
  /// If we have n incident edges we should have 2(n-2) incident cells
  /// Forward parameters (see F.19 of C++ Core Guidelines)
  /// @return The number of incident edges to a vertex
  template <typename VertexHandle>
  [[nodiscard]] auto degree(VertexHandle&& t_vertex) const
  {
    return m_triangulation.degree(std::forward<VertexHandle>(t_vertex));
  }  // degree

  /// @brief Call one of the TDS3.incident_cells functions
  ///
  /// See
  /// https://doc.cgal.org/latest/TDS_3/classTriangulationDataStructure__3.html#a93f8ab30228b2a515a5c9cdacd9d4d36
  ///
  /// @tparam VertexHandle Template parameter used to forward
  /// @param t_vh Vertex
  /// @return A container of incident cells
  template <typename VertexHandle>
  [[nodiscard]] auto incident_cells(VertexHandle&& t_vh) const -> decltype(auto)
  {
    std::vector<Cell_handle> inc_cells;
    get_delaunay().tds().incident_cells(std::forward<VertexHandle>(t_vh),
                                        std::back_inserter(inc_cells));
    return inc_cells;
  }  // incident_cells

  /// @brief Perfect forwarding to Delaunay3.tds().incident_cells()
  ///
  /// @tparam Ts Variadic template used to forward
  /// @param args Parameter pack of arguments to call incident_cells()
  /// @return A Cell_circulator
  template <typename... Ts>
  [[nodiscard]] auto incident_cells(Ts&&... args) const -> decltype(auto)
  {
    return get_delaunay().tds().incident_cells(std::forward<Ts>(args)...);
  }  // incident_cells

  /// @brief CGAL::squared_distance
  /// See
  /// https://doc.cgal.org/latest/Kernel_23/group__squared__distance__grp.html#ga1ff73525660a052564d33fbdd61a4f71
  /// @return Square of Euclidean distance between two geometric objects
  using squared_distance = Kernel::Compute_squared_distance_3;

  /// @brief Check the radius of a vertex from the origin with its timevalue
  /// @param t_vertex The vertex to check
  /// @return True if the effective radial distance squared matches timevalue
  /// squared
  [[nodiscard]] auto does_vertex_radius_match_timevalue(
      Vertex_handle t_vertex) const -> bool
  {
    auto actual_radius_squared   = squared_radius(t_vertex);
    auto radius                  = expected_radius(t_vertex);
    auto expected_radius_squared = std::pow(radius, 2);
    return (actual_radius_squared > expected_radius_squared * (1 - TOLERANCE) &&
            actual_radius_squared < expected_radius_squared * (1 + TOLERANCE));
  }  // does_vertex_radius_match_timevalue

  /// @brief Checks if vertex timevalue is correct
  /// @param t_vertex The vertex to check
  /// @return True if vertex->info() equals expected_timevalue()
  [[nodiscard]] auto is_vertex_timevalue_correct(
      Vertex_handle const& t_vertex) const -> bool
  {
    auto e_timevalue = this->expected_timevalue(t_vertex);
#ifdef DETAILED_DEBUGGING
    fmt::print("Vertex ({}) with info() {}: expected timevalue {}\n",
               t_vertex->point(), t_vertex->info(), e_timevalue);
#endif
    return e_timevalue == t_vertex->info();
  }  // is_vertex_timevalue_correct

  /// @brief Calculates the expected radial distance of a vertex given its
  /// timevalue
  ///
  /// The formula for the radius is:
  ///
  /// \f[R=I+S(t-1)\f]
  ///
  /// Where I is INITIAL_RADIUS, S is RADIAL_SEPARATION, and t is timevalue
  ///
  /// @param t_vertex The vertex to check
  /// @return The expected radial distance of the vertex of that timevalue
  [[nodiscard]] auto expected_radius(Vertex_handle const& t_vertex) const
      -> double
  {
    auto timevalue = t_vertex->info();
    return m_initial_radius + m_foliation_spacing * (timevalue - 1);
  }  // expected_radial_distance

  /// @brief Calculate the squared radius from the origin
  /// @param t_vertex The vertex to check
  /// @return The squared radial distance of the vertex
  [[nodiscard]] static auto squared_radius(Vertex_handle const& t_vertex)
      -> double
  {
    auto             origin = Point(0, 0, 0);
    squared_distance r2;
    return r2(t_vertex->point(), origin);
  }  // squared_radius

  /// @brief Calculate the expected timevalue for a vertex
  ///
  /// The formula for the expected timevalue is:
  ///
  /// \f[t=\frac{R-I+S}{S}\f]
  ///
  /// Where R is radius, I is INITIAL_RADIUS, and S is RADIAL_SEPARATION
  ///
  ///
  /// @param t_vertex The vertex to check
  /// @return The expected timevalue of the vertex
  [[nodiscard]] auto expected_timevalue(Vertex_handle const& t_vertex) const
      -> int
  {
    auto radius = std::sqrt(squared_radius(t_vertex));
    return static_cast<Int_precision>(
        std::lround((radius - m_initial_radius + m_foliation_spacing) /
                    m_foliation_spacing));
  }  // expected_timevalue

  /// @return True if all vertices have correct timevalues
  [[nodiscard]] auto check_all_vertices() const -> bool
  {
    auto checked_vertices = this->get_vertices();
    return this->check_vertices(checked_vertices);
  }  // check_all_vertices

  /// @brief Check if vertices have the correct timevalues
  /// @param vertices The container of vertices to check
  /// @return True if all vertices in the container have correct timevalues
  [[nodiscard]] auto check_vertices(
      std::vector<Vertex_handle> const& vertices) const -> bool
  {
    return std::all_of(vertices.begin(), vertices.end(),
                       [this](Vertex_handle const& vertex) {
                         return is_vertex_timevalue_correct(vertex);
                       });
  }  // check_vertices

  /// @return A container of incorrect vertices
  [[nodiscard]] auto find_incorrect_vertices() const
      -> std::vector<Vertex_handle>
  {
    std::vector<Vertex_handle> incorrect_vertices;
    auto                       checked_vertices = this->get_vertices();
    for (auto& vertex : checked_vertices)
    {
      if (!is_vertex_timevalue_correct(vertex))
      {
        incorrect_vertices.emplace_back(vertex);
      }
    }
    return incorrect_vertices;
  }  // find_incorrect_vertices

  /// @brief Fix vertices with wrong timevalues after foliation
  /// @param incorrect_vertices The container of incorrect vertices
  void fix_vertices(std::vector<Vertex_handle> const& incorrect_vertices) const
  {
    for (auto const& vertex : incorrect_vertices)
    {
      vertex->info() = expected_timevalue(vertex);
    }
  }  // fix_vertices

  /// @brief Print values of a vertex.
  void print_vertices() const
  {
    for (auto const& vertex : m_points)
    {
      fmt::print("Vertex Point: ({}) Timevalue: {} Expected Timevalue: {}\n",
                 vertex->point(), vertex->info(), expected_timevalue(vertex));
    }
  }  // print_vertices

  /// @brief Predicate to classify edge as timelike or spacelike
  /// @param t_edge The Edge_handle to classify
  /// @param t_debug_flag Debugging info toggle
  /// @return true if timelike and false if spacelike
  [[nodiscard]] static auto classify_edge(Edge_handle const& t_edge,
                                          bool t_debug_flag = false) -> bool
  {
    Cell_handle const& ch    = t_edge.first;
    auto               time1 = ch->vertex(t_edge.second)->info();
    auto               time2 = ch->vertex(t_edge.third)->info();
    if (t_debug_flag)
    {
      fmt::print("Edge: Vertex(1) timevalue: {} Vertex(2) timevalue: {}\n",
                 time1, time2);
    }
    return time1 != time2;
  }  // classify_edge

  /// @brief Print timevalues of each vertex in the edge and classify as
  /// timelike or spacelike
  void print_edges() const
  {
    for (auto const& edge : m_edges)
    {
      if (classify_edge(edge, true)) { fmt::print("==> timelike\n"); }
      else
      {
        fmt::print("==> spacelike\n");
      }
    }
  }  // print_edges

  /// @brief Print the number of spacelike faces per timeslice
  void print_volume_per_timeslice() const
  {
    for (auto j = min_time(); j <= max_time(); ++j)
    {
      fmt::print("Timeslice {} has {} spacelike faces.\n", j,
                 m_spacelike_facets.count(j));
    }
  }  // print_volume_per_timeslice

  /// @return Container of cells
  [[nodiscard]] auto get_cells() const -> std::vector<Cell_handle> const&
  {
    Ensures(m_cells.size() == number_of_finite_cells());
    return m_cells;
  }  // get_cells

  /// @brief Filter simplices by Cell_type
  /// @param t_cells The container of simplices to filter
  /// @param t_cell_type The Cell_type predicate filter
  /// @return A container of Cell_type simplices
  [[nodiscard]] static auto filter_cells(
      std::vector<Cell_handle> const& t_cells, Cell_type const& t_cell_type)
      -> std::vector<Cell_handle>
  {
    Expects(!t_cells.empty());
    std::vector<Cell_handle> filtered_cells;
    std::copy_if(t_cells.begin(), t_cells.end(),
                 std::back_inserter(filtered_cells),
                 [&t_cell_type](auto const& cell) {
                   return cell->info() == static_cast<int>(t_cell_type);
                 });
    return filtered_cells;
  }  // filter_cells

  /// @return Container of (3,1) cells
  [[nodiscard]] auto get_three_one() const -> std::vector<Cell_handle> const&
  {
    return m_three_one;
  }  // get_three_one

  /// @return Container of (2,2) cells
  [[nodiscard]] auto get_two_two() const -> std::vector<Cell_handle> const&
  {
    return m_two_two;
  }  // get_two_two

  /// @return Container of (1,3) cells
  [[nodiscard]] auto get_one_three() const -> std::vector<Cell_handle> const&
  {
    return m_one_three;
  }  // get_one_three

  /// @param t_cell The simplex to check
  /// @return The type of simplex
  [[nodiscard]] static auto expected_cell_type(Cell_handle const& t_cell,
                                               bool t_debug_flag = false)
  {
    std::vector<int> vertex_timevalues;
    for (int i = 0; i < 4; ++i)
    {
      // Obtain timevalue of vertex
      vertex_timevalues.emplace_back(t_cell->vertex(i)->info());
    }
    auto maxtime_ref =
        std::max_element(vertex_timevalues.begin(), vertex_timevalues.end());
    auto mintime_ref =
        std::min_element(vertex_timevalues.begin(), vertex_timevalues.end());
    auto maxtime = *maxtime_ref;
    auto mintime = *mintime_ref;
    // A properly foliated simplex should have a timevalue difference of 1
    if (maxtime - mintime != 1) { return Cell_type::ERROR; }
    std::multiset<int> timevalues{vertex_timevalues.begin(),
                                  vertex_timevalues.end()};
    auto               max_vertices = timevalues.count(maxtime);
    auto               min_vertices = timevalues.count(mintime);

    if (max_vertices == 3 && min_vertices == 1) { return Cell_type::ONE_THREE; }
    if (max_vertices == 2 && min_vertices == 2) { return Cell_type::TWO_TWO; }
    if (max_vertices == 1 && min_vertices == 3) { return Cell_type::THREE_ONE; }

    // If we got here, there's some kind of error
    if (t_debug_flag)
    {
      fmt::print("This simplex has an error.\n");
      fmt::print("Max timevalue is {}\n", maxtime);
      fmt::print("There are {} vertices with the max timevalue.\n",
                 max_vertices);
      fmt::print("Min timevalue is {}\n", mintime);
      fmt::print("There are {} vertices with the min timevalue.\n",
                 min_vertices);
      fmt::print("--\n");
    }
    return Cell_type::ERROR;
  }  // expected_cell_type

  /// @param t_cell The simplex to check
  /// @return True if the cell_info matches expected cell_info
  [[nodiscard]] static auto is_cell_type_correct(Cell_handle const& t_cell)
      -> bool
  {
    auto cell_type = expected_cell_type(t_cell);
    return cell_type != Cell_type::ERROR &&
           cell_type == static_cast<Cell_type>(t_cell->info());
  }  // is_cell_type_correct

  /// @brief Check that all cells are correctly classified
  /// @return True if all cells are validly classified
  [[nodiscard]] auto check_all_cells() const -> bool
  {
    auto checked_cells = this->get_cells();
    Expects(!checked_cells.empty());
    return FoliatedTriangulation<3>::check_cells(checked_cells);
  }  // check_all_cells

  /// @brief Check that all cells in a container are correctly classified
  /// @param t_cells The container of cells to check
  /// @return True if all cells in the container are validly classified
  [[nodiscard]] static auto check_cells(std::vector<Cell_handle> const& t_cells)
      -> bool
  {
    Expects(!t_cells.empty());
    return std::all_of(t_cells.begin(), t_cells.end(), is_cell_type_correct);
  }  // check_cells

  /// @return A container of incorrect cells
  [[nodiscard]] auto find_incorrect_cells() const -> std::vector<Cell_handle>
  {
    std::vector<Cell_handle> incorrect_cells;
    auto                     checked_cells = this->get_cells();
    for (auto& cell : checked_cells)
    {
      if (!is_cell_type_correct(cell)) { incorrect_cells.emplace_back(cell); }
    }
    return incorrect_cells;
  }  // find_incorrect_cells

  /// @brief Fix simplices with the wrong type
  /// @param t_cells The container of incorrect simplices
  static void fix_cells(std::vector<Cell_handle> const& t_cells)
  {
    Expects(!t_cells.empty());
    for (auto const& cell : t_cells)
    {
      cell->info() = static_cast<int>(expected_cell_type(cell));
    }
  }

  /// @brief Print timevalues of each vertex in the cell and the resulting
  /// cell->info()
  void print_cells() const { print_cells(m_cells); }

  /// @brief Print timevalues of each vertex in the cell and the resulting
  /// cell->info()
  /// @param t_cells The cells to print
  static void print_cells(std::vector<Cell_handle> const& t_cells)
  {
    for (auto const& cell : t_cells)
    {
      fmt::print("Cell info => {}\n", cell->info());
      for (int j = 0; j < 4; ++j)
      {
        fmt::print("Vertex({}) Point: ({}) Timevalue: {}\n", j,
                   cell->vertex(j)->point(), cell->vertex(j)->info());
      }
      fmt::print("---\n");
    }
  }  // print_cells

  /// @brief Check simplices for correct foliation
  ///
  /// This function is called by fix_timeslices which is called by
  /// make_triangulation which is called by the constructor, and so must take
  /// the triangulation as an argument.
  ///
  /// It should also be perfectly valid to call once the triangulation has
  /// been constructed; several unit tests do so.
  ///
  /// @tparam Triangulation Perfectly forwarded argument type
  /// @param t_triangulation Perfectly forwarded argument
  /// @return A container of invalidly foliated vertices if they exist
  template <typename Triangulation>
  [[nodiscard]] auto check_timeslices(Triangulation&& t_triangulation) const
      -> std::optional<std::vector<Vertex_handle>>
  {
    std::vector<Vertex_handle> invalid_vertices;
    // Iterate over all cells in the triangulation
    for (Delaunay3::Finite_cells_iterator cit =
             t_triangulation.finite_cells_begin();
         cit != t_triangulation.finite_cells_end(); ++cit)
    {
      Ensures(cit->is_valid());
      std::multimap<int, Vertex_handle> this_cell;
      // Collect a map of timevalues and vertices in each cell
      for (int i = 0; i < 4; ++i)
      {
        this_cell.emplace(
            std::make_pair(cit->vertex(i)->info(), cit->vertex(i)));
      }
      // Now it's sorted in the multimap
      auto minvalue = this_cell.cbegin()->first;
      auto maxvalue = this_cell.crbegin()->first;

#ifdef DETAILED_DEBUGGING
      auto min_vertex = this_cell.cbegin()->second;
      auto max_vertex = this_cell.crbegin()->second;
      fmt::print("Smallest timevalue in this cell is: {}\n", minvalue);
      fmt::print("Largest timevalue in this cell is: {}\n", maxvalue);
      fmt::print("Min vertex info() {}\n", min_vertex->info());
      fmt::print("Max vertex info() {}\n", max_vertex->info());
#endif
      // There must be a timevalue delta of 1 for a validly foliated simplex
      if (maxvalue - minvalue == 1)
      {
#ifdef DETAILED_DEBUGGING
        fmt::print("This cell is valid.\n");
#endif
      }
      else
      {
        auto minvalue_count = this_cell.count(minvalue);
        auto maxvalue_count = this_cell.count(maxvalue);
#ifdef DETAILED_DEBUGGING
        fmt::print("This cell is invalid.\n");
        fmt::print("There are {} vertices with the minvalue.\n",
                   minvalue_count);
        fmt::print("There are {} vertices with the maxvalue.\n",
                   maxvalue_count);
        fmt::print("So we should remove ");
#endif

        if (minvalue_count > maxvalue_count)
        {
          invalid_vertices.emplace_back(this_cell.rbegin()->second);
#ifdef DETAILED_DEBUGGING
          fmt::print("maxvalue.\n");
#endif
        }
        else
        {
          invalid_vertices.emplace_back(this_cell.begin()->second);
#ifdef DETAILED_DEBUGGING
          fmt::print("minvalue.\n");
#endif
        }
      }
    }
    if (invalid_vertices.empty()) { return std::nullopt; }

#ifdef DETAILED_DEBUGGING
    fmt::print("Removing ...\n");
    for (auto& v : invalid_vertices)
    {
      fmt::print("Vertex {} with timevalue {}\n", v->point(), v->info());
    }
#endif
    return invalid_vertices;

  }  // check_timeslices

 private:
  /// @brief Make a Delaunay Triangulation
  /// @param t_simplices Number of desired simplices
  /// @param t_timeslices Number of desired timeslices
  /// @param initial_radius Radius of first timeslice
  /// @param foliation_spacing Radial separation between timeslices
  /// @return A Delaunay Triangulation
  [[nodiscard]] auto make_triangulation(
      Int_precision const t_simplices, Int_precision const t_timeslices,
      double const initial_radius    = INITIAL_RADIUS,
      double const foliation_spacing = FOLIATION_SPACING) -> Delaunay3
  {
    fmt::print("Generating universe ...\n");
#ifdef CGAL_LINKED_WITH_TBB
    // Construct the locking data-structure
    // using the bounding-box of the points
    auto bounding_box_size = static_cast<double>(t_timeslices + 1);
    Delaunay3::Lock_data_structure locking_ds{
        CGAL::Bbox_3{-bounding_box_size, -bounding_box_size, -bounding_box_size,
                     bounding_box_size, bounding_box_size, bounding_box_size},
        50};
    Delaunay3 triangulation = Delaunay3{Kernel{}, &locking_ds};
#else
    Delaunay3 triangulation = Delaunay3{};
#endif

    auto causal_vertices = make_foliated_sphere(
        t_simplices, t_timeslices, initial_radius, foliation_spacing);
    triangulation.insert(causal_vertices.begin(), causal_vertices.end());
    int passes = 1;
    while (!fix_timeslices(triangulation))
    {
#ifndef NDEBUG
      fmt::print("Fix pass #{}\n", passes);
#endif
      ++passes;
    }
    print_triangulation(triangulation);
    Ensures(!check_timeslices(triangulation));
    return triangulation;
  }  // make_triangulation

  /// @brief Make foliated spheres
  /// @param t_simplices The desired number of simplices in the triangulation
  /// @param t_timeslices The desired number of timeslices in the triangulation
  /// @param initial_radius The radius of the first time slice
  /// @param foliation_spacing The distance between successive time slices
  /// @return A container of (vertex, timevalue) pairs
  [[nodiscard]] static auto make_foliated_sphere(
      Int_precision const t_simplices, Int_precision const t_timeslices,
      double const initial_radius    = INITIAL_RADIUS,
      double const foliation_spacing = FOLIATION_SPACING) -> Causal_vertices
  {
    Causal_vertices causal_vertices;
    causal_vertices.reserve(static_cast<std::size_t>(t_simplices));
    const auto points_per_timeslice =
        expected_points_per_timeslice(3, t_simplices, t_timeslices);
    Expects(points_per_timeslice >= 2);

    using Spherical_points_generator = CGAL::Random_points_on_sphere_3<Point>;
    for (gsl::index i = 0; i < t_timeslices; ++i)
    {
      auto radius = initial_radius + static_cast<double>(i) * foliation_spacing;
      Spherical_points_generator gen{static_cast<double>(radius)};
      // Generate random points at the radius
      for (gsl::index j = 0;
           j < static_cast<Int_precision>(points_per_timeslice * radius); ++j)
      {
        causal_vertices.emplace_back(std::make_pair(*gen++, i + 1));
      }  // j
    }    // i
    return causal_vertices;
  }  // make_foliated_sphere

  /// @brief Fix simplices with incorrect foliation
  ///
  /// This function iterates over all of the cells in the triangulation.
  /// Within each cell, it iterates over all of the vertices and reads
  /// timeslices.
  /// Validity of the cell is first checked by the **is_valid()** function.
  /// The foliation validity is then checked by finding maximum and minimum
  /// timeslices for all the vertices of a cell and ensuring that the
  /// difference
  /// is exactly 1.
  /// If a cell has a bad foliation, the vertex with the highest timeslice is
  /// deleted. The Delaunay triangulation is then recomputed on the remaining
  /// vertices.
  ///
  /// @tparam Triangulation Perfectly forwarded argument type
  /// @param t_triangulation Perfectly forwarded argument
  /// @return True if there are no incorrectly foliated simplices
  template <typename Triangulation>
  [[nodiscard]] auto fix_timeslices(Triangulation&& t_triangulation) -> bool
  {
    auto vertices_to_delete = check_timeslices(t_triangulation);
    std::set<Vertex_handle> deleted_vertices;
    // Remove duplicates
    if (vertices_to_delete)
    {
      for (auto& v : vertices_to_delete.value())
      {
        deleted_vertices.emplace(v);
      }
    }
    auto invalid = deleted_vertices.size();

    // Delete invalid vertices
    t_triangulation.remove(deleted_vertices.begin(), deleted_vertices.end());
    // Check that the triangulation is still valid
    // Turned off by -DCGAL_TRIANGULATION_NO_POSTCONDITIONS
    //  CGAL_triangulation_expensive_postcondition(universe_ptr->is_valid());
    //    if (!_delaunay.tds().is_valid())
    //      throw std::logic_error("Delaunay tds invalid!");
    Ensures(t_triangulation.tds().is_valid());
    Ensures(t_triangulation.is_valid());

#ifndef NDEBUG
    fmt::print("There are {} invalid simplices.\n", invalid);
#endif
    return invalid == 0;
  }  // fix_timeslices

  /// @return Container of all the finite simplices in the triangulation
  [[nodiscard]] auto collect_cells() const -> std::vector<Cell_handle>
  {
    Expects(get_delaunay().tds().is_valid(true));
    std::vector<Cell_handle> init_cells;
    init_cells.reserve(number_of_finite_cells());
    //    Delaunay3::Finite_cells_iterator cit;
    for (auto cit = get_delaunay().finite_cells_begin();
         cit != get_delaunay().finite_cells_end(); ++cit)
    {
      // Each cell is valid in the triangulation
      Ensures(get_delaunay().tds().is_cell(cit));
      init_cells.emplace_back(cit);
    }
    Ensures(init_cells.size() == number_of_finite_cells());
    return init_cells;
  }  // collect_cells

  /// @brief Classify cells
  /// @param cells The container of simplices to classify
  /// @param t_debug_flag Debugging info toggle
  /// @return A container of simplices with Cell_type written to cell->info()
  /// @todo Replace core of this function with call to expected_cell_type
  [[nodiscard]] auto classify_cells(std::vector<Cell_handle> const& cells,
                                    bool t_debug_flag = false) const
      -> std::vector<Cell_handle>
  {
    Expects(cells.size() == number_of_finite_cells());
    for (auto const& c : cells)
    {
      c->info() = static_cast<int>(expected_cell_type(c, t_debug_flag));
    }
    //    std::vector<Vertex_handle> cell_vertices;
    //    cell_vertices.reserve(4);
    //    std::vector<int> vertex_timevalues;
    //    vertex_timevalues.reserve(4);
    //    for (auto const& c : cells)
    //    {
    //      if (t_debug_flag) { fmt::print("Cell info was {}\n", c->info()); }
    //
    //      for (int j = 0; j < 4; ++j)
    //      {
    //        cell_vertices.emplace_back(c->vertex(j));
    //        vertex_timevalues.emplace_back(c->vertex(j)->info());
    //        if (t_debug_flag)
    //        {
    //          fmt::print("Cell vertex {} has timevalue {}\n", j,
    //                     c->vertex(j)->info());
    //        }
    //      }
    //
    //      // This is simply not sufficient. Need to check *both* maxtime and
    //      // min_time, and that there are exactly 1 and 3, 2 and 2, or 3
    //      and 1.
    //      // Anything else means we have an invalid simplex which we should
    //      // also return.
    //      // We also need to check that maxtime - min_time = 1, else we have
    //      // a mis-classified vertex (probably)
    //      // This is done in expected_cell_type()
    //      auto maxtime =
    //          std::max_element(vertex_timevalues.begin(),
    //          vertex_timevalues.end());
    //      auto maxtime_vertices = std::count_if(
    //          cell_vertices.begin(), cell_vertices.end(),
    //          [maxtime](auto const& vertex) { return vertex->info() ==
    //          *maxtime; });
    //      // Check maxtime - min_time here
    //      switch (maxtime_vertices)
    //      {
    //        case 1:
    //          c->info() = static_cast<int>(Cell_type::THREE_ONE);
    //          break;
    //        case 2:
    //          c->info() = static_cast<int>(Cell_type::TWO_TWO);
    //          break;
    //        case 3:
    //          c->info() = static_cast<int>(Cell_type::ONE_THREE);
    //          break;
    //        default:
    //          throw std::logic_error("Mis-classified cell.");
    //      }
    //      if (t_debug_flag)
    //      {
    //        fmt::print("Max timevalue is {}\n", *maxtime);
    //        fmt::print("There are {} vertices with max timeslice in the
    //        cell.\n",
    //                   maxtime_vertices);
    //        fmt::print("Cell info is now {}\n", c->info());
    //        fmt::print("--\n");
    //      }
    //      cell_vertices.clear();
    //      vertex_timevalues.clear();
    //    }
    return cells;
  }  // classify_cells

  /// @return Container of all the finite facets in the triangulation
  [[nodiscard]] auto collect_faces() const -> std::vector<Face_handle>
  {
    Expects(get_delaunay().tds().is_valid());
    std::vector<Face_handle> init_faces;
    init_faces.reserve(get_delaunay().number_of_finite_facets());
    //    Delaunay3::Finite_facets_iterator fit;
    for (auto fit = get_delaunay().finite_facets_begin();
         fit != get_delaunay().finite_facets_end(); ++fit)
    {
      Cell_handle ch = fit->first;
      // Each face is valid in the triangulation
      Ensures(get_delaunay().tds().is_facet(ch, fit->second));
      Face_handle thisFacet{std::make_pair(ch, fit->second)};
      init_faces.emplace_back(thisFacet);
    }
    Ensures(init_faces.size() == get_delaunay().number_of_finite_facets());
    return init_faces;
  }  // collect_faces

  /// /// @brief Collect spacelike facets into a container indexed by time value
  /// @param t_facets A container of facets
  /// @param t_debug_flag Debugging info toggle
  /// @return Container with spacelike facets per timeslice
  [[nodiscard]] static auto volume_per_timeslice(
      std::vector<Face_handle> const& t_facets, bool t_debug_flag = false)
      -> std::multimap<Int_precision, Facet>
  {
    std::multimap<Int_precision, Facet> space_faces;
    for (auto const& face : t_facets)
    {
      Cell_handle ch             = face.first;
      auto        index_of_facet = face.second;
      if (t_debug_flag) { fmt::print("Facet index is {}\n", index_of_facet); }
      std::set<Int_precision> facet_timevalues;
      for (int i = 0; i < 4; ++i)
      {
        if (i != index_of_facet)
        {
          if (t_debug_flag)
          {
            fmt::print("Vertex[{}] has timevalue {}\n", i,
                       ch->vertex(i)->info());
          }
          facet_timevalues.insert(ch->vertex(i)->info());
        }
      }
      // If we have a 1-element set then all timevalues on that facet are equal
      if (facet_timevalues.size() == 1)
      {
        if (t_debug_flag)
        {
          fmt::print("Facet is spacelike on timevalue {}.\n",
                     *facet_timevalues.begin());
        }
        space_faces.insert({*facet_timevalues.begin(), face});
      }
      else
      {
        if (t_debug_flag) { fmt::print("Facet is timelike.\n"); }
      }
    }
    return space_faces;
  }  // volume_per_timeslice

  /// @return Container of all the finite edges in the triangulation
  [[nodiscard]] auto collect_edges() const -> std::vector<Edge_handle>
  {
    Expects(get_delaunay().tds().is_valid());
    std::vector<Edge_handle> init_edges;
    init_edges.reserve(number_of_finite_edges());
    //    Delaunay3::Finite_edges_iterator eit;
    for (auto eit = get_delaunay().finite_edges_begin();
         eit != get_delaunay().finite_edges_end(); ++eit)
    {
      Cell_handle ch = eit->first;
      Edge_handle thisEdge{ch, ch->index(ch->vertex(eit->second)),
                           ch->index(ch->vertex(eit->third))};
      // Each edge is valid in the triangulation
      Ensures(get_delaunay().tds().is_valid(thisEdge.first, thisEdge.second,
                                            thisEdge.third));
      init_edges.emplace_back(thisEdge);
    }
    Ensures(init_edges.size() == number_of_finite_edges());
    return init_edges;
  }  // collect_edges

  /// @brief Filter edges into timelike and spacelike
  /// @param t_edges The container of edges to filter
  /// @param t_is_Timelike_pred The predicate condition
  /// @return A container of is_Timelike edges
  [[nodiscard]] static auto filter_edges(
      std::vector<Edge_handle> const& t_edges, bool t_is_Timelike_pred)
      -> std::vector<Edge_handle>
  {
    Expects(!t_edges.empty());
    std::vector<Edge_handle> filtered_edges;
    std::copy_if(t_edges.begin(), t_edges.end(),
                 std::back_inserter(filtered_edges), [&](auto const& edge) {
                   return (t_is_Timelike_pred == classify_edge(edge));
                 });
    Ensures(!filtered_edges.empty());
    return filtered_edges;
  }  // filter_edges

  /// @return Container of all finite vertices in the triangulation
  [[nodiscard]] auto collect_vertices() const -> std::vector<Vertex_handle>
  {
    Expects(get_delaunay().tds().is_valid());
    std::vector<Vertex_handle> init_vertices;
    init_vertices.reserve(get_delaunay().number_of_vertices());
    //    Delaunay3::Finite_vertices_iterator vit;
    for (auto vit = get_delaunay().finite_vertices_begin();
         vit != get_delaunay().finite_vertices_end(); ++vit)
    {  // Each vertex is valid in the triangulation
      Ensures(get_delaunay().tds().is_vertex(vit));
      init_vertices.emplace_back(vit);
    }
    Ensures(init_vertices.size() == get_delaunay().number_of_vertices());
    return init_vertices;
  }  // collect_vertices

  /// @brief Find maximum timevalues
  /// @param t_vertices Container of vertices
  /// @return The maximum timevalue
  [[nodiscard]] static auto find_max_timevalue(
      std::vector<Vertex_handle> const& t_vertices) -> Int_precision
  {
    Expects(!t_vertices.empty());
    auto it           = std::max_element(t_vertices.begin(), t_vertices.end(),
                               compare_v_info<Vertex_handle>);
    auto result_index = std::distance(t_vertices.begin(), it);
    Ensures(result_index >= 0);
    auto index = static_cast<std::size_t>(std::abs(result_index));
    return t_vertices[index]->info();
  }  // find_max_timevalue

  /// @brief Find minimum timevalues
  /// @param t_vertices Container of vertices
  /// @return The minimum timevalue
  [[nodiscard]] static auto find_min_timevalue(
      std::vector<Vertex_handle> const& t_vertices) -> int
  {
    Expects(!t_vertices.empty());
    auto it           = std::min_element(t_vertices.begin(), t_vertices.end(),
                               compare_v_info<Vertex_handle>);
    auto result_index = std::distance(t_vertices.begin(), it);
    Ensures(result_index >= 0);
    auto index = static_cast<std::size_t>(std::abs(result_index));
    return t_vertices[index]->info();
  }  // find_min_timevalue
};

using FoliatedTriangulation3 = FoliatedTriangulation<3>;

/// 4D Triangulation
// template <>
// class Foliated_triangulation<4> : Delaunay4
//{};
//
// using FoliatedTriangulation4 = Foliated_triangulation<4>;

#endif  // CDT_PLUSPLUS_FOLIATEDTRIANGULATION_HPP
