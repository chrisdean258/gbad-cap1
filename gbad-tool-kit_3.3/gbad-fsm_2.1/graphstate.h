//******************************************************************************
// graphstate.h
//
// GBAD-FSM, version 0.1
//
// Date      Name       Description
// ========  =========  ========================================================
// 09/28/10  Eberle     Initial version, taken from GASTON.
//                      Courtesy of Siegfried Nijssen,snijssen@liacs.nl, 01/2004
//
//******************************************************************************

#ifndef GRAPHSTATE_H
#define GRAPHSTATE_H

#include "misc.h"
#include <vector>
#include <iostream>
#include "database.h"
#include "closeleg.h"
#include "patterntree.h"
using namespace std;

typedef unsigned int Mark;

class GraphState;

extern GraphState graphstate;

class GraphState 
{
   public:
      struct GSDeletedEdge {
         NodeId tonode, fromnode;
         EdgeLabel edgelabel;
         int postonode, posfromnode;
         bool close;
         Mark cyclemark;
      };
	  void clearGraphState();
      vector<GSDeletedEdge> deletededges;
      // the current pattern
      vector<Tuple> *treetuples;
      vector<CloseTuple> *closetuples;
      vector<NodeId> nodesinpreorder;
      int backbonelength; // the length of the backbone, in number of nodes
      int startsecondpath; // the position of the second part of the backbone in
                           // the treetuples.
      bool nasty;  // nasty == A-B-A-B-A-B -like cases
      NodeLabel centerlabel;
      EdgeLabel bicenterlabel;
      int closecount;
      bool selfdone; // set by isnormal to store whether the original graph has been
                     // normal-checked; we try to delay this until the very last moment,
                     // as we know that on this graph the normalisation procedure will
                     // have to go through all phases
    
      struct GSEdge 
      {
         NodeId tonode;
         int postonode; // position in the adjacency list of the corresponding reverse edge
         EdgeLabel edgelabel;
         Mark cyclemark;
         bool close; // closing edge
         GSEdge ():cyclemark ( 0 ), close ( false ) { }
         GSEdge ( int tonode, int postonode, EdgeLabel edgelabel, bool close = false ): 
            tonode ( tonode ), postonode ( postonode ), edgelabel ( edgelabel ), 
            cyclemark ( 0 ), close ( close ) { }
         bool alreadyMarked;
         Tid tid;
      };
      struct GSNode 
      {
         NodeLabel label;
         short unsigned int maxdegree;
         vector<GSEdge> edges;
         bool alreadyMarked;
         vector<LegOccurrence> elements;
      };
      //keep for debugging purposes
      void makeState ( DatabaseTree *databasetree );
      void undoState ();
      void insertNode ( NodeLabel nodelabel, short unsigned int maxdegree, 
                        vector<LegOccurrence> &elements );
      void deleteNode2 ();
      vector<GSNode> nodes;
      int edgessize;
      short unsigned int getNodeDegree ( int i ) const { return nodes[i].edges.size (); }
      short unsigned int getNodeMaxDegree ( int i ) const { return nodes[i].maxdegree; }
      GraphState ();
      void determineCycles ( unsigned int usedbit );
      int enumerateSpanning ();
      int isnormal ();
      int normalizetree ();
      int normalizeSelf ();
      void init ();
      void insertStartNode ( NodeLabel nodelabel );
      void deleteStartNode ();
      void insertNode ( int from, EdgeLabel edgelabel, short unsigned int maxdegree,
                        vector<LegOccurrence> &elements );
      void deleteNode ();
      void insertEdge ( int from, int to, EdgeLabel edgelabel );
      void deleteEdge ( int from, int to );
      void deleteEdge ( GSEdge &edge ); // pushes deleted edge on stack
      void reinsertEdge (); // reinserts last edge on the stack
      NodeId lastNode () const { return nodes.size () - 1; }
      void print ( FILE *f );
      vector<OccurrenceId> occurrenceids;

   private:
      void readcommand(FILE *);
};

#endif
