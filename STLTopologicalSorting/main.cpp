//
//  main.cpp
//  STLTopologicalSorting
//
//  Created by Saxon Nicholls on 30/3/2024.
//
//  me [at] saxonnicholls.com

#include <iostream>
#include <cassert>

#include "stl_topological_sorter.hpp"
#include "third_party/cxx-prettyprint/prettyprint.hpp"

template <typename S>
void print_stack( S&& s )
{
    snicholls::stack_helper( s, []( auto key ){ std::cout << key << std::endl; } );
}

void BasicStackExample()
{
    snicholls::topological_sorter<std::string> g;
    
    // F before C etc
    g.precede("F", "C");
    g.precede("F", "A");
    g.precede("E", "A");
    g.precede("E", "B");
    g.precede("C", "D");
    g.precede("D", "B");
    
    auto s = g.topological_sort();
    
    // F E A C D B
    print_stack(s);
}

void STLMapExample()
{
    snicholls::topological_sort_map<std::string, int> g;
    
    // F before C, E before A etc
    g.precede("F", "C");
    g.precede("F", "A");
    g.precede("E", "A");
    g.precede("E", "B");
    g.precede("C", "D");
    g.precede("D", "B");
    
    g["A"] = 0;
    g["B"] = 1;
    g["C"] = 2;
    g["D"] = 3;
    g["E"] = 4;
    g["F"] = 5;
    
    g["X"] = 100;
    g["Y"] = 101;
    g["Z"] = 102;
    
 //   g.Precede("Z", "F");
    
    auto v = g.sort();
    
    // [(F, 5), (E, 4), (A, 0), (C, 2), (D, 3), (B, 1), (X, 100), (Y, 101), (Z, 102)]
    std::cout << v << std::endl;
    
    assert( g.size() == v.size() );
}

void STLUnorderedMapExample()
{
    snicholls::topological_sort_unordered_map<std::string, int> g;
    
    // F before C, E before A etc
    g.precede("F", "C");
    g.precede("F", "A");
    g.precede("E", "A");
    g.precede("E", "B");
    g.precede("C", "D");
    g.precede("D", "B");
    
    g["A"] = 0;
    g["B"] = 1;
    g["C"] = 2;
    g["D"] = 3;
    g["E"] = 4;
    g["F"] = 5;
    
    g["X"] = 100;
    g["Y"] = 101;
    g["Z"] = 102;
    
    g.precede("Z", "F");
    
    auto v = g.sort();
    
    // [(Z, 102), (F, 5), (E, 4), (A, 0), (C, 2), (D, 3), (B, 1), (X, 100), (Y, 101)]
    std::cout << v << std::endl;
    
    assert( g.size() == v.size() );
}

void STLVectorExample()
{
    snicholls::topological_sort_vector<std::string> g;
    
    // F before C, E before A etc
    g.precede("F", "C");
    g.precede("F", "A");
    g.precede("E", "A");
    g.precede("E", "B");
    g.precede("C", "D");
    g.precede("D", "B");
    
    g.push_back("A");        g.push_back("A");       g.push_back("A");
    g.push_back("B");        g.push_back("B");
    g.push_back("C");        g.push_back("C");
    g.push_back("D");        g.push_back("D");
    g.push_back("E");        g.push_back("E");
    g.push_back("F");        g.push_back("F");       g.push_back("F");
 
    // Note that there is no g.push_back("Z"); as Z is not in the container it is ignore....
    g.precede("Z", "F");
    
    auto v = g.sort();
    // [F, F, F, E, E, A, A, A, C, C, D, D, B, B]
    std::cout << v << std::endl;
    assert( g.size() == v.size() );
    
    // Now push back Z
    g.push_back("Z");
    auto v2 = g.sort();
    // [Z, F, F, F, E, E, C, C, D, D, B, B, A, A, A]
    std::cout << v2 << std::endl;
    assert( g.size() == v2.size() );
    
    // Example 2
    snicholls::topological_sort_vector<int> g3{0,1,2,3,4,5,6,7,8,9};
    g3.precede( 9, 0 );
    g3.precede( 8, 1 );
    g3.precede( 7, 2 );
    g3.precede( 6, 3 );
    g3.precede( 5, 4);
    
    auto v3 = g3.sort();
    // [9, 0, 8, 1, 7, 2, 6, 3, 5, 4]
    std::cout << v3 << std::endl;
    assert( g3.size() == v3.size() );
}

void STLArrayExample()
{
    snicholls::topological_sort_array<std::string,9> g{ "A", "B", "C", "D", "E", "F", "X", "Y", "Z" };
    
    // F before C, E before A etc
    g.precede("F", "C");
    g.precede("F", "A");
    g.precede("E", "A");
    g.precede("E", "B");
    g.precede("C", "D");
    g.precede("D", "B");
    
    auto v = g.sort();
    
    // [F, E, A, C, D, B, X, Y, Z]
    std::cout << v << std::endl;
    assert( g.size() == v.size() );
}

int main(int argc, const char * argv[]) {
    
    BasicStackExample();
    STLMapExample();
    STLUnorderedMapExample();
    STLVectorExample();
    STLArrayExample();
    
    return 0;
}
