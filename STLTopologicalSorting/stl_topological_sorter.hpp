//
//  stl_topological_sorter.hpp
//  STLTopologicalSorting
//
//  Created by Saxon Nicholls on 30/3/2024.
//
//  me [at] saxonnicholls.com

#ifndef stl_topological_sorter_hpp
#define stl_topological_sorter_hpp

#include <stack>
#include <map>
#include <unordered_map>
#include <vector>
#include <array>
#include <utility>
#include <algorithm>

//
// Header only adapter to enable topological sorting of STL containers
// We only include the commonly used containers - std::map, std::unordered_map, std::vector, std::array - easy to generalise to the rest of the STL library
//

namespace snicholls {

    // S is some sort of stack 
    // TODO: add concepts / requires to ensure that empty(), top() and pop() are defined
    // Any Callable with Args if we want them
    template <typename S, typename Callable, typename ... Args >
    inline
    constexpr
    void stack_helper( S&& s,  Callable&& f, Args&&... args )
    {
        while (s.empty() == false) {
            f( s.top(), std::forward<Args>(args)... );
            s.pop();
        }
    }

    // Note: we are NOT checking for cycles
    // Complexity O(V+E) where V are the number of vertices in the DAG and E is the number of edges
    template <typename Key>
    struct topological_sorter
    {
        using stack_type = std::stack< Key >;
        using visited_type = std::map< Key, bool>;
        using adjacency_type = std::map< Key, std::vector<Key> >;
         
        // result type for an associative container - std::map, std::unordered_map
        template <typename T>
        using associative_sort_type = std::vector< std::pair<const Key, T> >;
        
        // result type for std::vector
        template <typename T>
        using vector_sort_type = std::vector< T >;
        
        // result type for std::array
        template <typename T, std::size_t N>
        using array_sort_type = std::array< T, N >;
        
        adjacency_type adj;
        
        ~topological_sorter() {};
        
        // Means v must occur before w
        // Use this method to form the Directed Acyclic Graph ("DAG")
        // Note that these elements are not automatically inserted into the container - this is by design
        // The DAG represents a constraint
        // We cleanly seperate the contents of the container from the constraints that we impose on the container
        constexpr
        void precede( Key v, Key w )
        {
            adj[v].push_back(w); // All w's must come after v
        }
       
        // Note may be stack heavy for very large DAGs ... in practice not found this to be an issue...
        void sort_util( Key v, visited_type& visited, stack_type& s )
        {
            visited[v] = true;

            // Note that if a key does not exist in the adjacency map - we create it - function is deliberately non-const
            for ( const auto& i : adj[v] )
                if (!visited[i])
                    sort_util( i, visited, s );
            
            s.push(v);
        }

        // Note that this is non-const - it is by design for use cases where we repeatedly call topological_sort - marginal increase in performance for sort_util above
        stack_type topological_sort()
        {
            // Mark all the vertices as not visited - std::map<Key,bool> defaults to false
            visited_type visited;
            stack_type s;
            
            for ( const auto& [key, value] : adj )
                if (visited[key] == false)
                   sort_util( key, visited, s );
            
            return s;
        }
    };

    //
    // Associative containers
    //
    // std::map - drop in replacement
    // when sort is called - returns a std::vector sorted according to the DAG
    //
    //

