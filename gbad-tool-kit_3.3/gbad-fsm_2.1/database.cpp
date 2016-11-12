//******************************************************************************
// database.cpp
//
// GBAD-FSM, version 0.1
//
// Date      Name       Description
// ========  =========  ========================================================
// 09/28/10  Eberle     Initial version, taken from GASTON.
//                      Courtesy of Siegfried Nijssen,snijssen@liacs.nl, 01/2004
//
//******************************************************************************

#include "database.h"
#include <algorithm>
#include <iostream>

#include "actions.h"

//******************************************************************************
// NAME:    (ostream operator)
//
// INPUTS:  output stream
//          graph edge
//
// RETURN:  stream
//
// PURPOSE: Overloaded output stream for printing a tree edge.
//
//******************************************************************************

ostream &operator<< ( ostream &stream, DatabaseTreeEdge &databasetreeedge ) 
{
   stream << "DatabaseTreeEdge; edgelabel: " << databasetreeedge.edgelabel 
          << "; tonode: " << databasetreeedge.tonode << endl;
   return stream;
}


//******************************************************************************
// NAME:    (ostream operator)
//
// INPUTS:  output stream
//          graph node
//
// RETURN:  stream
//
// PURPOSE: Overloaded output stream for printing a tree node.
//
//******************************************************************************

ostream &operator<< ( ostream &stream, DatabaseTreeNode &databasetreenode ) 
{
   stream << "DatabaseTreeNode; label: " << databasetreenode.nodelabel << "; edges: " << endl;
   for ( int i = 0; i < databasetreenode.edges.size (); i++ )
      stream << databasetreenode.edges[i];
   stream << endl;
   return stream;
}


//******************************************************************************
// NAME:    (ostream operator)
//
// INPUTS:  output stream
//          tree
//
// RETURN:  stream
//
// PURPOSE: Overloaded output stream for printing a tree.
//
//******************************************************************************

ostream &operator<< ( ostream &stream, DatabaseTree &databasetree ) 
{
   stream << "DatabaseTree; tid: " << databasetree.tid << "; nodes: " << endl;
   for ( unsigned int i = 0; i < databasetree.nodes.size (); i++ )
      stream << databasetree.nodes[i];
   stream << endl;
   return stream;
}


//******************************************************************************
// NAME:    read
//
// INPUTS:  file
//
// RETURN:  none
//
// PURPOSE: Read file that contains input graph.
//
//******************************************************************************

void Database::read(FILE *input)
{
   Graph_Info GP_info;
   Tid tid2 = 0;
   
   GP_info.labelList = &labelList;
   GP_read_graph(input, &GP_info);
   
   vector< vector< pair<unsigned int, int> > >::iterator v_iter = GP_info.vertices.begin();
   vector< vector< pair< pair<unsigned int, unsigned int>, int> > >::iterator e_iter = GP_info.edges.begin();
   
   for(; v_iter != GP_info.vertices.end(); ++v_iter, ++e_iter)
   {
      readTree(tid2, *v_iter, *e_iter);
      tid2++;
   }
   
   numberOfTids = tid2;
}


//******************************************************************************
// NAME:    readint
//
// INPUTS:  file
//
// RETURN:  integer
//
// PURPOSE: Read number from specified file and return the actual numeric
//          value.
//
//******************************************************************************

int readint ( FILE *input ) 
{
   char car = fgetc ( input );
   while ( car < '0' || car > '9' ) 
   {
      if ( feof ( input ) )
         return -1;
      car = fgetc ( input );
   }
   int n = car - '0';
   car = fgetc ( input );
   while ( car >= '0' && car <= '9' ) 
   {
      n = n * 10 + car - '0';
      car = fgetc ( input );
   }
  
   return n;
}


//******************************************************************************
// NAME:    readcommand
//
// INPUTS:  file
//
// RETURN:  character
//
// PURPOSE: Read character from specified file and return the validated
//          character.
//
//******************************************************************************

