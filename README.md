# CDT-plusplus

[![Build Status](https://img.shields.io/travis/acgetchell/CDT-plusplus.svg?label=Linux/MacOS)](https://travis-ci.com/acgetchell/CDT-plusplus)
[![Windows Build status](https://img.shields.io/appveyor/ci/acgetchell/cdt-plusplus.svg?label=Windows)](https://ci.appveyor.com/project/acgetchell/cdt-plusplus)
[![](https://github.com/acgetchell/CDT-plusplus/workflows/C++%20CI/badge.svg?label=Actions)](https://github.com/acgetchell/CDT-plusplus/actions)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/acgetchell/CDT-plusplus.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/acgetchell/CDT-plusplus/context:cpp)
[![Language grade: Python](https://img.shields.io/lgtm/grade/python/g/acgetchell/CDT-plusplus.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/acgetchell/CDT-plusplus/context:python)
[![codecov](https://codecov.io/gh/acgetchell/CDT-plusplus/branch/develop/graph/badge.svg)](https://codecov.io/gh/acgetchell/CDT-plusplus)
[![Open Issues](https://img.shields.io/github/issues-raw/acgetchell/CDT-plusplus.svg)](https://github.com/acgetchell/CDT-plusplus/issues)
[![Join the chat at https://gitter.im/acgetchell/CDT-plusplus](https://img.shields.io/badge/gitter-join%20chat%20→-brightgreen.svg)](https://gitter.im/acgetchell/CDT-plusplus)
[![Gitpod Ready-to-Code](https://img.shields.io/badge/Gitpod-ready--to--code-blue?logo=gitpod)](https://gitpod.io/#https://github.com/acgetchell/CDT-plusplus)

**Quantize spacetime on your laptop.**

![Small foliated Delaunay triangulation](docs/images/t8-v68-s298.png "8 timeslices 68 vertices 298 simplices")

## Introduction

For an introduction to [Causal Dynamical Triangulations](https://github.com/acgetchell/CDT-plusplus/wiki),
including the foundations and recent results, please see the [wiki](https://github.com/acgetchell/CDT-plusplus/wiki).

[Causal Dynamical Triangulations][CDT] in [C++] uses the
[Computational Geometry Algorithms Library][CGAL], [Boost], [TBB], and [Eigen].
Arbitrary-precision numbers and functions are by [MPFR] and [GMP].
[Docopt] provides a beautiful command-line interface.
[Melissa E. O'Neill's Permuted Congruential Generators][PCG] library provides high-quality RNGs that pass L'Ecuyer's
[TestU01] statistical tests.
[Catch] provides [BDD]/[TDD].
[vcpkg] provides library management and building.
[Doxygen] provides automated document generation.
[{fmt}] provides a safe and fast alternative to `iostream`.
[PVS-Studio] and [LGTM] provide commercial-grade static analysis and security checks.
[CometML] provides machine learning for model building.

## Goals

- [x] Develop with [literate programming] using [Doxygen]
- [x] [Efficient Pure Functional Programming in C++ Using Move Semantics][functional]
- [x] Test using [CTest]
- [x] Develop using Behavior-driven development ([BDD]) with [Catch]
- [x] Continuous integration by [Travis-CI] on MacOS and Linux with [gcc]/[Clang]
- [x] Continuous integration by [AppVeyor] on Windows with [MSVC]
- [x] Continuous integration by [Github Actions] on the leading edge
- [x] 3D Simplex
- [x] 3D Spherical triangulation
- [x] 2+1 foliation
- [x] Integrate [Docopt] CLI
- [x] S3 Bulk action
- [x] 3D Ergodic moves
- [x] High-quality Random Number Generation with M.E. O'Neill's [PCG] library
- [x] Multithreading via [TBB]
- [x] Automated code analysis with [LGTM]
- [x] Build/debug with [Visual Studio 2019]
- [x] Use [{fmt}] library (instead of `iostream`)
- [x] Static code analysis with [PVS-Studio]
- [ ] Metropolis algorithm
- [ ] Output via [HDF5]
- [ ] A [Surface mesh] manifold of 3D Triangulation
- [ ] 4D Simplex
- [ ] 4D Spherical triangulation
- [ ] 3+1 foliation
- [ ] S4 Bulk action
- [ ] 4D Ergodic moves
- [ ] Initialize two masses
- [ ] Shortest path algorithm
- [ ] Einstein tensor
- [ ] Complete test coverage
- [ ] Complete documentation
- [ ] Quantize Spacetime!

## Quickstart

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/acgetchell/CDT-plusplus)

## Setup

This project uses [CMake]+[Ninja] to build and [vcpkg] to manage C++ libraries. Using [C++]17 features, it successfully
builds with AppleClang, [gcc-10], [clang-10], and [Visual Studio 2019].

### Short

If you use [Docker]:

```bash
docker pull acgetchell/cdt-plusplus
docker run -it --name cdt cdt-plusplus
```
Binaries will be in `/CDT-plusplus/build/src`.
Proceed to [Use](#use).

### Long

On macOS or Linux, you will first need to install some prerequisites using your favorite
package manager (e.g. [homebrew] or [apt]):

- build-essential (Linux only)
- automake
- autoconf
- autoconf-archive
- libtool (macOS) or libtool-bin (Linux)
- texinfo
- yasm
- ninja (macOS) or ninja-build (Linux)

Next, install [vcpkg]:

```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh -useSystemBinaries
./vcpkg integrate install
```

[vcpkg] will then be invoked by CMake in [vcpkg manifest] mode and install the project dependencies
listed in [vcpkg.json] into a local `vcpkg_installed` directory.

## Build

Clone the repo:

~~~
git clone https://github.com/acgetchell/CDT-plusplus.git
~~~

To get [CMake] and the build scripts to run correctly, you'll need to set `$VCPKG_ROOT` to wherever you cloned
[vcpkg], e.g.

```bash
export VCPKG_ROOT="$HOME/vcpkg"
```

This will set the `CMAKE_TOOLCHAIN_FILE` option for [CMake].

You can optionally pre-build the project dependencies (100+ packages) by running at the top level of the project:

```
vcpkg install --feature-flags=manifests
```

### Project Layout

The project is organized similar to the [PitchFork Layout], as follows:

- .github - GitHub specific settings
- build - Ephemeral out-of-source build directory
- cmake - Cmake configurations
- docs - Documentation
- external - Includes submodules of external projects (none so far, all using [vcpkg])
- include - Header files
- scripts - Build, test, and run scripts
- src - Source files
- tests - Unit tests

### Run
If you want to get started right away, run `fast-build.sh` or `fast-build.bat`, depending on your operating system,
from `scripts`. This will compile the appropriate executables in `RELEASE` mode with no tests.

This should result in the main program executable, `cdt` in `build/src` or `build\Debug`,
along with several others.

- `cdt-gv` converts output files to [Geomview] format for [visualization](#visualize)
- `cdt-opt` is a simplified version with hard-coded inputs, mainly useful for debugging and scripting
- `initialize` is used by [CometML] to run [parameter optimization](#optimize-parameters)

## Use

CDT-plusplus uses [Docopt] to parse options from the help message, and so
understands long or short argument formats, provided the short argument given
is an unambiguous match to a longer one. The help message should be instructive:

~~~text
./build/cdt --help
Causal Dynamical Triangulations in C++ using CGAL.

Copyright (c) 2014-2021 Adam Getchell

A program that generates d-dimensional triangulated spacetimes
with a defined causal structure and evolves them according
to the Metropolis algorithm. Specify the number of passes to control
how much evolution is desired. Each pass attempts a number of ergodic
moves equal to the number of simplices in the simulation.

Usage:./cdt (--spherical | --toroidal) -n SIMPLICES -t TIMESLICES [-d DIM] -k K --alpha ALPHA --lambda LAMBDA [-p PASSES] [-c CHECKPOINT]

Examples:
./cdt --spherical -n 32000 -t 11 --alpha 0.6 -k 1.1 --lambda 0.1 --passes 1000
./cdt --s -n32000 -t11 -a.6 -k1.1 -l.1 -p1000

Options:
  -h --help                   Show this message
  --version                   Show program version
  -n SIMPLICES                Approximate number of simplices
  -t TIMESLICES               Number of timeslices
  -d DIM                      Dimensionality [default: 3]
  -a --alpha ALPHA            Negative squared geodesic length of 1-d
                              timelike edges
  -k K                        K = 1/(8*pi*G_newton)
  -l --lambda LAMBDA          K * Cosmological constant
  -p --passes PASSES          Number of passes [default: 100]
  -c --checkpoint CHECKPOINT  Checkpoint every n passes [default: 10]
~~~

The dimensionality of the spacetime is such that each slice of spacetime is
`d-1`-dimensional, so setting `d=3` generates 2 spacelike dimensions and one
timelike dimension, with a defined global time foliation. Thus a
`d`-dimensional simplex will have some `d-1` sub-simplices that are purely
spacelike (all on the same timeslice) as well as some that are timelike
(span two timeslices). In [CDT] we actually care more about the timelike
links (in 2+1 spacetime) and the timelike faces (in 3+1 spacetime).

## Document

Online documentation may be found at <https://adamgetchell.org/CDT-plusplus/> automatically
generated by [Travis-CI].

If you have [Doxygen] installed you can generate the same information
locally using the configuration file in `docs\Doxyfile` by simply typing at the top
level directory ([Doxygen] will recursively search):

~~~bash
doxygen ./docs/Doxyfile
~~~

This will generate a `docs/html/` directory containing
documentation generated from CDT++ source files. `USE_MATHJAX` has been enabled
in [Doxyfile] so that the LaTeX formulae can be rendered in the html
documentation using [MathJax]. `HAVE_DOT` is set to **YES** which allows
various graphs to be autogenerated by [Doxygen] using [GraphViz].
If you do not have GraphViz installed, set this option to **NO**
(along with `UML_LOOK`).

## Test

In the `scripts` directory, run `build.sh` or `build.bat` depending on your operating system.

Unit tests are run (in `build/tests` or `build\tests\Debug`) via `CDT_test`, the [Catch] executable:

~~~bash
./CDT_test
~~~

or (Windows):

~~~cmd
CDT_test.exe
~~~

You can also run both [CTest] integration and [Catch] unit tests in the `build` directory with:

~~~bash
cmake --build . --target test
~~~

or (Windows):

~~~
ctest -C Debug
~~~

In addition to the command line output, you can see detailed results in the
`build/Testing` directory which is generated thereby.

Unit tests are turned off with `-D ENABLE_TESTING:BOOL=FALSE`, e.g. `scripts/fast-build.sh`.

### Static Analysis

This project follows the [CppCore Guidelines][guidelines] as enforced by [ClangTidy], which you can install
and then run using the [clang-tidy.sh] script:

~~~bash
sudo apt-get install clang-tidy
cd scripts
./clang-tidy.sh
~~~

(Or use your favorite linter plugin for your editor/IDE.)

The [cppcheck.sh] script runs a quick static analysis using [cppcheck].

~~~bash
brew install cppcheck
cd scripts
./cppcheck-build.sh
~~~

[Clang] comes with [scan-build] which can run a much more thorough,
but slower static analysis integrated with [CMake] and [Ninja].

~~~bash
./scan.sh
~~~

### Sanitizers

[AddressSanitizer] + [UndefinedBehaviorSanitizer], [LeakSanitizer], [MemorySanitizer],
and [ThreadSanitizer] may be run with `scripts/asan.sh`, `scripts/lsan.sh`, `scripts/msan.sh`,
and `scripts/tsan.sh`. They are also checked by [Travis-CI] during commits.

## Optimize Parameters

[CometML] is used to record [Experiments] which conduct [Model Optimization]. The script to do
this is `optimize-initialize.py`. In order for this to work, you must install the following
into your Python [virtual environment].

~~~bash
pip install tensorflow
pip install comet-ml
~~~

You can then run experiments and look at results on https://www.comet.ml!

## Visualize

[Geomview] is used to generate pictures of triangulations using the `cdt-gv` binary. In order for this to
work, you must have [Geomview] installed (which doesn't work on Windows). On MacOS:

~~~bash
brew cask install xquartz
brew install geomview
~~~

If you get a `Can't open display` problem, look at the [Geomview FAQ].

## Contribute

Please see [CONTRIBUTING.md] and our [CODE_OF_CONDUCT.md].

Your code should pass Continuous Integration:

- Whitespace formatting (`git diff --check HEAD^`)

- [clang-format] with project settings ([.clang-format])

- [cppcheck]; test with [cppcheck.sh]

- [Valgrind]; be sure to look at the results to ensure you're not leaking memory

- [AddressSanitizer] and [UndefinedBehaviorSanitizer]; test with [asan.sh]

- [LeakSanitizer] (if supported by your platform); test with [lsan.sh]

- [MemorySanitizer] (if supported by your platform); test with [msan.sh]

- [ThreadSanitizer]; test with [tsan.sh]

- [LGTM]; check to ensure you haven't introduced a security vulnerability. Look at the [query console] for
  more details.

Optional:

- [ClangTidy] on all changed files

- [PVS-Studio] using [pvs-studio.sh] if you have it installed

## Issues

[Eigen] does not work for c++20 and MSVC 2019 (issue [#1894]).

[CLion] skips debugger breakpoints (issue [CPP-20158]).

[AppVeyor] is broken due to a bug in [vcpkg manifest] mode, awaiting a merge of [16478].

[CDT]: https://arxiv.org/abs/hep-th/0105267
[CGAL]: https://www.cgal.org
[CMake]: https://www.cmake.org
[Clang]: https://clang.llvm.org
[gcc]: https://gcc.gnu.org/
[Catch]: https://github.com/catchorg/Catch2/blob/master/docs/Readme.md
[guidelines]: https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines
[clang-tidy.sh]: https://github.com/acgetchell/CDT-plusplus/blob/master/clang-tidy.sh
[CTest]: https://gitlab.kitware.com/cmake/community/wikis/doc/ctest/Testing-With-CTest
[Travis-CI]: https://travis-ci.com/getting_started
[literate programming]: http://www.literateprogramming.com
[Doxygen]: http://www.doxygen.org
[Homebrew]: https://brew.sh
[Ninja]: https://ninja-build.org
[Docopt]: https://github.com/docopt/docopt.cpp
[Mathjax]: https://www.mathjax.org
[GraphViz]: https://www.graphviz.org
[Eigen]: http://eigen.tuxfamily.org/index.php?title=Main_Page
[CMakeLists.txt]: https://github.com/acgetchell/CDT-plusplus/blob/master/CMakeLists.txt
[MPFR]: https://www.mpfr.org
[GMP]: https://gmplib.org
[HDF5]: https://www.hdfgroup.org
[scan-build]: https://clang-analyzer.llvm.org/scan-build.html
[scan.sh]: https://github.com/acgetchell/CDT-plusplus/blob/master/scan.sh
[cppcheck]: http://cppcheck.sourceforge.net
[cppcheck.sh]: https://github.com/acgetchell/CDT-plusplus/blob/develop/scripts/cppcheck.sh
[functional]: https://blog.knatten.org/2012/11/02/efficient-pure-functional-programming-in-c-using-move-semantics/
[TBB]: https://www.threadingbuildingblocks.org
[CDT++]: https://github.com/acgetchell/CDT-plusplus
[.travis.yml]: https://github.com/acgetchell/CDT-plusplus/blob/master/.travis.yml
[Doxyfile]: https://github.com/acgetchell/CDT-plusplus/blob/master/Doxyfile
[Boost]: https://www.boost.org
[contrib]: https://github.com/acgetchell/CDT-plusplus/blob/master/CONTRIBUTING.md
[ClangTidy]: https://releases.llvm.org/6.0.1/tools/clang/tools/extra/docs/clang-tidy/index.html
[Valgrind]: http://valgrind.org/docs/manual/quick-start.html#quick-start.mcrun
[conduct]: https://github.com/acgetchell/CDT-plusplus/blob/master/CODE_OF_CONDUCT.md
[date]: https://howardhinnant.github.io/date/date.html
[AppVeyor]: https://www.appveyor.com
[BDD]: https://en.wikipedia.org/wiki/Behavior-driven_development
[TDD]: https://en.wikipedia.org/wiki/Test-driven_development
[.appveyor.yml]: https://github.com/acgetchell/CDT-plusplus/blob/master/.appveyor.yml
[LGTM]: https://lgtm.com/projects/g/acgetchell/CDT-plusplus/
[CometML]: https://www.comet.ml/
[Experiments]: https://www.comet.ml/acgetchell/cdt-plusplus
[Model Optimization]: https://www.comet.ml/parameter-optimization
[virtual environment]: https://docs.python.org/3/tutorial/venv.html
[vcpkg]: https://github.com/Microsoft/vcpkg
[clang-10]: https://releases.llvm.org/10.0.0/tools/clang/docs/ReleaseNotes.html
[gcc-10]: https://gcc.gnu.org/gcc-10/
[C++]: https://isocpp.org/
[Geomview]: http://www.geomview.org/
[Geomview FAQ]: http://www.geomview.org/FAQ/answers.shtml
[development]: https://github.com/acgetchell/CDT-plusplus
[Pitchfork Layout]: https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs#tld.docs
[Surface mesh]: https://doc.cgal.org/latest/Surface_mesher/index.html
[PCG]: http://www.pcg-random.org/paper.html
[TestU01]: http://simul.iro.umontreal.ca/testu01/tu01.html
[apt]: https://wiki.debian.org/Apt
[ms-gsl]: https://github.com/microsoft/GSL
[yasm-tool:x86-windows]: https://github.com/microsoft/vcpkg/issues/15956#issuecomment-782370823
[MSVC]: https://docs.microsoft.com/en-us/cpp/build/reference/compiling-a-c-cpp-program?view=vs-2019
[m4]: https://www.gnu.org/software/m4/
[1]: https://github.com/microsoft/vcpkg/issues/9082
[2]: https://github.com/microsoft/vcpkg/issues/9087
[3]: https://github.com/microsoft/vcpkg/issues/8627
[CONTRIBUTING.md]: https://github.com/acgetchell/CDT-plusplus/blob/develop/.github/CONTRIBUTING.md
[CODE_OF_CONDUCT.md]: https://github.com/acgetchell/CDT-plusplus/blob/develop/.github/CODE_OF_CONDUCT.md
[query console]: https://lgtm.com/query/lang:cpp/
[Github Actions]: https://github.com/features/actions
[Visual Studio 2019]: https://visualstudio.microsoft.com/vs/
[{fmt}]: https://github.com/fmtlib/fmt
[AddressSanitizer]: https://github.com/google/sanitizers/wiki/AddressSanitizer
[LeakSanitizer]: https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer
[ThreadSanitizer]: https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual
[MemorySanitizer]: https://github.com/google/sanitizers/wiki/MemorySanitizer
[UndefinedBehaviorSanitizer]: https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
[clang-format]: https://releases.llvm.org/10.0.0/tools/clang/docs/ReleaseNotes.html#clang-format
[.clang-format]: https://github.com/acgetchell/CDT-plusplus/blob/develop/.clang-format
[asan.sh]: https://github.com/acgetchell/CDT-plusplus/blob/develop/scripts/asan.sh
[lsan.sh]: https://github.com/acgetchell/CDT-plusplus/blob/develop/scripts/lsan.sh
[msan.sh]: https://github.com/acgetchell/CDT-plusplus/blob/develop/scripts/msan.sh
[tsan.sh]: https://github.com/acgetchell/CDT-plusplus/blob/develop/scripts/tsan.sh
[PVS-Studio]: https://www.viva64.com/en/pvs-studio/
[pvs-studio.sh]: https://github.com/acgetchell/CDT-plusplus/blob/develop/scripts/pvs-studio.sh
[#1894]: https://gitlab.com/libeigen/eigen/-/issues/1894
[CLion]: https://www.jetbrains.com/clion/
[CPP-20158]: https://youtrack.jetbrains.com/issue/CPP-20158
[Docker]: https://www.docker.com/
[GitPod]: https://gitpod.io
[vcpkg manifest]: https://github.com/microsoft/vcpkg/blob/master/docs/users/manifests.md
[vcpkg.json]: https://github.com/acgetchell/CDT-plusplus/blob/develop/vcpkg.json
[16478]: https://github.com/microsoft/vcpkg/pull/16478