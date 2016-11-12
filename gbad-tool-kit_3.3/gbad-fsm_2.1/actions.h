#ifndef _ACTIONS_H_
#define _ACTIONS_H_

#include <vector>
#include <stdio.h>

#include "LabelList.h"

using namespace std;

void GP_add_xp(void *arg, int num);
void GP_add_ps(void *arg, int num);
void GP_add_vertex_i(void *arg, int v, int label);
void GP_add_vertex_f(void *arg, int v, double label);
void GP_add_vertex_s(void *arg, int v, char *label);
void GP_add_edge_i(void *arg, int type, int src, int dst, int label);
void GP_add_edge_f(void *arg, int type, int src, int dst, double label);
void GP_add_edge_s(void *arg, int type, int src, int dst, char *label);


/**
 * The following structure is used to temporaryly hold the graph read in
 * by the GP_read_graph function.  All of the labels are stored in labelList
 * when reading in a graph.  The vertices and edges in the ith XP instance
 * are stored, respectively, in vertices[i] and edges[i].  vertices[i]
 * is a vector of (vertex id, numeric label) pairs representing the vertices
 * in the ith XP instance.  The numeric label for the vertex is the index where
 * the actual vertex label is stored within the labelList.  Similarly, edges[i]
 * is a vector of ((src,dst), numeric label) pairs.  The first element in the pair
 * is also a pair that contains the source and target vertices.  The second
 * element is a numeric label which is the index of where the actual vertex
 * label is stored within the labelList.
 */
typedef struct graph_info_t
{
	LabelList *labelList;
	vector< vector< pair<unsigned int, int> > > vertices;
	vector< vector< pair< pair<unsigned int, unsigned int>, int> > > edges;
} Graph_Info;

void GP_read_graph(FILE *input, Graph_Info *GP_info);

#endif