char readcommand ( FILE *input ) 
{
   char car = fgetc ( input );
   while ( car < 'a' || car > 'z' ) 
   {
      if ( feof ( input ) )
         return -1;
      car = fgetc ( input );
   }
   return car;
}


//******************************************************************************
// NAME:    readTree FIX
//
// INPUTS:  file
//          transaction ID
//
// RETURN:  character
//
// PURPOSE: Read all of the information for the specified transaction (tid)
//          and store it as a tree.
//
// NOTE:  With the way the input graph file is read, all of the vertices
//        need to be specified before all of the edges.
//
//******************************************************************************

void Database::readTree ( Tid tid, vector< pair<unsigned int, int> > &GP_vertices, vector< pair< pair<unsigned int, unsigned int>, int> > &GP_edges )
{
   InputNodeLabel inputnodelabel;

   DatabaseTreePtr tree = new DatabaseTree ( tid );
   trees.push_back ( tree );

   unsigned int dummy;
   unsigned int nodessize = 0, edgessize = 0;

   static vector<DatabaseTreeNode> nodes;
   static vector<vector<DatabaseTreeEdge> > edges;
   nodes.resize ( 0 );

   vector< pair<unsigned int, int> > ::iterator v_iter = GP_vertices.begin();
   for(; v_iter != GP_vertices.end(); ++v_iter)
   {
      dummy = v_iter->first - 1;
      inputnodelabel = v_iter->second;
      
      if ( dummy != nodessize ) 
      {
         cerr << "(readTree) Error reading input file - node number does not correspond to its position:" 
            << "v " << dummy << " " << inputnodelabel << endl;
         exit ( 1 );
      }
      nodessize++;

      map_insert_pair ( nodelabelmap ) p = nodelabelmap.insert ( make_pair ( inputnodelabel, nodelabels.size () ) );
      if ( p.second ) 
      {
         vector_push_back ( DatabaseNodeLabel, nodelabels, nodelabel );
         nodelabel.inputlabel = inputnodelabel;
         nodelabel.occurrences.parent = NULL;
         nodelabel.occurrences.number = 1;
         nodelabel.lasttid = tid;
      }
      else 
      {
         DatabaseNodeLabel &nodelabel = nodelabels[p.first->second];
         //
         // NOTE:  The way the FSM works is that a substructure only gets 
         //        counted (frequency) once for every transaction in which it 
         //        resides.  So, the logic below allows us to count every
         //        instance (i.e., there could be multiple instances of a
         //        subgraph within a single transaction).
         //
         nodelabel.frequency++;
         nodelabel.lasttid = tid;
      }

      vector_push_back ( DatabaseTreeNode, nodes, node );
      node.nodelabel = p.first->second;
      node.incycle = false;
      node.tid = tid;
   }

   tree->nodes.reserve ( nodessize );
   if ( edges.size () < nodessize )
      edges.resize ( nodessize );
   for ( unsigned int i = 0; i < nodessize; i++ ) 
   {
      edges[i].resize ( 0 );
      tree->nodes.push_back ( nodes[i] );
   }

   InputEdgeLabel inputedgelabel;
   InputNodeId nodeid1, nodeid2;

   vector< pair< pair<unsigned int, unsigned int>, int> >::iterator e_iter = GP_edges.begin();
   for(; e_iter != GP_edges.end(); ++e_iter)
   {
      nodeid1 = e_iter->first.first-1;
      nodeid2 = e_iter->first.second-1;
      inputedgelabel = e_iter->second;
      
      NodeLabel node2label = tree->nodes[nodeid2].nodelabel;
      NodeLabel node1label = tree->nodes[nodeid1].nodelabel;
      CombinedInputLabel combinedinputlabel;
      if ( node1label > node2label ) 
      {
         NodeLabel temp = node1label;
         node1label = node2label;
         node2label = temp;
      }
      combinedinputlabel = combineInputLabels ( inputedgelabel, node1label, node2label );

      map_insert_pair ( edgelabelmap ) p = edgelabelmap.insert ( make_pair ( combinedinputlabel, edgelabels.size () ) );
      if ( p.second ) 
      {
         vector_push_back ( DatabaseEdgeLabel, edgelabels, edgelabel );
         edgelabel.fromnodelabel = node1label;
         edgelabel.tonodelabel = node2label;
         edgelabel.inputedgelabel = inputedgelabel;
         edgelabel.lasttid = tid;
      }
      else 
      {
         DatabaseEdgeLabel &edgelabel = edgelabels[p.first->second];
         //
         // NOTE:  The way the FSM works is that a substructure only gets 
         //        counted (frequency) once for every transaction in which it 
         //        resides.  So, the logic below allows us to count every
         //        instance (i.e., there could be multiple instances of a
         //        subgraph within a single transaction).
         //
         edgelabel.frequency++;
         edgelabel.lasttid = tid;
      }

      vector_push_back ( DatabaseTreeEdge, edges[nodeid1], edge );
      edge.edgelabel = p.first->second;
      edge.tonode = nodeid2;

      vector_push_back ( DatabaseTreeEdge, edges[nodeid2], edge2 );
      edge2.edgelabel = p.first->second;
      edge2.tonode = nodeid1;

      edgessize++;
   }

   tree->edges = new DatabaseTreeEdge[edgessize * 2];
   int pos = 0;
   for ( unsigned int i = 0; i < nodessize; i++ ) 
   {
      int s = edges[i].size ();
      tree->nodes[i].edges._size = s;
      tree->nodes[i].edges.array = tree->edges + pos;
      for ( int j = 0; j < s; j++, pos++ ) 
      {
         tree->edges[pos] = edges[i][j];
      }
   }

   static vector<int> nodestack;
   static vector<bool> visited1, visited2;
   nodestack.resize ( 0 );
   visited1.resize ( 0 );
   visited1.resize ( nodessize, false );
   visited2.resize ( 0 );
   visited2.resize ( nodessize, false );
   for ( unsigned int i = 0; i < nodessize; i++ ) 
   {
      if ( !visited1[i] ) 
      {
         nodestack.push_back ( i );
         visited1[i] = visited2[i] = true;
         determineCycledNodes ( tree, nodestack, visited1, visited2 );
         visited2[i] = false;
         nodestack.pop_back ();
      }
   }
}