    template<
        class Key,
        class T,
        class Compare = std::less<Key>,
        class Allocator = std::allocator<std::pair<const Key, T>> >
    struct topological_sort_map : 
        std::map< Key, T, Compare, Allocator>,
        topological_sorter< Key >
    {
        // Adapter types
        using container = std::map< Key, T, Compare, Allocator>;
        using sorter    = topological_sorter< Key >;
        using sort_type = typename sorter::template associative_sort_type<T>;
        using visited_type = typename sorter::visited_type;
        
        // STL types
        using key_type          = typename container::key_type;
        using mapped_type       = typename container::mapped_type;
        using value_type        = typename container::value_type;
        using size_type         = typename container::size_type;
        using difference_type   = typename container::difference_type;
        using key_compare       = typename container::key_compare;
        using allocator_type    = typename container::allocator_type;
        using reference         = typename container::reference;
        using const_reference   = typename container::const_reference;
        using pointer           = typename container::pointer;
        using const_pointer     = typename container::const_pointer;
        using iterator          = typename container::iterator;
        using const_iterator    = typename container::const_iterator;
        using reverse_iterator  = typename container::reverse_iterator;
        using const_reverse_iterator    = typename container::const_reverse_iterator;
        using node_type         = typename container::node_type;
        
        // Forwarding constructor
        template <typename...Xs>
        topological_sort_map( Xs&&...xs ) : container{ std::forward<Xs>(xs)... } {};
        
        // Destructor
        ~topological_sort_map() {};
        
        sort_type sort()
        {
            // Do the topological sort
            auto s = this->sorter::topological_sort();
            
            // Now return our ordered vector
            sort_type result;
            // Has this particular key been copied to the result ?
            visited_type copied;
            
            // First copy in the elements from the topological sort using stack helper
            stack_helper( s,
                         [&](const auto key) {
                auto value = this->container::at( key );
                result.emplace_back( std::make_pair( key, value ) );
                copied[key] = true;
                        } );
            
            // Now copy the rest make sure that we haven't missed anything
            // By defintion - we put these last since putting them first may violate other topological constraints - eg sort is called before precede...
            for (const auto& [key,value] : *this )
            {
                if ( copied[key] == false )
                {
                    copied[key] = true;
                    result.emplace_back( std::make_pair(key, value) );
                }
            }
            return result;
        }
    };  // struct topological_sort_map

    //
    // std::unordered map drop in replacement
    // when sort is called - returns a std::vector sorted according to the DAG
    //

    template<
        class Key,
        class T,
        class Hash = std::hash<Key>,
        class KeyEqual = std::equal_to<Key>,
        class Allocator = std::allocator<std::pair<const Key, T>> >
    struct topological_sort_unordered_map :
        std::unordered_map< Key, T, Hash, KeyEqual, Allocator >,
        topological_sorter< Key >
    {
        // Adapter types
        using container = std::unordered_map< Key, T, Hash, KeyEqual, Allocator >;
        using sorter    = topological_sorter< Key >;
        using sort_type = typename sorter::template associative_sort_type<T>;
        using visited_type = typename sorter::visited_type;
        
        // STL types
        using key_type          = typename container::key_type;
        using mapped_type       = typename container::mapped_type;
        using value_type        = typename container::value_type;
        using size_type         = typename container::size_type;
        using difference_type   = typename container::difference_type;
        using hasher            = typename container::hasher;
        using key_equal         = typename container::key_equal;
        using allocator_type    = typename container::allocator_type;
        using reference         = typename container::reference;
        using const_reference   = typename container::const_reference;
        using pointer           = typename container::pointer;
        using const_pointer     = typename container::const_pointer;
        using iterator          = typename container::iterator;
        using const_iterator    = typename container::const_iterator;
        using local_iterator    = typename container::local_iterator;
        using const_local_iterator    = typename container::const_local_iterator;
        using node_type         = typename container::node_type;
        using insert_return_type= typename container::insert_return_type;
        
        // Forwarding constructor
        template <typename...Xs>
        topological_sort_unordered_map( Xs&&...xs ) : container{ std::forward<Xs>(xs)... } {};
        
        // Destructor
        ~topological_sort_unordered_map() {};
        
        sort_type sort()
        {
            // Do the topological sort
            auto s = this->sorter::topological_sort();
            
            // Now return our ordered vector
            sort_type result;
            // Has this particular key been copied to the result ?
            visited_type copied;
            
            // First copy in the elements from the topological sort using stack helper
            stack_helper( s,
                         [&](const auto key) {
                auto value = this->container::at( key );
                result.emplace_back( std::make_pair( key, value ) );
                copied[key] = true;
                        } );
            
            // Now copy the rest make sure that we haven't missed anything
            // By defintion - we put these last since putting them first may violate other topological constraints - eg sort is called before precede...
            for (const auto& [key,value] : *this )
            {
                if ( copied[key] == false )
                {
                    copied[key] = true;
                    result.emplace_back( std::make_pair(key, value) );
                }
            }
            return result;
        }
    };  // struct topological_sort_unordered_map

    //
    // Sequence containers
    // std::vector
    //

