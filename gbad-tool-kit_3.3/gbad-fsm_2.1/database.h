//******************************************************************************
// database.h
//
// GBAD-FSM, version 0.1
//
// Date      Name       Description
// ========  =========  ========================================================
// 09/28/10  Eberle     Initial version, taken from GASTON.
//                      Courtesy of Siegfried Nijssen,snijssen@liacs.nl, 01/2004
//
//******************************************************************************

#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include "legoccurrence.h"
#include "misc.h"
#include "actions.h"

using namespace std;

// NOTE:  The maximum number of unique labels is 65,535, and the maximum
//        number of input nodes is 4,294,967,295.... I think...
typedef unsigned int InputEdgeLabel;
typedef unsigned int InputNodeLabel;
typedef unsigned int InputNodeId;
typedef unsigned int CombinedInputLabel;
#define combineInputLabels(label1,label2,label3) (label1 | ( ((unsigned int)label2 ) << 16 ) | ( ( (unsigned int)label3 ) << 24 ) )

#define NOINPUTEDGELABEL ((InputEdgeLabel) -1)
#define NOINPUTNODELABEL ((InputNodeLabel) -1)

template<class T>
class pvector 
{
   public:
      T *array;
      int _size;
      pvector<T> ( T *array, int _size ): array ( array ), _size ( _size ) { }
      pvector<T> () { }
      inline int size () const { return _size; }
      void resize ( int s ) { _size = s; }
      void clear () { _size = 0; } // cannot remove allocation, as we are not managing that memory here 
      T &operator[] ( int i ) { return array[i]; }
};

struct DatabaseTreeEdge 
{
   EdgeLabel edgelabel;
   NodeId tonode;
   Tid tid;           // transaction where this edge is from

   DatabaseTreeEdge () { }

   friend ostream &operator<< ( ostream &stream, DatabaseTreeEdge &databasetreeedge );
};

struct DatabaseTreeNode 
{
   NodeLabel nodelabel;
   bool incycle;
   Tid tid;           // transaction where this node is from

   pvector<DatabaseTreeEdge> edges;

   DatabaseTreeNode () { }

   friend ostream &operator<< ( ostream &stream, DatabaseTreeNode &databasetreenode );
};

struct DatabaseTree 
{
   Tid tid;
   vector<DatabaseTreeNode> nodes;

   DatabaseTreeEdge *edges;

   DatabaseTree ( Tid tid ): tid ( tid ) { }
   DatabaseTree () { }
  
   friend ostream &operator<< ( ostream &stream, DatabaseTree &databasetree );
};

typedef DatabaseTree *DatabaseTreePtr;

struct DatabaseNodeLabel 
{
   InputNodeLabel inputlabel;
   Frequency frequency;
   Tid lasttid;

   LegOccurrences occurrences;
   vector<EdgeLabel> frequentedgelabels;

   DatabaseNodeLabel (): frequency ( 1 ) { }
};

struct DatabaseEdgeLabel 
{
   InputEdgeLabel inputedgelabel;
   NodeLabel tonodelabel, fromnodelabel; 
   EdgeLabel edgelabel; // the (order) edge label to which this entry corresponds during the search
   Frequency frequency;
   Tid lasttid;
 
   DatabaseEdgeLabel (): frequency ( 1 ) { }
};

class Database 
{
   public:
      Database () { }
      LabelList labelList;
      vector<DatabaseTreePtr> trees;
      vector<DatabaseNodeLabel> nodelabels;
      vector<DatabaseEdgeLabel> edgelabels;
      map<InputNodeLabel,NodeLabel> nodelabelmap;
      map<CombinedInputLabel,EdgeLabel> edgelabelmap;
      vector<EdgeLabel> edgelabelsindexes; // given an edge label, returns the index of the element in edgelabels in which
                                           // all information about this edge can be found. Used during the search,
                                           // only frequent edge label, node label pairs are stored.
      EdgeLabel frequentEdgeLabelSize () const { return edgelabelsindexes.size (); }

      // NOTE! In the input file, the nodes MUST be listed in pre-order.

      // "read" reads the input file, and determines the frequency counts for node labels and (combined) edge labels
      void read ( FILE *input );

      // after "read", determines the frequency of edges, using DatabaseNodeLabel's edgelasttid/edgelabelfrequency
      void edgecount ();

      // after "edgecount",
      // - removes infrequent data
      // - cleans up the datastructures used until now for counting frequencies
      // - changes the edge label order to optimise the search, fills the database with order numbers instead of
      //   the numbers assigned in the previous phases; fills edgelabelsindexes.
      void reorder ();

      void printTrees ();
      void printTrees (char *name);
      ~Database ();
      
      void toDot(string &fileName);
      
      void toDot(string &fileName,
         map<pair<int,int>, char *> &anomNodes, map< pair<int, pair<int,int> >, char *> &anomEdges,
         map<pair<int,int>, char *> &normNodes, map< pair<int, pair<int,int> >, char *> &normEdges);
      
      void toDot(ofstream &dot,
         map<pair<int,int>, char *> &anomNodes, map< pair<int, pair<int,int> >, char *> &anomEdges,
         map<pair<int,int>, char *> &normNodes, map< pair<int, pair<int,int> >, char *> &normEdges);
   protected:
      
   private:
      void readTree ( Tid tid, vector< pair<unsigned int, int> > &GP_vertices, vector< pair< pair<unsigned int, unsigned int>, int> > &GP_edges );
      void determineCycledNodes ( DatabaseTreePtr tree, vector<int> &nodestack, vector<bool> &visited1, vector<bool> &visited2 );
};

extern Database database;

#endif