//******************************************************************************
// NAME:    determineCycledNodes
//
// INPUTS:  DatabaseTreePtr tree
//          vector<int> &nodestack
//          vector<bool> &visited1
//          vector<bool> &visited2
//
// RETURN:  none
//
// PURPOSE: TBD
//
//******************************************************************************

void Database::determineCycledNodes ( DatabaseTreePtr tree, 
                                      vector<int> &nodestack, 
                                      vector<bool> &visited1, 
                                      vector<bool> &visited2 ) 
{
   int node = nodestack.back ();
   pvector<DatabaseTreeEdge> &edges = tree->nodes[node].edges;

   for ( int i = 0; i < edges.size (); i++ ) 
   {
      if ( !visited1[edges[i].tonode] ) 
      {
         nodestack.push_back ( edges[i].tonode );
         visited1[edges[i].tonode] = visited2[edges[i].tonode] = true;
         determineCycledNodes ( tree, nodestack, visited1, visited2 );
         nodestack.pop_back ();
         visited2[edges[i].tonode] = false;
      }
      else 
      {
         if ( visited2[edges[i].tonode] && ( nodestack.size () == (unsigned int)1 || 
              (unsigned int)nodestack[nodestack.size () - 2] != edges[i].tonode ) ) 
         {
            int j = nodestack.size () - 1;
            while ( (unsigned int)nodestack[j] != edges[i].tonode ) 
            {
               tree->nodes[nodestack[j]].incycle = true;
               j--;
            }
            tree->nodes[nodestack[j]].incycle = true;
         }
      }
   }
}