    template<
        class T,
        class Allocator = std::allocator<T>
    > struct topological_sort_vector : 
        std::vector<T, Allocator>,
        topological_sorter< T >
    {
        // Adapter types
        using container = std::vector< T, Allocator>;
        using sorter    = topological_sorter< T >;
        using sort_type = typename sorter::template vector_sort_type<T>;
        using visited_type = typename sorter::visited_type;
        
        // STL types
        using value_type        = typename container::value_type;
        using size_type         = typename container::size_type;
        using difference_type   = typename container::difference_type;
        using allocator_type    = typename container::allocator_type;
        using reference         = typename container::reference;
        using const_reference   = typename container::const_reference;
        using pointer           = typename container::pointer;
        using const_pointer     = typename container::const_pointer;
        using iterator          = typename container::iterator;
        using const_iterator    = typename container::const_iterator;
        using reverse_iterator  = typename container::reverse_iterator;
        using const_reverse_iterator    = typename container::const_reverse_iterator;
      
        // Forwarding constructor
        template <typename...Xs>
        topological_sort_vector( Xs&&...xs ) : container{ std::forward<Xs>(xs)... } {};
        
        // Destructor
        ~topological_sort_vector() {};
        
        sort_type sort()
        {
            // Do the topological sort
            auto s = this->sorter::topological_sort();
            
            // Now return our ordered vector
            sort_type result;
            // Has this particular key been copied to the result ?
            visited_type copied;
            
            // First copy in the elements from the topological sort using stack helper
            stack_helper( s,
                         [&](const auto key) {
                        // Make sure that our result is the same length - if key occurs n times, insert it n times - if a key does not occur at all - ignore it
                        auto n = std::count( this->begin(), this->end(), key );
                        for (auto i{0};i<n;++i) result.emplace_back( key );
                        copied[key] = true;
                        } );
            
            // Now copy the rest make sure that we haven't missed anything
            // By defintion - we put these last since putting them first may violate other topological constraints - eg sort is called before precede...
            for (const auto& key : *this )
            {
                if ( copied[key] == false )
                {
                    copied[key] = true;
                    auto n = std::count( this->begin(), this->end(), key );
                    for (auto i{0};i<n;++i) result.emplace_back( key );
                }
            }
            return result;
        }
    }; // struct topological_sort_vector
 
    //
    // Sequence containers
    // std::array
    //

    template<
        class T,
        std::size_t N
    > struct topological_sort_array  :
        std::array<T, N>,
        topological_sorter< T >
    {
        // Adapter types
        using container = std::array< T, N>;
        using sorter    = topological_sorter< T >;
        using sort_type = typename sorter::template array_sort_type<T, N>;
        using visited_type = typename sorter::visited_type;
        
        // STL types
        using value_type        = typename container::value_type;
        using size_type         = typename container::size_type;
        using difference_type   = typename container::difference_type;
        using reference         = typename container::reference;
        using const_reference   = typename container::const_reference;
        using pointer           = typename container::pointer;
        using const_pointer     = typename container::const_pointer;
        using iterator          = typename container::iterator;
        using const_iterator    = typename container::const_iterator;
        using reverse_iterator  = typename container::reverse_iterator;
        using const_reverse_iterator    = typename container::const_reverse_iterator;
        
        // Forwarding constructor
        template <typename...Xs>
        topological_sort_array( Xs&&...xs ) : container{ std::forward<Xs>(xs)... } {};
        
        // Destructor
        ~topological_sort_array() {};
        
        sort_type sort()
        {
            // Do the topological sort
            auto s = this->sorter::topological_sort();
            
            // Now return our ordered array
            sort_type result;
            // Has this particular key been copied to the result ?
            visited_type copied;
            
            // First copy in the elements from the topological sort using stack helper
            std::size_t index{0};
            stack_helper( s,
                         [&](const auto key) {
                        // Make sure that our result is the same length - if key occurs n times, insert it n times - if a key does not occur at all - ignore it
                        auto n = std::count( this->begin(), this->end(), key );
                        for (auto i{0};i<n;++i) result[index++] = key;
                        copied[key] = true;
                        } );
            
            // Now copy the rest make sure that we haven't missed anything
            // By defintion - we put these last since putting them first may violate other topological constraints - eg sort is called before precede...
            for (const auto& key : *this )
            {
                if ( copied[key] == false )
                {
                    copied[key] = true;
                    auto n = std::count( this->begin(), this->end(), key );
                    for (auto i{0};i<n;++i) result[index++] = key;
                }
            }
            return result;
        }
    }; // topological_sort_array
} // namespace snicholls

#endif /* stl_topological_sorter_hpp */
