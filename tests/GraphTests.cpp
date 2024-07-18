#include <gtest/gtest.h>

#include <vector>
#include <list>

#include "graphs.hpp"


class GraphTests : public testing::Test {

	protected:
	AdjacentList<int, std::vector, std::list> _M_graph;		
	graph::adjacent_list _M_graph_;
};


TEST_F(GraphTests, CreateObjects)
{
	_M_graph.add_vertex(1);
}

TEST_F(GraphTests, BFS_Test)
{
//	graph::bfs(_M_graph_, _M_graph_.begin());
}