//******************************************************************************
// NAME:    edgecount
//
// INPUTS:  none
//
// RETURN:  none
//
// PURPOSE: TBD
//
//******************************************************************************

void Database::edgecount () 
{
   for ( unsigned int i = 0; i < edgelabels.size (); i++ ) 
   {
      if ((edgelabels[i].frequency >= minfreq ) || (phase == 4))
      {
         nodelabels[edgelabels[i].tonodelabel].frequentedgelabels.push_back ( i );
         if ( edgelabels[i].fromnodelabel != edgelabels[i].tonodelabel )
            nodelabels[edgelabels[i].fromnodelabel].frequentedgelabels.push_back ( i );
      }
   }
}


//******************************************************************************
// NAME:    
//
// INPUTS:  none
//
// RETURN:  none
//
// PURPOSE: TBD
//
//******************************************************************************

class EdgeLabelsIndexesSort:public std::binary_function<int,int,bool> 
{
      const vector<DatabaseEdgeLabel> &edgelabels;
   public:
      EdgeLabelsIndexesSort ( const vector<DatabaseEdgeLabel> &edgelabels ) : edgelabels ( edgelabels ) { }
      bool operator () ( int a, int b ) const 
      {
         return edgelabels[a].frequency < edgelabels[b].frequency;
      }
};


//******************************************************************************
// NAME:    (overloaded less than operator)
//
// INPUTS:  none
//
// RETURN:  none
//
// PURPOSE: TBD
//
//******************************************************************************

bool operator< ( const DatabaseTreeEdge &a, const DatabaseTreeEdge &b ) 
{
   return a.edgelabel < b.edgelabel;
}


//******************************************************************************
// NAME:    reorder
//
// INPUTS:  none
//
// RETURN:  none
//
// PURPOSE: TBD
//
//******************************************************************************

void Database::reorder () 
{
   edgelabelsindexes.reserve ( edgelabels.size () );

   for ( unsigned int i = 0; i < edgelabels.size (); i++ ) 
   {
      if ((edgelabels[i].frequency >= minfreq ) || (phase == 4))
         edgelabelsindexes.push_back ( i );
   }

   sort ( edgelabelsindexes.begin (), edgelabelsindexes.end (), EdgeLabelsIndexesSort ( edgelabels ) );
   // NOTE FROM GASTON AUTHOR:  This does not seem to make a lot of difference 
   // on the databases that we tried.  We don't know yet why - in similar 
   // other algorithms it seemed a good idea...

   for ( unsigned int i = 0; i < edgelabelsindexes.size (); i++ ) 
   {
      edgelabels[edgelabelsindexes[i]].edgelabel = i; // fill in the final edge labels
#ifdef DEBUG
      DatabaseEdgeLabel &label = edgelabels[edgelabelsindexes[i]];
      cout << (int) nodelabels[label.tonodelabel].inputlabel 
           << "[" << (int) label.inputedgelabel << "]" 
           << (int) nodelabels[label.fromnodelabel].inputlabel <<"-->" << i <<endl;
#endif
   }

   for ( Tid i = 0; i < trees.size (); i++ ) 
   {
      DatabaseTree &tree = * (trees[i]);
      for ( NodeId j = 0; j < tree.nodes.size (); j++ ) 
      {
         DatabaseTreeNode &node = tree.nodes[j];
         if (( nodelabels[node.nodelabel].frequency >= minfreq ) || (phase == 4))
         {
            DatabaseNodeLabel &nodelabel = nodelabels[node.nodelabel];
       nodelabel.occurrences.elements.push_back (
               LegOccurrence (
                  tree.tid,
                  (OccurrenceId) nodelabel.occurrences.elements.size (),
                  j,
                  NONODE)
            );
            int k = 0;
            for ( int l = 0; l < node.edges.size (); l++ ) 
            {
               EdgeLabel lab = node.edges[l].edgelabel;
               if (( edgelabels[lab].frequency >= minfreq ) || (phase == 4))
               {
             node.edges[k].edgelabel = edgelabels[node.edges[l].edgelabel].edgelabel; // translate old into new edge labels
             node.edges[k].tonode = node.edges[l].tonode;
             k++;
          }
       }
       node.edges.resize ( k );
         }
         else
            node.edges.clear ();
      }
   }
}


