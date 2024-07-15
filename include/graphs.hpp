#ifndef __GRAPHS__
#define __GRAPHS__

#include<memory>


template<class Value, template<class> class ArrayPolicy, 
		template<class> class ListPolicy, 
		class Allocator = std::allocator<Value>>
class AdjacentList {
	public:

	struct Vertex {
		
		using vertex_ptr = Vertex*;
		using vertex_list = ArrayPolicy<vertex_ptr>;
		using iterator = typename vertex_list::iterator;

		struct Edge {
			vertex_ptr _M_v;
		};
		
		bool operator==(const Vertex& rhs)
		{
			return _M_value == rhs._M_value;
		}

		Value _M_value;
		ListPolicy<Edge> _M_edges;

		template<typename Allocator1>
		static  vertex_ptr createVertex(Allocator1& allocator, const Value& value)
		{
			auto ptr = std::allocator_traits<Allocator1>::allocate(allocator, 1);	
			::new (static_cast<void *>(ptr)) Vertex();
			ptr->_M_value = value;
			return ptr;
		}
		template<typename Allocator1>
		static void freeVertex(Allocator1& allocator, vertex_ptr ptr)
		{
			ptr->~Vertex();
			std::allocator_traits<Allocator1>::deallocate(allocator, ptr, 1);
		}
	};

	private:

	using vertex_allocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Vertex>;
	using vertex_ptr = typename Vertex::vertex_ptr;
	using vertex_list = typename Vertex::vertex_list;
	using vertex_iterator = typename Vertex::iterator;
	public:
	

	AdjacentList() = default;

	void add_vertex(Value v)
	{
		auto ptr = Vertex::createVertex(_M_allocator, v);
		_M_graph.emplace_back(ptr);
	}

	vertex_iterator find_vertex(Value v)
	{
		return _M_graph.begin();
	}

	~AdjacentList() 
	{
		for(int i = 0; i < _M_graph.size(); ++i)
		{
			Vertex::freeVertex(_M_allocator, _M_graph[i]);
		}
	}

	private:
	vertex_list  _M_graph;
	vertex_allocator _M_allocator;

};


#include<list>
#include<queue>

namespace graph {

	struct bfs_v_properties
	{
		enum class Color : std::uint8_t {
			WHITE,
			GRAY,
			BLACK
		};

		Color _M_color;
		int _M_distance;
	};

	template<template<class> class EdgeListType, 
			typename VertexProperties>
	struct vertex {
		using pointer = vertex<EdgeListType, VertexProperties>*; 
		using properties_type = VertexProperties;
		using edge_list = EdgeListType<pointer>;
		VertexProperties _M_properties;	
		edge_list _M_pred;
		edge_list _M_edges;
		
	};

	using  adjacent_list = std::list<vertex<std::list, bfs_v_properties>::pointer>;
	
	static void bfs(adjacent_list graph, adjacent_list::iterator source)
	{
		using Color = bfs_v_properties::Color;

		for(auto& value : graph)
		{
			value->_M_properties._M_color = Color::WHITE;
			value->_M_properties._M_distance = std::numeric_limits<int>::max();
		}
		
		(*source)->_M_properties._M_color = Color::GRAY;
		(*source)->_M_properties._M_distance = 0;
		std::queue<typename adjacent_list::value_type> visited;
		visited.push(*source);
		while(!visited.empty())
		{
			auto v = visited.front();
			visited.pop();
			for(auto value : v->_M_edges)
			{
				if(value->_M_properties._M_color == Color::WHITE)
				{
					value->_M_properties._M_distance = v->_M_properties._M_distance + 1;
					value->_M_properties._M_color = Color::GRAY;
					value->_M_pred.push_back(v);
					visited.push(value);
				}
			}
			v->_M_properties._M_color = Color::BLACK;
		}
	}
};
#endif
