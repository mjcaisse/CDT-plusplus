/// Causal Dynamical Triangulations in C++ using CGAL
///
/// Copyright © 2017-2020 Adam Getchell
///
/// Tests for lambdas and function_refs to store function objects for delayed
/// calls
///
/// @file Function_ref.cpp
/// @brief Tests on function pointers, lambdas, and function_refs
/// @author Adam Getchell

#include "Ergodic_moves_3.hpp"
#include "Function_ref.hpp"
#include <catch2/catch.hpp>

using namespace std;

SCENARIO("Simple Lambda operations", "[function-ref]")
{
  constexpr auto increment_lambda = [](int a) { return ++a; };
  GIVEN("A simple lambda.")
  {
    WHEN("Lambda is called with 0.")
    {
      THEN("We should get 1.") { REQUIRE(increment_lambda(0) == 1); }
    }

    WHEN("Lambda is called with 1.")
    {
      THEN("We should get 2.") { REQUIRE(increment_lambda(1) == 2); }
    }

    WHEN("Lambda is called with 5.")
    {
      THEN("We should get 6.") { REQUIRE(increment_lambda(5) == 6); }
    }
  }
}

SCENARIO("Complex lambda operations", "[function-ref]")
{
  GIVEN("A lambda storing a move.")
  {
    auto constexpr desired_simplices  = static_cast<Int_precision>(640);
    auto constexpr desired_timeslices = static_cast<Int_precision>(4);
    Manifold3 manifold(desired_simplices, desired_timeslices);
    REQUIRE(manifold.is_correct());
    WHEN("A lambda is constructed for a move.")
    {
      auto const move23 = [](Manifold3& m) -> Manifold3 {
        return manifold3_moves::do_23_move(m);
      };
      THEN("Running the lambda makes the move.")
      {
        auto result = move23(manifold);
        result.update();
        CHECK(manifold3_moves::check_move(
            manifold, result, manifold3_moves::move_type::TWO_THREE));
        // Human verification
        fmt::print("Manifold properties:\n");
        print_manifold_details(manifold);
        fmt::print("Moved manifold properties:\n");
        print_manifold_details(result);
      }
    }
  }
}

SCENARIO("Function_ref operations", "[function-ref]")
{
  GIVEN("A simple lambda stored in a function_ref.")
  {
    auto                   increment_lambda = [](int a) { return ++a; };
    function_ref<int(int)> lambda_ref(increment_lambda);
    WHEN("Function_ref is called with 0.")
    {
      THEN("We should get 1.") { REQUIRE(lambda_ref(1) == 2); }
    }
  }
  GIVEN("A function pointer to a move stored in a function_ref.")
  {
    auto constexpr desired_simplices  = static_cast<Int_precision>(640);
    auto constexpr desired_timeslices = static_cast<Int_precision>(4);
    Manifold3 manifold(desired_simplices, desired_timeslices);
    REQUIRE(manifold.is_correct());
    function_ref<Manifold3(Manifold3&)> complex_ref(
        manifold3_moves::do_23_move);
    WHEN("The function_ref is invoked.")
    {
      auto result = complex_ref(manifold);
      result.update();
      THEN("The move from the function_ref is correct.")
      {
        CHECK(manifold3_moves::check_move(
            manifold, result, manifold3_moves::move_type::TWO_THREE));
        // Human verification
        fmt::print("Manifold properties:\n");
        print_manifold_details(manifold);
        fmt::print("Moved manifold properties:\n");
        print_manifold_details(result);
      }
    }
  }
  GIVEN("A lambda invoking a move stored in a function_ref.")
  {
    auto constexpr desired_simplices  = static_cast<Int_precision>(640);
    auto constexpr desired_timeslices = static_cast<Int_precision>(4);
    Manifold3 manifold(desired_simplices, desired_timeslices);
    REQUIRE(manifold.is_correct());
    auto const move23 = [](Manifold3& m) -> Manifold3 {
      return manifold3_moves::do_23_move(m);
    };
    function_ref<Manifold3(Manifold3&)> complex_ref(move23);
    WHEN("The function_ref is invoked.")
    {
      auto result = complex_ref(manifold);
      result.update();
      THEN(
          "The move stored in the lambda invoked by the function_ref is "
          "correct.")
      {
        CHECK(manifold3_moves::check_move(
            manifold, result, manifold3_moves::move_type::TWO_THREE));
        // Human verification
        fmt::print("Manifold properties:\n");
        print_manifold_details(manifold);
        fmt::print("Moved manifold properties:\n");
        print_manifold_details(result);
      }
    }
  }
}