//******************************************************************************
// NAME:    printTrees
//
// INPUTS:  none
//
// RETURN:  none
//
// PURPOSE: Print each of the trees associated with each transaction.
//
//******************************************************************************
void Database::printTrees () 
{
   for ( unsigned int i = 0; i < trees.size (); i++ )
   {
      cout << "Tree[" << i << "]->tid = " << trees[i]->tid << endl;
      for ( unsigned int j = 0; j < trees[i]->nodes.size (); j++ )
      {
         cout << "   node[" << j << "] " << trees[i]->nodes[j].nodelabel << " tid = " << trees[i]->nodes[j].tid << endl;
         
         for(int kk = 0; kk < trees[i]->nodes[j].edges._size; kk++)
         {
            //cout << "      " << " node_" << trees[i]->nodes[j].edges.array[kk].tonode << " " << trees[i]->nodes[j].edges.array[kk].edgelabel << endl;
            cout << "      " << " node_" << trees[i]->nodes[j].edges.array[kk].tonode << " ";
            cout << edgelabels[trees[i]->nodes[j].edges.array[kk].edgelabel].inputedgelabel << endl;
         }
      }
   }
}


//******************************************************************************
// NAME:    (Database destructor)
//
// INPUTS:  none
//
// RETURN:  none
//
// PURPOSE: TBD
//
//******************************************************************************

Database::~Database () 
{
   for ( unsigned int i = 0; i < trees.size (); i++ )
      delete trees[i];
}

void Database::toDot(string &fileName)
{
   map<pair<int,int>, char *> nodes;
   map< pair<int, pair<int,int> >, char *> edges;
   
   ofstream dot;
   dot.open (fileName.c_str(), ios::out | ios::trunc);
   if(dot.fail())
   {
      perror(fileName.c_str());
      return;
   }
   
   toDot(dot, nodes, edges, nodes, edges);
}

void Database::toDot(string &fileName, 
   map<pair<int,int>, char *> &anomNodes, map< pair<int, pair<int,int> >, char *> &anomEdges,
   map<pair<int,int>, char *> &normNodes, map< pair<int, pair<int,int> >, char *> &normEdges)
{
   ofstream dot;
   dot.open(fileName.c_str(), ios::out | ios::trunc);
   if(dot.fail())
   {
      perror(fileName.c_str());
      return;
   }
   
   toDot(dot, anomNodes, anomEdges, normNodes, normEdges);
}

