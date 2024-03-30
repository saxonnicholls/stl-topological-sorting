# Background

The C++ STL library is very handy, however it does not make it easy to topologically sort an STL container. This is a header-only library providing drop in substitutes for std::map, std::unordered_map, std::vector and std::array. A single method **void** **precede**( Key v, Key w ) is provided to construct the directed acyclic graph (DAG). Using boost topological_sort can be cumbersome for many use cases - it was felt that this approach is easier. The emphasis here is on speed and simplicity.

It should be clear to the reader how to generalise the approach utilised here to the rest of the STL library. Working examples are provided. Tested on clang **17.0.6** and gcc **13.2** .

# Complexity

Complexity is O(V + E) where V is the number of vertices and E the number of elements in the DAG.

# Design

To the extent that the DAG represents a set of constraints on the sorting of the container, we cleanly seperate the constraints from the contents of the container. This is deliberate.

# Ongoing work

When I get some time, I will extend this to the rest of the STL library and work on some **constexpr** cases.

me [at] saxonnicholls.com 

Saxon Nicholls

March 2024
