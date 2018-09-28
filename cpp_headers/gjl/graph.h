// -*-c++-*-
#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
//#include <list>
#include <unordered_map>
#include <map>
#include <set>
#include <vector>
/*****************************************************
* Copyright 2014 John Lapeyre                        *
*                                                    *
* You can redistribute this software under the terms *
* of the GNU General Public License version 3        *
* or a later version.                                *
******************************************************/

/*
  Sun Nov  9 15:57:53 CET 2014
  class gjl::Graph --- Simple graph class to do vertex coloring.
  The graph is stored as a vertex list and an adjacency list for
  each vertex. The coloring algorithm is a simple greedy algorithm.

  I was unable to find any graph class, let alone a simple one, that
  has a vertex coloring algorithm.

  The code for the greedy algorithm is modified from code found on the internet.
  Perhaps I can locate the author if this file is to be made public.
*/

typedef size_t vert_t;
//typedef std::unordered_map<vert_t, bool> edge_list_t;
typedef std::set<vert_t> edge_list_t;

typedef int color_t;

class Graph
{

public:
  inline void set_num_vertices(size_t n) { adj_.resize(n);}
  inline void add_vertex(size_t n) { for(size_t i=0;i<n;++i) adj_.push_back(edge_list_t ());}
  inline void add_vertex() { adj_.push_back(edge_list_t ());}
  inline void add_edge(vert_t v, vert_t w);
  inline void color_vertices();
  inline void print_vertex_colors();
  inline size_t n_vertices() const {return adj_.size();}
  inline size_t n_edges(vert_t i) const {return adj_[i].size();}
  inline size_t n_edges() const {return n_edges_;}
  inline int vertex_color(int i) const { return vertex_colors_[i];}
  inline size_t n_colors() const {return num_colors_;}
  inline void clear() {adj_.clear();vertex_colors_.clear(); num_colors_=0; n_edges_ = 0;}

  inline std::vector<edge_list_t>::iterator  vertices_begin() {return adj_.begin();}
  inline std::vector<edge_list_t>::iterator  vertices_end() {return adj_.end();}

  inline edge_list_t::iterator edges_begin(vert_t i) {return adj_[i].begin();}
  inline edge_list_t::iterator edges_end(vert_t i) {return adj_[i].end();}

private:
  //  std::vector<std::unordered_map<vert_t, bool>> adj_;
  std::vector<edge_list_t> adj_;
  std::vector<color_t> vertex_colors_;
  size_t num_colors_ = 0;
  size_t n_edges_ = 0;
};  /* End class Graph */

void Graph::add_edge(size_t v, size_t w)
{
  adj_[v].insert(w);
  adj_[w].insert(v);
  n_edges_++;
}

/*
  Assigns colors (starting from 0) to all vertices and prints
  the assignment of colors.
*/
void Graph::color_vertices()
{
  const int N = adj_.size();
  int max_color = 0;
  vertex_colors_.resize(N);
  vertex_colors_[0]  = 0;

  for (int u = 1; u < N; u++)
    vertex_colors_[u] = -1;  // no color is assigned to u

  /*
    A temporary array to store the available colors. True
    value of available[cr] would mean that the color cr is
    assigned to one of its adjacent vertices
  */
  std::vector<bool> available(N); // strict warnings does not like array
  for (int cr = 0; cr < N; cr++)
    available[cr] = false;

    // Assign colors to remaining N-1 vertices
    for (int u = 1; u < N; u++)
    {
        // Process all adjacent vertices and flag their colors
        // as unavailable

      for (auto i = edges_begin(u); i != edges_end(u); ++i)
            if (vertex_colors_[*i] != -1)
                available[vertex_colors_[*i]] = true;

        // Find the first available color
        int cr;
        for (cr = 0; cr < N; cr++)
            if (available[cr] == false)
                break;

        vertex_colors_[u] = cr; // Assign the found color
        if (cr > max_color) max_color = cr;

        // Reset the values back to false for the next iteration
        for (auto i = edges_begin(u); i != edges_end(u); ++i)
            if (vertex_colors_[*i] != -1)
                available[vertex_colors_[*i]] = false;
    }
    num_colors_ = max_color+1;
}

void Graph::print_vertex_colors() {
    // print the vertex_colors_
  int N = adj_.size();
    for (int u = 0; u < N; u++)
      std::cout << "Vertex " << u << " --->  Color "
                << vertex_color(u) << std::endl;
}

#endif