void Database::toDot(ofstream &dot,
   map<pair<int,int>, char *> &anomNodes, map< pair<int, pair<int,int> >, char *> &anomEdges,
   map<pair<int,int>, char *> &normNodes, map< pair<int, pair<int,int> >, char *> &normEdges)
{
   unsigned int offset=1;
   unsigned int cur_id = 0;
   
   const Label *label;
   
   char *color = (char *)"azure3";
   
   pair<int,int> nodePair;
   pair<int, pair<int,int> > edgePair;
      
   vector<DatabaseTreeNode>::iterator nodeIter;
   vector<DatabaseTreeNode>::iterator nodeStop;
   
   int edgeIndex;
   
   vector<DatabaseTreePtr>::iterator treeIter = trees.begin();
   vector<DatabaseTreePtr>::iterator treeStop = trees.end();
   
   map<pair<int,int>, char *>::iterator nodeSearchIter;
   map< pair<int, pair<int,int> >, char *>::iterator edgeSearchIter;
   
   dot << "digraph G {" << endl;
   // loop over each transaction
   for(; treeIter != treeStop; ++treeIter)
   {
      dot << "\tsubgraph cluster_" << (*treeIter)->tid+1 << " {" << endl;
      dot << "\t\tlabel = \"XP # " << (*treeIter)->tid+1 << "\";" << endl;
      //dot << "\t\tcolor = blue;" << endl;
      
      nodeIter = (*treeIter)->nodes.begin();
      nodeStop = (*treeIter)->nodes.end();
      cur_id = 0;
      
      // loop over each node in this transaction
      for(; nodeIter != nodeStop; ++nodeIter)
      {
         nodePair.first = (*treeIter)->tid;
         nodePair.second = cur_id;
         
         color = (char *)"azure3";
         
         // check if this vertex is in the set of anomalies
         if((nodeSearchIter = anomNodes.find(nodePair)) != anomNodes.end())
         {
            color = nodeSearchIter->second;
         }
         // check if this vertex is part of a normative pattern
         else if((nodeSearchIter = normNodes.find(nodePair)) != normNodes.end())
         {
            color = nodeSearchIter->second;
         }
         
         label = labelList.getLabel(nodeIter->nodelabel);
         
         dot << "\t\t" << cur_id+offset << " [label=";
         
         if(label != NULL)
         {
            dot << (*label);
         }
         else
         {
            dot << "(VERTEX LABEL NOT FOUND: " << nodeIter->nodelabel << ")";
         }
         
         dot << ",style=filled,fillcolor=" << color << ",fontcolor=black];" << endl;
         
         //cout << "v " << cur_id << endl;
         cur_id++;
      }
      
      nodeIter = (*treeIter)->nodes.begin();
      nodeStop = (*treeIter)->nodes.end();
      
      // loop over the nodes again and print the edges
      for(cur_id = 0; nodeIter != nodeStop; ++nodeIter, cur_id++)
      {
         for(edgeIndex=0; edgeIndex < nodeIter->edges._size; edgeIndex++)
         {
            if(cur_id <= nodeIter->edges.array[edgeIndex].tonode)
            {
               edgePair.first = (*treeIter)->tid;
               edgePair.second.first = cur_id;
               edgePair.second.second = nodeIter->edges.array[edgeIndex].tonode;
               
               //cout << "\t\t\t" << edgePair.first << " " << edgePair.second.first << " " << edgePair.second.second << endl;
               
               color = (char *)"azure3";
               
               // check if this edge is in the set of anomalies
               if((edgeSearchIter = anomEdges.find(edgePair)) != anomEdges.end())
               {
                  color = edgeSearchIter->second;
               }
               // check if this edge is part of a normative pattern
               else if((edgeSearchIter = normEdges.find(edgePair)) != normEdges.end())
               {
                  color = edgeSearchIter->second;
               }
               dot << "\t\t" << cur_id+offset << " -> " << nodeIter->edges.array[edgeIndex].tonode+offset << " [label=";
               
               label = labelList.getLabel(edgelabels[nodeIter->edges.array[edgeIndex].edgelabel].inputedgelabel);
               
               if(label != NULL)
               {
                  dot << (*label);
               }
               else
               {
                  dot << "\"(EDGE LABEL NOT FOUND: " << nodeIter->edges.array[edgeIndex].edgelabel << ")\"";
               }
               dot << ",arrowhead=none,color=" << color << ",fontcolor=black];" << endl;
            }
         }
      }
      
      offset += cur_id;
      dot << "\t}" << endl;
   }
   
   /* // THE FOLLOWING IS FOR DEBUGGING
   map< pair<int, pair<int,int> >, char *>::iterator iter = anomEdges.begin();
   for(; iter != anomEdges.end(); iter++)
   {
      printf("%d (%d,%d)\n", iter->first.first, iter->first.second.first, iter->first.second.second);
   }*/
   
   dot << "}" << endl;
   dot.close();
   
   //cout << "anom " << anomNodes.size() << " " << anomEdges.size() << endl;
   //cout << "norm " << normNodes.size() << " " << normEdges.size() << endl;
}

