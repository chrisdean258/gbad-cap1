//******************************************************************************
// gbad.cpp
//
// GBAD-FSM, version 0.3
//
// Date      Name        Description
// ========  =========   ========================================================
// 12/08/10  Eberle      Initial version.
// 03/04/11  Eberle      Fixed issue with overlapping instances and reported
//                       number of best substructure instances.
// 05/25/11  Massengill  Instances of the best substructure are saved to memory
//                       or best_sub.inst.
// 05/11/11  Eberle      Changed name of method saveAnomSub() to 
//                       outputAnomalousModifications(); changed logic in
//                       outputAnomalousModifications() such that the actual
//                       vertex numbers from the anomalous instances are output.
// 06/23/11  Massengill  All output files are named in main.cpp.
// 02/16/13  Hensley     Removed obsolete code.
//
//******************************************************************************

#include "gbad.h"
#include "graphstate.h"
#include "database.h"
#include "misc.h"
#include <queue>
#include <stdlib.h>
#include <cmath>
#include <time.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdio.h>
using namespace std;

char readcommand ( FILE *input );
int readint ( FILE *input );
DatabaseTree *bestSubTree;
extern bool saveBestSubInst;
extern bool phase2;

//output file names
extern const char* best_sub_g_name;
extern const char* best_sub_inst_name;
extern const char* anom_sub_name;

GBAD gbad;

//******************************************************************************
// NAME:    readBestSub
//
// INPUTS:  none
//
// RETURN:  stream
//
// PURPOSE: Read the best substructure, that has been written to a file, and 
//          store it in the bestSubTree.
//
//******************************************************************************

void GBAD::readBestSub ()
{
   InputNodeLabel inputnodelabel;

   DatabaseTreePtr tree = new DatabaseTree ( 0 );
   char command;
   Tid tidNumber;
   int nodeNumber;
   int nodessize = 0, edgessize = 0;
   // Want to use the best substructure that was written in the first pass...
   FILE *input = fopen ( best_sub_g_name, "r" );
   if (input==NULL && phase2)
   {
      cerr << "Unable to open " << best_sub_g_name << endl;
      exit(1);
   }
   command = readcommand ( input );
  
   cout << "Reading in the best subtructure "<< best_sub_g_name <<" ... " << endl;

   static vector<DatabaseTreeNode> nodes;
   static vector<vector<DatabaseTreeEdge> > edges;
   nodes.resize ( 0 );
   bestSize = 0;
   bestNumNodes = 0;

   while ( command == 'v' ) 
   {
      nodeNumber = readint ( input );
      inputnodelabel = readint ( input );
      if ( nodeNumber != nodessize ) 
      {
         cerr << "(readBestSub) Error reading input file - node number does not correspond to its position." << endl;
         exit ( 1 );
      }
      nodessize++;
      pair<map<InputNodeLabel,NodeLabel>::iterator,bool> 
         p = database.nodelabelmap.insert ( make_pair ( inputnodelabel, database.nodelabels.size () ) );
      if ( p.second ) 
         cout << "ERROR! best substructure vertex label does not occur in dataset -- SHOULD NEVER HAPPEN!" << endl;
    
      vector_push_back ( DatabaseTreeNode, nodes, node );
      node.nodelabel = p.first->second;
      node.incycle = false;

      command = readcommand ( input );
      bestSize++;
      bestNumNodes++;
      bestNodeLabels.push_back(inputnodelabel);
   }

   tree->nodes.reserve ( nodessize );
   if ( edges.size () < (unsigned int)nodessize )
      edges.resize ( nodessize );
   for ( int i = 0; i < nodessize; i++ ) 
   {
      edges[i].resize ( 0 );
      tree->nodes.push_back ( nodes[i] );
   }
  
   InputEdgeLabel inputedgelabel;
   InputNodeId nodeid1, nodeid2;

   while ( !feof ( input ) && command == 'e' ) 
   {
      nodeid1 = readint ( input );
      nodeid2 = readint ( input );
      inputedgelabel = readint ( input );
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

      pair<map<CombinedInputLabel,EdgeLabel>::iterator,bool>
         p = database.edgelabelmap.insert ( make_pair ( combinedinputlabel, database.edgelabels.size () ) );
      if ( p.second ) 
         cout << "ERROR! best substructure edge label does not occur in dataset -- SHOULD NEVER HAPPEN!" << endl;

      vector_push_back ( DatabaseTreeEdge, edges[nodeid1], edge );
      edge.edgelabel = database.edgelabels[p.first->second].edgelabel;
      edge.tonode = nodeid2;

      vector_push_back ( DatabaseTreeEdge, edges[nodeid2], edge2 );
      edge2.edgelabel = database.edgelabels[p.first->second].edgelabel;
      edge2.tonode = nodeid1;

      edgessize++;

      command = readcommand ( input );
      bestSize++;
   }
   // After reading vertices and edges, last line should be the number
   // of instances of the best substructure (preceded by a 'z')
   numBestInstances = readint ( input );
   cout << "Number of instances of best substructure: " << numBestInstances << endl;

   tree->edges = new DatabaseTreeEdge[edgessize * 2];
   int pos = 0;
   for ( int i = 0; i < nodessize; i++ ) 
   {
      int s = edges[i].size ();
      tree->nodes[i].edges._size = s;
      tree->nodes[i].edges.array = tree->edges + pos;
      for ( int j = 0; j < s; j++, pos++ ) {
         tree->edges[pos] = edges[i][j];
      }
   }
   cout << "Size of best substructure: " << bestSize << endl;
   bestSubTree = tree;
   fclose ( input );

   cout << "Reading in the best subtructure instances ";
   if (saveBestSubInst)
   {
      cout << best_sub_inst_name;
   }
   cout << " ... " << endl;

   NodeId fromnodeid;
   NodeId tonodeid;
   EdgeLabel edgelabel;
   OccurrenceId occurrenceid;
   // Finally, read best substructure instances into memory
   if (saveBestSubInst)
   {
      FILE *instances = fopen ( best_sub_inst_name, "r" );
      if (instances==NULL && phase2)
      {
          cerr << "Unable to open " << best_sub_inst_name << endl;
          exit(1);
      }

      // The best_sub.inst file consists of pairs of tid's and occurrenceid's
      tidNumber = readint(instances);
      while (!feof(instances))
      {
        bestTids.push_back(tidNumber);
        fromnodeid = readint(instances);
        bestFromnodeids.push_back(fromnodeid);
        tonodeid = readint(instances);
        bestTonodeids.push_back(tonodeid);
        edgelabel = readint(instances);
        bestEdgelabels.push_back(edgelabel);
        occurrenceid = readint(instances);
        bestOccurrenceids.push_back(occurrenceid);
        tidNumber = readint(instances);
      }
      fclose(instances);
   }
}


//******************************************************************************
// NAME:    graphMatch
//
// INPUTS:  matchPercentage
//
// RETURN:  TRUE if graphs match within the specified matchPercentage,
//          FALSE otherwise.
//
// PURPOSE: This function returns TRUE if the graphstate node could POSSIBLY 
//          match the node in the bestSubTree if it were large enough, given the 
//          matchThreshold.
//
//          It also returns the potential percentage of match in its parameter 
//          list.
//
// NOTE:  This should really be called passing in a matching threshold (rather
//        than a global)...  The fix I have implemented will effect all of the
//        calls to graphMatch that are made outside of this file.  However,
//        those calls were implemented based upon running GBAD-FSM with a
//        normal frequency (i.e., not 1) - which worked for edge anomalies, just
//        not node anomalies.  So, if we go to making runs (for speedup reasons)
//        where we don't set the frequency to 1, this will need to be addressed...
//
// NOTE:  A side-effect of this routine is the setting of the alreadyMarked
//        flags in the nodes and edges, needed by other routines.
//
//******************************************************************************

bool GBAD::graphMatch (double *matchPercentage) 
{
   double percentageChange = 0.0;

   int numMatchingNodes = 0;
   int numBestNodes = bestSubTree->nodes.size();

   int i, j, k, m, n;
   int* nonMatchedNodes = 0;
   int numNonMatchingNodes = 0;
   nonMatchedNodes = new int[numBestNodes];

   // First, mark all vertices so they do not get counted more than once
   for ( j = 0; (unsigned int)j < graphstate.nodes.size(); j++ ) 
      graphstate.nodes[j].alreadyMarked = false;

   // Then, see which nodes match
   for ( int i = 0; i < numBestNodes; i++ ) 
   {
      for ( j = 0; (unsigned int)j < graphstate.nodes.size(); j++ ) 
      {
         // This is a comparison of indices, that will match entries in the database
         if ((bestSubTree->nodes[i].nodelabel == graphstate.nodes[j].label) &&
             (!graphstate.nodes[j].alreadyMarked))
         {
            numMatchingNodes++;
            graphstate.nodes[j].alreadyMarked = true;
            j = graphstate.nodes.size();   // skip rest of vertices
         }
      }
   }
   // Now save the nodes that did not match
   for ( j = 0; (unsigned int)j < graphstate.nodes.size(); j++ ) 
   {
      if (numNonMatchingNodes >= numBestNodes)  // can’t change more nodes/edges than exist…
         break;
      
      if (!graphstate.nodes[j].alreadyMarked)
      {
         nonMatchedNodes[numNonMatchingNodes] = j;
         numNonMatchingNodes++;
      }
   }
   
   int numMatchingEdges = 0;
   int numMissingEdges = 0;
   int numNonMatchingEdges = 0;
   int numBestEdges = 0;

   int bestFromNodeLabel = 0;
   int bestToNodeLabel = 0;
   int bestEdgeLabel = 0;
   int graphFromNodeLabel = 0;
   int graphToNodeLabel = 0;
   int graphEdgeLabel = 0;

   bool foundMatchingEdge = false;
   int graphAnomFromNodeLabel = -1;
   int graphAnomToNodeLabel = -1;

   // Now, mark all edges so they do not get counted more than once
   for ( j = 0; (unsigned int)j < graphstate.nodes.size(); j++ ) 
      for ( n = 0; (unsigned int)n < graphstate.nodes[j].edges.size(); n++ )
         graphstate.nodes[j].edges[n].alreadyMarked = false;

   // Loop through all of the new graph nodes to look at their edges...
   for ( j = 0; (unsigned int)j < graphstate.nodes.size(); j++ ) 
   {
      for ( n = 0; (unsigned int)n < graphstate.nodes[j].edges.size(); n++ )
      {
         graphFromNodeLabel = 
            database.nodelabels[database.edgelabels[database.edgelabelsindexes[graphstate.nodes[j].edges[n].edgelabel]].fromnodelabel].inputlabel;
         graphToNodeLabel = 
            database.nodelabels[database.edgelabels[database.edgelabelsindexes[graphstate.nodes[j].edges[n].edgelabel]].tonodelabel].inputlabel;
         graphEdgeLabel = 
            database.edgelabels[database.edgelabelsindexes[graphstate.nodes[j].edges[n].edgelabel]].inputedgelabel;
   
         // ... and see if they match any of the best sub edges...
         for ( i = 0; i < numBestNodes; i++ ) 
         {
            foundMatchingEdge = false;
            numBestEdges = bestSubTree->nodes[i].edges.size();
            for ( m = 0; m < numBestEdges; m++ ) 
            {
               bestFromNodeLabel = 
                  database.nodelabels[database.edgelabels[database.edgelabelsindexes[bestSubTree->nodes[i].edges[m].edgelabel]].fromnodelabel].inputlabel;
               bestToNodeLabel = 
                  database.nodelabels[database.edgelabels[database.edgelabelsindexes[bestSubTree->nodes[i].edges[m].edgelabel]].tonodelabel].inputlabel;
               bestEdgeLabel = 
                  database.edgelabels[database.edgelabelsindexes[bestSubTree->nodes[i].edges[m].edgelabel]].inputedgelabel;

               if ((bestFromNodeLabel == graphFromNodeLabel) &&
                   (bestToNodeLabel == graphToNodeLabel) &&
                   (bestEdgeLabel == graphEdgeLabel) &&
                   (!graphstate.nodes[j].edges[n].alreadyMarked))
               {
                  numMatchingEdges++;
                  m = numBestEdges;   // stop searching for matches in the best sub
                  i = numBestNodes;
                  foundMatchingEdge = true;
                  graphstate.nodes[j].edges[n].alreadyMarked = true;
               }
            }
            // It is possible that the edge goes to an anomalous node...
            // So, if one of the nodes matches, and the edge label matches, call
            // it a match...
            if (!foundMatchingEdge)
            {
               for ( m = 0; m < numBestEdges; m++ ) 
               {
                  bestFromNodeLabel = 
                     database.nodelabels[database.edgelabels[database.edgelabelsindexes[bestSubTree->nodes[i].edges[m].edgelabel]].fromnodelabel].inputlabel;
                  bestToNodeLabel = 
                     database.nodelabels[database.edgelabels[database.edgelabelsindexes[bestSubTree->nodes[i].edges[m].edgelabel]].tonodelabel].inputlabel;
                  bestEdgeLabel = 
                     database.edgelabels[database.edgelabelsindexes[bestSubTree->nodes[i].edges[m].edgelabel]].inputedgelabel;

                  for (k = 0; k < numNonMatchingNodes; k++)
                  {
                     if (nonMatchedNodes[k] == j)
                     {
                        graphFromNodeLabel = 
                           database.nodelabels[database.edgelabels[database.edgelabelsindexes[graphstate.nodes[j].edges[n].edgelabel]].fromnodelabel].inputlabel;
                        graphToNodeLabel = 
                           database.nodelabels[database.edgelabels[database.edgelabelsindexes[graphstate.nodes[j].edges[n].edgelabel]].tonodelabel].inputlabel;
                        graphAnomFromNodeLabel = 
                           database.nodelabels[database.edgelabels[database.edgelabelsindexes[graphstate.nodes[nonMatchedNodes[k]].edges[n].edgelabel]].fromnodelabel].inputlabel;
                        graphAnomToNodeLabel = 
                           database.nodelabels[database.edgelabels[database.edgelabelsindexes[graphstate.nodes[nonMatchedNodes[k]].edges[n].edgelabel]].tonodelabel].inputlabel;

                        graphEdgeLabel = 
                           database.edgelabels[database.edgelabelsindexes[graphstate.nodes[j].edges[n].edgelabel]].inputedgelabel;
   
                        // If either of the endpoints match, and this is the node
                        // that does not match, and the edge label is the same...
                        if ((((bestToNodeLabel == graphToNodeLabel) && 
                              (bestFromNodeLabel != graphFromNodeLabel) &&
                              (graphFromNodeLabel == graphAnomFromNodeLabel)) || 
                             ((bestFromNodeLabel == graphFromNodeLabel) && 
                              (bestToNodeLabel != graphToNodeLabel) &&
                              (graphToNodeLabel == graphAnomToNodeLabel)) ||
                             ((bestToNodeLabel == graphFromNodeLabel) && 
                              (bestFromNodeLabel != graphToNodeLabel) &&
                              (graphFromNodeLabel == graphAnomFromNodeLabel)) || 
                             ((bestToNodeLabel == graphFromNodeLabel) && 
                              (bestToNodeLabel != graphFromNodeLabel) &&
                              (graphToNodeLabel == graphAnomToNodeLabel))) &&
                            (bestEdgeLabel == graphEdgeLabel))
                        {
                           numMatchingEdges++;
                           //
                           // NOTE:  This above logic was to handle the edge that went (to or from)
                           //        a non-anomalous node to/from an anomalous node... but what 
                           //        about the case of an edge between two anomalous nodes...
                           //
                           graphstate.nodes[j].edges[n].alreadyMarked = true;
                        }
                     }
                  }
               }
            }
         }
      }
   }

   // Count should be double because the edges are undirected...
   if (numMatchingEdges > 1)
      numMatchingEdges = numMatchingEdges/2;

   // How many edges are there in the best substructure?
   numBestEdges = 0;
   for ( unsigned int i = 0; i < bestSubTree->nodes.size(); i++ ) 
      numBestEdges = numBestEdges + bestSubTree->nodes[i].edges.size();
   numBestEdges = numBestEdges/2;    // remove the fact that edges go both ways...

   // How many edges are there in the graph?
   int numEdges = 0;
   for ( unsigned int j = 0; j < graphstate.nodes.size(); j++ ) 
      numEdges = numEdges + graphstate.nodes[j].edges.size();
   numEdges = numEdges/2;   // removes the fact that edges go both ways...

   if (numMatchingEdges > numBestEdges)
      numMatchingEdges = numBestEdges;   // can't be more than that....

   numNonMatchingEdges = numEdges - numMatchingEdges;
   numMissingEdges = numBestEdges - numMatchingEdges - numNonMatchingEdges;

   // if number of non-matching and missing edges are zero, set all of their
   // alreadyMarked flags to true (so that we make sure we don't show any edges
   // as anomalous)
   if ((numNonMatchingEdges == 0) && (numMissingEdges == 0))
   {
      for ( j = 0; (unsigned int)j < graphstate.nodes.size(); j++ ) 
      {
         for ( n = 0; (unsigned int)n < graphstate.nodes[j].edges.size(); n++ )
         {
               graphstate.nodes[j].edges[n].alreadyMarked = true;
         }
      }
   }

   // Depending upon which substructure is bigger (best sub or this sub)...
   int graphSize = graphstate.nodes.size() + numEdges;
   if (bestSize >= graphSize)
   {
      percentageChange = (double) 1.0 - 
                         ((double) (((double)numMatchingNodes + (double)numMatchingEdges) /
                                    ((double)bestSize))) ;
   }
   else
      percentageChange = (double) 1.0 - 
                         ((double) (((double)numMatchingNodes + (double)numMatchingEdges) /
                                    ((double)graphSize))) ;

   // deallocate memory
   delete []nonMatchedNodes;

   // Pass back actual value, if desired
   if (matchPercentage != NULL)
      *matchPercentage = percentageChange;

   //
   // NOTE:  This routine really needs to be an inexactGraphMatch - a match that
   //        returns TRUE if the pattern matches within a specified threshold.
   //
   if (percentageChange <= matchThreshold)
      return true;
   else
      return false;
}

//******************************************************************************
// NAME:    getNumInstances
//
// INPUTS:  occurrences of a substructure
//
// RETURN:  number of occurrences of the specified substructure
//
// PURPOSE: This function loops through the top occurrences of a substructure,
//          and searches for all of the unique pairs (by tid).  The number is
//          then returned as the count of instances for that substructure.
//
//******************************************************************************

int GBAD::getNumInstances(LegOccurrences &legoccurrences)
{
   typedef std::map<string, int> PairCount;
   PairCount pairCounts;
   
   for (unsigned int numOccs=0; numOccs<legoccurrences.elements.size(); numOccs++)
   {
      ostringstream stringPair;
      stringPair << legoccurrences.elements[numOccs].tid << "-" 
                 << legoccurrences.elements[numOccs].fromnodeid << "-" 
                 << legoccurrences.elements[numOccs].tonodeid;
      string s  = stringPair.str();
      map<string, int>::iterator p;
      p = pairCounts.find(s);
      if (p == pairCounts.end())    // if pair not found...
         pairCounts.insert(make_pair(s,1));
   }

   return pairCounts.size();
}


//******************************************************************************
// NAME:    saveBestSub
//
// INPUTS:  occurrences of the best sub
//
// RETURN:  none
//
// PURPOSE: This function saves the current occurrence as the best
//          substructure, and prints it to a file.
//
//******************************************************************************

void GBAD::saveBestSub(LegOccurrences &legoccurrences) 
{
   bestTids.clear();
   bestFromnodeids.clear();
   bestTonodeids.clear();
   bestEdgelabels.clear();
   bestOccurrenceids.clear();
   
   normNodes.clear();
   normEdges.clear();
   
   FILE *best_sub_file;
   best_sub_file = fopen ( best_sub_g_name, "w" );
   Substructure bestSub;
   bestSize = 0;

   for ( unsigned int i = 0; i < graphstate.nodes.size (); i++ ) 
   {
      fprintf(best_sub_file,"v %d %d\n",(int)i,
              (int)database.nodelabels[graphstate.nodes[i].label].inputlabel);
      bestSize++;
   }

   for ( unsigned int i = 0; i < graphstate.nodes.size (); i++ ) 
   {
      for ( unsigned int j = 0; j < graphstate.nodes[i].edges.size (); j++ ) 
      {
         GraphState::GSEdge &edge = graphstate.nodes[i].edges[j];
         if ( i < edge.tonode ) 
         {
            fprintf(best_sub_file,"e %d %d %d\n",(int)i,(int)edge.tonode,
                    (int)database.edgelabels[database.edgelabelsindexes[edge.edgelabel]].inputedgelabel);
            bestSub.fromNode.push_back (i);
            bestSub.toNode.push_back ((int) edge.tonode);
            bestSub.edgeLabel.push_back ((int) database.edgelabels[database.edgelabelsindexes[edge.edgelabel]].inputedgelabel);
            bestSize++;
         }      
      }
   }

   numBestInstances = getNumInstances(legoccurrences);

   fprintf(best_sub_file,"z\n%d\n",numBestInstances);
   fclose(best_sub_file);

   // So, let's try saving the edges associated with the best sub instances
   FILE *best_instances_file= fopen ( best_sub_inst_name, "w" );

   bool found;

   InstanceSubstructure* bestSubInstanceEdges = new InstanceSubstructure[numberOfTids];

   for (unsigned int numOccs=0; numOccs<legoccurrences.elements.size(); numOccs++)
   {
      normNodes[make_pair(legoccurrences.elements[numOccs].tid, legoccurrences.elements[numOccs].fromnodeid)] = (char*)("dodgerblue");
      normNodes[make_pair(legoccurrences.elements[numOccs].tid, legoccurrences.elements[numOccs].tonodeid)] = (char*)("dodgerblue");
      normEdges[make_pair(legoccurrences.elements[numOccs].tid,
         make_pair(legoccurrences.elements[numOccs].fromnodeid, legoccurrences.elements[numOccs].tonodeid))] = (char*)("dodgerblue");
      if (saveBestSubInst)
      {
         fprintf(best_instances_file,"%d ",legoccurrences.elements[numOccs].tid);
         fprintf(best_instances_file,"%d ",legoccurrences.elements[numOccs].fromnodeid);
         fprintf(best_instances_file,"%d ",legoccurrences.elements[numOccs].tonodeid);
         fprintf(best_instances_file,"%d ",
                 getEdgeLabelFromDatabaseTree(legoccurrences.elements[numOccs].tid,
                                              legoccurrences.elements[numOccs].fromnodeid,
                                              legoccurrences.elements[numOccs].tonodeid));
         fprintf(best_instances_file,"%d\n",legoccurrences.elements[numOccs].occurrenceid);
      }
      else
      {
         bestTids.push_back(legoccurrences.elements[numOccs].tid);
         bestFromnodeids.push_back(legoccurrences.elements[numOccs].fromnodeid);
         bestTonodeids.push_back(legoccurrences.elements[numOccs].tonodeid);
         bestEdgelabels.push_back(getEdgeLabelFromDatabaseTree(legoccurrences.elements[numOccs].tid,
                                  legoccurrences.elements[numOccs].fromnodeid,
                                  legoccurrences.elements[numOccs].tonodeid));
         bestOccurrenceids.push_back(legoccurrences.elements[numOccs].occurrenceid);
      }

      // Save for determining if an edge is truly part of a best sub instance...
      bestSubInstanceEdges[legoccurrences.elements[numOccs].tid].fromNode.push_back(legoccurrences.elements[numOccs].fromnodeid);
      bestSubInstanceEdges[legoccurrences.elements[numOccs].tid].toNode.push_back(legoccurrences.elements[numOccs].tonodeid);
   }
   if (legoccurrences.parent != NULL)
   {
      LegOccurrences parent = * (legoccurrences.parent);
      while (parent.elements[0].fromnodeid != maxsize)
      {
         for (unsigned int numParentOccs=0; numParentOccs < parent.elements.size(); numParentOccs++)
         {
            found = false;
            for ( unsigned int i = 0; i < bestSubInstanceEdges[parent.elements[numParentOccs].tid].fromNode.size(); i++ )
            {
               if ((bestSubInstanceEdges[parent.elements[numParentOccs].tid].fromNode[i] == parent.elements[numParentOccs].fromnodeid) ||
                   (bestSubInstanceEdges[parent.elements[numParentOccs].tid].fromNode[i] == parent.elements[numParentOccs].tonodeid) ||
                   (bestSubInstanceEdges[parent.elements[numParentOccs].tid].toNode[i] == parent.elements[numParentOccs].fromnodeid) ||
                   (bestSubInstanceEdges[parent.elements[numParentOccs].tid].toNode[i] == parent.elements[numParentOccs].tonodeid))
               {
                  found = true;
                  i = bestSubInstanceEdges[parent.elements[numParentOccs].tid].fromNode.size();
               }
            }
            if (found)
            {
               normNodes[make_pair(parent.elements[numParentOccs].tid, parent.elements[numParentOccs].fromnodeid)] = (char*)("dodgerblue");
               normNodes[make_pair(parent.elements[numParentOccs].tid, parent.elements[numParentOccs].tonodeid)] = (char*)("dodgerblue");
               normEdges[make_pair(parent.elements[numParentOccs].tid,
                  make_pair(parent.elements[numParentOccs].fromnodeid, parent.elements[numParentOccs].tonodeid))] = (char*)("dodgerblue");
               if (saveBestSubInst)
               {
                  fprintf(best_instances_file,"%d ",parent.elements[numParentOccs].tid);
                  fprintf(best_instances_file,"%d ",parent.elements[numParentOccs].fromnodeid);
                  fprintf(best_instances_file,"%d ",parent.elements[numParentOccs].tonodeid);
                  fprintf(best_instances_file,"%d ",
                          getEdgeLabelFromDatabaseTree(parent.elements[numParentOccs].tid,
                                                       parent.elements[numParentOccs].fromnodeid,
                                                       parent.elements[numParentOccs].tonodeid));
                  fprintf(best_instances_file,"%d\n",parent.elements[numParentOccs].occurrenceid);
               }
               else
               {
                  bestTids.push_back(parent.elements[numParentOccs].tid);
                  bestFromnodeids.push_back(parent.elements[numParentOccs].fromnodeid);
                  bestTonodeids.push_back(parent.elements[numParentOccs].tonodeid);
                  bestEdgelabels.push_back(getEdgeLabelFromDatabaseTree(parent.elements[numParentOccs].tid,
                                             parent.elements[numParentOccs].fromnodeid,
                                             parent.elements[numParentOccs].tonodeid));
                  bestOccurrenceids.push_back(parent.elements[numParentOccs].occurrenceid);
               }

               // Save for determining if an edge is truly part of a best sub instance...
               bestSubInstanceEdges[parent.elements[numParentOccs].tid].fromNode.push_back(parent.elements[numParentOccs].fromnodeid);
               bestSubInstanceEdges[parent.elements[numParentOccs].tid].toNode.push_back(parent.elements[numParentOccs].tonodeid);
            }
         }
         if (parent.parent != NULL)
            parent = * (parent.parent);
      }
   }

   //
   // NOTE: This is where a -prune option could skip this, as it is slow....
   //       Might result in a smaller best substructure being discovered, but
   //       in many cases it will not...  that is why having this as an option
   //       (i.e., -prune) gives the user that control.
   //

   // Now, let's make another pass and add those edges that connect...
   bool bestSubEdge;
   if (legoccurrences.parent != NULL)
   {
      LegOccurrences parent = * (legoccurrences.parent);
      while (parent.elements[0].fromnodeid != maxsize)
      {
         for (unsigned int numParentOccs=0; numParentOccs < parent.elements.size(); numParentOccs++)
         {
            bestSubEdge = false;
            for ( unsigned int i = 0; i < bestSubInstanceEdges[parent.elements[numParentOccs].tid].fromNode.size(); i++ )
            {
               if ((bestSubInstanceEdges[parent.elements[numParentOccs].tid].fromNode[i] == parent.elements[numParentOccs].fromnodeid) ||
                   (bestSubInstanceEdges[parent.elements[numParentOccs].tid].fromNode[i] == parent.elements[numParentOccs].tonodeid) ||
                   (bestSubInstanceEdges[parent.elements[numParentOccs].tid].toNode[i] == parent.elements[numParentOccs].fromnodeid) ||
                   (bestSubInstanceEdges[parent.elements[numParentOccs].tid].toNode[i] == parent.elements[numParentOccs].tonodeid))
               {
                  bestSubEdge = true;
                  i = bestSubInstanceEdges[parent.elements[numParentOccs].tid].fromNode.size();
               }
            }
            if (bestSubEdge)
            {
               // If not already saved, add it
               found = false;
               for ( unsigned int i = 0; i < bestSubInstanceEdges[parent.elements[numParentOccs].tid].fromNode.size(); i++ )
               {
                  if ((bestSubInstanceEdges[parent.elements[numParentOccs].tid].fromNode[i] == parent.elements[numParentOccs].fromnodeid) &&
                      (bestSubInstanceEdges[parent.elements[numParentOccs].tid].toNode[i] == parent.elements[numParentOccs].tonodeid))
                  {
                     found = true;
                     i = bestSubInstanceEdges[parent.elements[numParentOccs].tid].fromNode.size();
                  }
               }
               if (!found)
               {
                  if (saveBestSubInst)
                  {
                     fprintf(best_instances_file,"%d ",parent.elements[numParentOccs].tid);
                     fprintf(best_instances_file,"%d ",parent.elements[numParentOccs].fromnodeid);
                     fprintf(best_instances_file,"%d ",parent.elements[numParentOccs].tonodeid);
                     fprintf(best_instances_file,"%d ",
                             getEdgeLabelFromDatabaseTree(parent.elements[numParentOccs].tid,
                                             parent.elements[numParentOccs].fromnodeid,
                                             parent.elements[numParentOccs].tonodeid));
                     fprintf(best_instances_file,"%d\n",parent.elements[numParentOccs].occurrenceid);
                  }
                  else
                  {
                     bestTids.push_back(parent.elements[numParentOccs].tid);
                     bestFromnodeids.push_back(parent.elements[numParentOccs].fromnodeid);
                     bestTonodeids.push_back(parent.elements[numParentOccs].tonodeid);
                     bestEdgelabels.push_back(getEdgeLabelFromDatabaseTree(parent.elements[numParentOccs].tid,
                                              parent.elements[numParentOccs].fromnodeid,
                                              parent.elements[numParentOccs].tonodeid));
                     bestOccurrenceids.push_back(parent.elements[numParentOccs].occurrenceid);
                  }

                  bestSubInstanceEdges[parent.elements[numParentOccs].tid].fromNode.push_back(parent.elements[numParentOccs].fromnodeid);
                  bestSubInstanceEdges[parent.elements[numParentOccs].tid].toNode.push_back(parent.elements[numParentOccs].tonodeid);
               }
            }
         }
         if (parent.parent != NULL)
            parent = * (parent.parent);
      }
   }

   fclose(best_instances_file);
   delete [] bestSubInstanceEdges;
   if (!saveBestSubInst)
   {
      if ( remove( best_sub_inst_name ) != 0 )
      {
         cout<<"Error deleting file."<<endl;
      }
   }
}

//******************************************************************************
// NAME:    outputAnomalousModifications
//
// INPUTS:  legoccurrences - occurrences of the best sub
//          newLowerScore - flag indicating that score is a new (better) lower one
//          anomValue - anomalous value associated with this sub
//
// RETURN:  none
//
// PURPOSE:  This function saves the current occurrence as the
//           anomalous substructure, and prints it to a file.
//
//******************************************************************************

void GBAD::outputAnomalousModifications(LegOccurrences &legoccurrences, 
                                        bool newLowerScore, 
                                        double anomValue)
{
   FILE *anom_sub_file;

   bool found;
   Tid actualTID;
   Tid lasttid = 0;
   double anomalousValue;

   anomalousNodes.clear();

   anomalousTIDs.clear();
   // First, get the list of anomalous nodes (tids and nodeids)
   for ( unsigned int j = 0; j < legoccurrences.elements.size (); j++ )
   {
      found = false;
      for (unsigned int k = 0; k < bestTids.size(); k++)
      {
         if ((legoccurrences.elements[j].tid == bestTids[k]) &&
             (legoccurrences.elements[j].fromnodeid == bestFromnodeids[k]) &&
             (legoccurrences.elements[j].tonodeid == bestTonodeids[k]))
         {
            found = true;
            k = bestTids.size();
         }
      }
      if (!found)
      {
         actualTID = legoccurrences.elements[j].tid + 1;   // add one so that it matches the
                                                           // transaction in the input graph
         if (actualTID > lasttid)
         {
            anomalousTIDs.push_back(actualTID);
            lasttid = actualTID;
         }
      }
   }
   // Now, adjust anomalous score by the number of occurrences
   // (i.e., the more occurrences, the less anomalous)
   //
   // NOTE: newLowerScore is passed in, but that does not take into account
   //       the number of TIDs that contain this anomaly, so we have to 
   //       adjust it here
   if (anomalousTIDs.size() > 0)
   {
      anomalousValue = (double)anomValue * (double)anomalousTIDs.size();
      if (anomalousValue > bestAnomValue)
         return;
      else if (anomalousValue == bestAnomValue)
         newLowerScore = false;   // add this to any other ones
      else 
      {
         bestAnomValue = anomalousValue;
         newLowerScore = true;
      }
   }

   // If there are no anomalous TIDs, no point in continuing
   if (anomalousTIDs.size() == 0)
      return;

   // If this is from a score lower than any previous scores, we want to
   // start over with the saving of the anomalous substructures - otherwise,
   // just append it to the end of the file
   if (newLowerScore)
   {
      anom_sub_file = fopen ( anom_sub_name, "w" );
      anomNodes.clear();
      anomEdges.clear();
   }
   else
      anom_sub_file = fopen ( anom_sub_name, "a" );

   // output unique transaction IDs
   for ( unsigned int i = 0; i < anomalousTIDs.size(); i++)
   {
      fprintf(anom_sub_file,"%% transaction containing anomalous structure: %d\n",(int)anomalousTIDs[i]);
      fprintf(anom_sub_file,"%% anomalous score: %f\n",bestAnomValue);

      actualTID = anomalousTIDs[i] - 1;

      for ( unsigned int j = 0; j < graphstate.nodes.size (); j++ ) 
      {
         // Find actual vertex number
         // NOTE:  This assumes it exists in the database - if it does not
         //        that would be a big problem...
         unsigned int vertexnumber = 0;
         for (unsigned int n = 0; n < database.trees[actualTID]->nodes.size(); n++)
         {
            if (graphstate.nodes[j].label == database.trees[actualTID]->nodes[n].nodelabel)
            {
               vertexnumber = n;
               n = database.trees[actualTID]->nodes.size();
            }
         }
         fprintf(anom_sub_file,"v %d %d",
                 vertexnumber,
                 (int)database.nodelabels[graphstate.nodes[j].label].inputlabel);

         if (!graphstate.nodes[j].alreadyMarked)
         {
            fprintf(anom_sub_file," <---- anomaly");
            anomNodes[make_pair((int)actualTID,vertexnumber)] = (char*)("red");
         }
         else
         {
            anomNodes[make_pair((int)actualTID,vertexnumber)] = (char*)("orange");
         }
         fprintf(anom_sub_file,"\n");
      }

      for ( unsigned int j = 0; j < graphstate.nodes.size (); j++ ) 
      {
         for ( unsigned int k = 0; k < graphstate.nodes[j].edges.size (); k++ ) 
         {
            GraphState::GSEdge &edge = graphstate.nodes[j].edges[k];
            if ( j < edge.tonode ) 
            {
               // Find actual vertex numbers for both the from and the to nodes
               // NOTE:  This assumes they exist in the database - if they
               //        didn't, that would be a big problem...
               unsigned int tovertexnumber = 0, fromvertexnumber = 0;
               for (unsigned int n = 0; n < database.trees[actualTID]->nodes.size(); n++)
               {
                  if (database.edgelabels[database.edgelabelsindexes[edge.edgelabel]].tonodelabel ==
                      database.trees[actualTID]->nodes[n].nodelabel)
                  {
                     tovertexnumber = n;
                     n = database.trees[actualTID]->nodes.size();
                  }
               }
               for (unsigned int n = 0; n < database.trees[actualTID]->nodes.size(); n++)
               {
                  if (database.edgelabels[database.edgelabelsindexes[edge.edgelabel]].fromnodelabel ==
                      database.trees[actualTID]->nodes[n].nodelabel)
                  {
                     fromvertexnumber = n;
                     n = database.trees[actualTID]->nodes.size();
                  }
               }
               fprintf(anom_sub_file,"e %d %d %d",
                       (int)fromvertexnumber,
                       (int)tovertexnumber,
                       (int)database.edgelabels[database.edgelabelsindexes[edge.edgelabel]].inputedgelabel);

               // NOTE: If a node is anomalous, edges going to that node may
               //       also be marked as anomalous... is this okay?
               if (!graphstate.nodes[j].edges[k].alreadyMarked)
               {
                  fprintf(anom_sub_file," <---- anomaly");
                  anomEdges[make_pair((int)actualTID, make_pair(fromvertexnumber, tovertexnumber))] = (char*)("red");
               }
               else
               {
                  anomEdges[make_pair((int)actualTID, make_pair(fromvertexnumber, tovertexnumber))] = (char*)("orange");
               }
               fprintf(anom_sub_file,"\n");
            }      
         }
      }
   }

   // Add demarcation between the anomalous substructures
   fprintf(anom_sub_file,"%%\n%%\n"); 
   fclose(anom_sub_file);
}


//******************************************************************************
// NAME:    overlapsBestSubInstances
//
// INPUTS:  legoccurrences - occurrences of the best sub
//
// RETURN:  FALSE is there are no anomalous occurrences, otherwise TRUE.
//
// PURPOSE:  This function returns TRUE if any of the nodes in the current
//           graph substructure overlaps with any of the nodes from instances
//           of the best substructure.
//
//******************************************************************************

bool GBAD::overlapsBestSubInstances(LegOccurrences &legoccurrences)
{
   for (unsigned int best=0; best<bestTids.size(); best++)
   {
      // first look in legoccurrences
      for (unsigned int numOccs=0; numOccs<legoccurrences.elements.size(); numOccs++)
      {
         if ((legoccurrences.elements[numOccs].tid == bestTids[best]) &&
             (legoccurrences.elements[numOccs].fromnodeid == bestFromnodeids[best]) &&
             (legoccurrences.elements[numOccs].tonodeid == bestTonodeids[best]))
         {
            return true;
         }
      }
   }
   return false;
}


//******************************************************************************
// NAME:    findBestSub
//
// INPUTS:  legoccurrences - subgraph occurrences
//
// RETURN:  none
//
// PURPOSE:  This function allows us to skip any substructures that are not of
//           the appropriate size to be considered for anomalousness.
//
//******************************************************************************

void GBAD::findBestSub(LegOccurrences &legoccurrences) 
{
   // Get size of graph substructure
   int numVerticesAndEdges = graphstate.nodes.size ();
   for ( unsigned int i = 0; i < graphstate.nodes.size (); i++ ) 
   {
      for ( unsigned int j = 0; j < graphstate.nodes[i].edges.size (); j++ ) 
      {
         GraphState::GSEdge &edge = graphstate.nodes[i].edges[j];
         if ( i < edge.tonode ) 
         {
            numVerticesAndEdges++;
         }      
      }
   }

   // Set out how many instances there are
   //
   // NOTE:  Tried to use the getNumInstances function, but it cause some
   //        inconsistent results when used here.
   //
   int numInstances = legoccurrences.frequency;

   // If the size of this occurrence is greater than the size of the best
   // substructure so far, save it.
   //
   // NOTE:  Thus, we are only keeping the first, largest substructure.
   //
   if ((numVerticesAndEdges * numInstances) > bestSizeTimesFreq)
   {
      bestSizeTimesFreq = numVerticesAndEdges * numInstances;
      saveBestSub(legoccurrences);
   }
}


//******************************************************************************
// NAME:    gbadMDL
//
// INPUTS:  legoccurrences - subgraph occurrences
//
// RETURN:  none
//
// PURPOSE:  This function applies the GBAD-MDL algorithm to the specified
//           substructure (graphstate).
//
//******************************************************************************

void GBAD::gbadMDL(LegOccurrences &legoccurrences) 
{
   // In the anomalous searching phase, we want to search for instances that
   // closely match the best substructure and are of the same size (GBAD-MDL).
   
   //
   // NOTE:  Only look at substructures that have the same number of nodes
   //
   if ((phase == 4) && (bestSubTree->nodes.size() == graphstate.nodes.size()))
   {
      double matchPercentage = 0.0;
      if (graphMatch(& matchPercentage))
      {
         // NOTE:  There is not a good way to compare real numbers because of
         //        the issue with precision.  For now, this is just a straight
         //        comparison.  However, this will need to be revisited if the
         //        precision is too far to the right of the decimal.
         if ((matchPercentage <= bestAnomValue) && (matchPercentage > 0.000000) &&
             (!overlapsBestSubInstances(legoccurrences)))
         {
            bool flag;
            if (matchPercentage < bestAnomValue)
            {
               // Debug-only
               cout << "   ... new lower anomalous score (MDL): " << matchPercentage 
                    << "  (number of instances: " << legoccurrences.elements.size() 
                    << ")" << endl;
               flag = true;
            }
            else
            {
               // Debug-only
               cout << "   ... same lower anomalous score (MDL): " << matchPercentage 
                    << "  (number of instances: " << legoccurrences.elements.size() 
                    << ")" << endl;
               flag = false;
            }
            outputAnomalousModifications(legoccurrences,flag,matchPercentage);
         }
      }
      else
      {
         // Debug-only
         //cout << "*** Substructure is NOT potentially anomalous (matchPercentage = " 
         //     << matchPercentage << ") " << endl;
      }
   }
}


//******************************************************************************
// NAME:    getEdgeLabelFromDatabaseTree
//
// INPUTS:  tid - trandsaction identifier
//          fromNodeId - from vertex
//          toNodeId - to vertex
//
// RETURN:  edge labl
//
// PURPOSE:  This function returns the label of the edge that connects the
//           two supplied vertices.
//
// NOTE:     What if two vertices have multiple, different edges?
//
//******************************************************************************
InputEdgeLabel GBAD::getEdgeLabelFromDatabaseTree(Tid tid, NodeId fromNodeId, 
                                                  NodeId toNodeId)
{
   for (unsigned int j = 0; j < database.trees[tid]->nodes.size(); j++)
   {
      if (j == fromNodeId)
      {
         for (int k = 0; k < database.trees[tid]->nodes[fromNodeId].edges.size(); k++)
         {
            if (database.trees[tid]->nodes[j].edges[k].tonode == toNodeId)
               return database.edgelabels[database.edgelabelsindexes[database.trees[tid]->nodes[j].edges[k].edgelabel]].inputedgelabel;
         }
      }
   }
   cout << "ERROR:  no edge label for edge from " << fromNodeId << " to " 
        << toNodeId << "(tid = " << tid << ")" << endl;
   return 0;
}


//******************************************************************************
// NAME:    gbadMPS
//
// INPUTS:  legoccurrences - subgraph occurrences
//
// RETURN:  none
//
// PURPOSE:  This function applies the GBAD-MPS algorithm to the specified
//           substructure (graphstate).
//
// NOTE:  A potential issue is multiple edges between two vertices, where
//        a missing edge is one of them.
//
//******************************************************************************

void GBAD::gbadMPS(LegOccurrences &legoccurrences) 
{
   EdgeLabel edgeLabel;
   bool found;
   vector<Tid> uniqueTids;
   vector<OccurrenceId> uniqueOccurrenceids;
   vector<Tid> anomalousTids;
   vector<NodeId> anomalousFrom;
   vector<NodeId> anomalousTo;
   vector<EdgeLabel> anomalousEdgeLabel;
   vector<OccurrenceId> anomalousOccurrenceids;
   
   //
   // Now let's see if this is the best substructure (which is what we want
   // when running the MPS algorithm)...
   //
   double matchPercentage = 0.0;
   //
   // NOTE:  Only look at substructures that have the same number of nodes
   //
   if (bestSubTree->nodes.size() == graphstate.nodes.size())
   {
      graphMatch(& matchPercentage);
      if (relDiff(matchPercentage,0.000000) > 0.000000)
         return;
   }
   else
      return;

   //
   // Search legoccurrences (of the best sub) for any edges that are not in
   // the best sub instances
   //
   if (legoccurrences.parent != NULL)
   {
      LegOccurrences parent = * (legoccurrences.parent);
      while (parent.elements[0].fromnodeid != maxsize)   // maxsize indicates end of info
      {
         for (unsigned int numParentOccs=0; numParentOccs < parent.elements.size(); numParentOccs++)
         {
            edgeLabel = getEdgeLabelFromDatabaseTree(parent.elements[numParentOccs].tid,
                                                     parent.elements[numParentOccs].fromnodeid,
                                                     parent.elements[numParentOccs].tonodeid);
            found = false;
            for (unsigned int numBestEdges = 0; numBestEdges < bestTids.size(); numBestEdges++)
            {
               if ((parent.elements[numParentOccs].tid == bestTids[numBestEdges]) &&
                   (parent.elements[numParentOccs].fromnodeid == bestFromnodeids[numBestEdges]) &&
                   (parent.elements[numParentOccs].tonodeid == bestTonodeids[numBestEdges]) &&
                   (parent.elements[numParentOccs].occurrenceid == bestOccurrenceids[numBestEdges]) &&
                   (edgeLabel == bestEdgelabels[numBestEdges]))
               {
                  found = true;
                  numBestEdges = bestTids.size();
               }
            }
            // If not found, then potentially anomaly, so store the edge
            if (!found)
            {
               // Debug
               //cout << "Edge in potentially anomalous instance: (tid = " << parent.elements[numParentOccs].tid
               //     << ")  e " << parent.elements[numParentOccs].fromnodeid
               //     << " " << parent.elements[numParentOccs].tonodeid
               //     << " " << edgeLabel << endl;
               for (unsigned int numUniqueTid=0; numUniqueTid < uniqueTids.size(); numUniqueTid++)
               {
                  if ((uniqueTids[numUniqueTid] == parent.elements[numParentOccs].tid) &&
                      (uniqueOccurrenceids[numUniqueTid] == parent.elements[numParentOccs].occurrenceid))
                  {
                     found = true;
                     numUniqueTid = uniqueTids.size();
                  }
               }
               if (!found)
               {
                  uniqueTids.push_back(parent.elements[numParentOccs].tid);
                  uniqueOccurrenceids.push_back(parent.elements[numParentOccs].occurrenceid);
                  outputAnomalousInstanceWithMissingInfo(parent.elements[numParentOccs].fromnodeid,
                                                         parent.elements[numParentOccs].tonodeid,
                                                         edgeLabel,
                                                         parent.elements[numParentOccs].tid,
                                                         legoccurrences);
               }
            }
         }
         if (parent.parent != NULL)
            parent = * (parent.parent);
      }
   }
}


//******************************************************************************
// NAME:    gbadPROB
//
// INPUTS:  legoccurrences - subgraph occurrences
//
// RETURN:  none
//
// PURPOSE:  This function applies the GBAD-PROB algorithm to the specified
//           substructure (graphstate).
//
//
// NOTE:  May not be able to handle multiple edges between two nodes...
//
//******************************************************************************

void GBAD::gbadPROB() 
{
   vector<Tid> listOfUniqueBestTids;
   bool found;

   // Generate list of unique tids (from bestTids)
   for (unsigned int numBestInstanceEntries=0; numBestInstanceEntries < bestTids.size(); numBestInstanceEntries++)
   {
      found = false;
      for (unsigned int numUniqueBestTids=0; numUniqueBestTids < listOfUniqueBestTids.size(); numUniqueBestTids++)
      {
         if (listOfUniqueBestTids[numUniqueBestTids] == bestTids[numBestInstanceEntries])
         {
            found = true;
            numUniqueBestTids = listOfUniqueBestTids.size();
         }
      }
      if (!found)
         listOfUniqueBestTids.push_back(bestTids[numBestInstanceEntries]);
   }

   vector<Tid> listOfUniqueBestVertexTids;
   vector<NodeId> listOfUniqueBestVertexFromnodeids;
   vector<NodeId> listOfUniqueBestVertexTonodeids;

   // Generate list of unique normative vertices (from bestFromnodeids and bestTonodeids, associated with bestTids)
   for (unsigned int numBestInstanceEntries=0; numBestInstanceEntries < bestTids.size(); numBestInstanceEntries++)
   {
      found = false;
      for (unsigned int numUniqueBestVertices=0; numUniqueBestVertices < listOfUniqueBestVertexTids.size(); numUniqueBestVertices++)
      {
         if ((listOfUniqueBestVertexTids[numUniqueBestVertices] == bestTids[numBestInstanceEntries]) &&
             (listOfUniqueBestVertexFromnodeids[numUniqueBestVertices] == bestFromnodeids[numBestInstanceEntries]) &&
             (listOfUniqueBestVertexTonodeids[numUniqueBestVertices] == bestTonodeids[numBestInstanceEntries]))
         {
            found = true;
            numUniqueBestVertices = listOfUniqueBestVertexTids.size();
         }
      }
      if (!found)
      {
         listOfUniqueBestVertexTids.push_back(bestTids[numBestInstanceEntries]);
         listOfUniqueBestVertexFromnodeids.push_back(bestFromnodeids[numBestInstanceEntries]);
         listOfUniqueBestVertexTonodeids.push_back(bestTonodeids[numBestInstanceEntries]);
      }
   }

   // Using database tree for each unique tid, generate list of EVERY 
   // from-to where either (or both) the from and to are in the list of 
   // unique normative vertices AND the from-to is not in the unique list
   Tid tidnumber;
   vector<Tid> tidExtension;
   vector<NodeId> fromExtension; 
   vector<NodeId> toExtension; 
   vector<EdgeLabel> labelExtension;

   for (unsigned int numUniqueBestTids = 0; numUniqueBestTids < listOfUniqueBestTids.size(); numUniqueBestTids++)
   {
      tidnumber = listOfUniqueBestTids[numUniqueBestTids];

      for (unsigned int numNodeInTree = 0; numNodeInTree < database.trees[tidnumber]->nodes.size(); numNodeInTree++)
      {
         for (int numEdge = 0; numEdge < database.trees[tidnumber]->nodes[numNodeInTree].edges.size(); numEdge++)
         {
            found = false;
            for (unsigned int numUniqueBestVertices=0; numUniqueBestVertices < listOfUniqueBestVertexTids.size(); numUniqueBestVertices++)
            {
               if (tidnumber == listOfUniqueBestVertexTids[numUniqueBestVertices])
               {
                  if ((numNodeInTree == listOfUniqueBestVertexFromnodeids[numUniqueBestVertices]) &&
                      (database.trees[tidnumber]->nodes[numNodeInTree].edges[numEdge].tonode == listOfUniqueBestVertexTonodeids[numUniqueBestVertices]))
                  {
                     found = true;
                     numUniqueBestVertices = listOfUniqueBestVertexTids.size();
                  }
               }
            }
            // if no match, this is an extension to save...
            if (!found)
            {
               tidExtension.push_back(tidnumber);
               fromExtension.push_back(numNodeInTree);
               toExtension.push_back(database.trees[tidnumber]->nodes[numNodeInTree].edges[numEdge].tonode);
               labelExtension.push_back(database.edgelabels[database.edgelabelsindexes[database.trees[tidnumber]->nodes[numNodeInTree].edges[numEdge].edgelabel]].inputedgelabel);
            }
         }
      }
   }

   // If there aren't any extension, no need in continuing
   if (tidExtension.size() == 0)
   {
      cout << "GBAD-P:  No anomalous insertions." << endl;
      return;
   }

   // Count each unique edge (tid-from-to)
   vector<Tid> uniqueEdgeTid;
   vector<NodeId> uniqueEdgeFrom;
   vector<NodeId> uniqueEdgeTo;
   vector<EdgeLabel> uniqueEdgeLabel;
   vector<double> uniqueEdgeProbability;
   vector<unsigned int> uniqueEdgeCount;
   double tempCount;
   double tempProbability;

   // First, create an initial count for each edge
   for (unsigned int numExtension = 0; numExtension < tidExtension.size(); numExtension++)
   {
      uniqueEdgeTid.push_back(tidExtension[numExtension]);
      uniqueEdgeFrom.push_back(fromExtension[numExtension]);
      uniqueEdgeTo.push_back(toExtension[numExtension]);
      uniqueEdgeLabel.push_back(labelExtension[numExtension]);
      uniqueEdgeProbability.push_back((double)1.0);
      uniqueEdgeCount.push_back(1);
   }

   // Then, update the count for each edge
   for (unsigned int numUnique = 0; numUnique < uniqueEdgeLabel.size(); numUnique++)
   {
      tempCount = 0;
      for (unsigned int numEdges = 0; numEdges < uniqueEdgeLabel.size(); numEdges++)
      {
         if ((uniqueEdgeFrom[numUnique] == uniqueEdgeFrom[numEdges]) &&
             (uniqueEdgeTo[numUnique] == uniqueEdgeTo[numEdges]) &&
             (uniqueEdgeLabel[numUnique] == uniqueEdgeLabel[numEdges]))
         {
            tempCount++;
            uniqueEdgeCount[numUnique] = tempCount;
         }
      }
   }

   //
   // And finally, after finding the count for the edge, now need to store the probability
   // for the edge in each appropriate tid...
   //
   // NOTE:  The probability is solely based upon the existence of the edge.
   //        The source and/or destination vertices are not taken into
   //        account.
   //
   for (unsigned int numUniqueEdge = 0; numUniqueEdge < uniqueEdgeTid.size(); numUniqueEdge++)
   {
      tempProbability = (double)uniqueEdgeCount[numUniqueEdge]/(double)numBestInstances;
      uniqueEdgeProbability[numUniqueEdge] = tempProbability;
   }

   // Find the lowest probability
   double lowestProbability = 1.0;
   for (unsigned int numUniqueEdge = 0; numUniqueEdge < uniqueEdgeTid.size(); numUniqueEdge++)
   {
      if (uniqueEdgeProbability[numUniqueEdge] < lowestProbability)
         lowestProbability = uniqueEdgeProbability[numUniqueEdge];
   }
   bestAnomValue = lowestProbability;

   // Find all of the TIDs that have these lowest-probability edges
   vector<Tid> anomalousTids;
   vector<NodeId> anomalousFrom;
   vector<NodeId> anomalousTo;
   vector<EdgeLabel> anomalousEdgeLabel;
   vector<double> anomalousProbability;
   for (unsigned int numUniqueEdge = 0; numUniqueEdge < uniqueEdgeTid.size(); numUniqueEdge++)
   {
      if (uniqueEdgeProbability[numUniqueEdge] == lowestProbability)
      {
         found = false;
         for (unsigned int numAnom=0; numAnom < anomalousTids.size(); numAnom++)
         {
            if ((anomalousTids[numAnom] == uniqueEdgeTid[numUniqueEdge]) &&
                (((anomalousFrom[numAnom] == uniqueEdgeFrom[numUniqueEdge]) &&
                  (anomalousTo[numAnom] == uniqueEdgeTo[numUniqueEdge])) ||
                 ((anomalousFrom[numAnom] == uniqueEdgeTo[numUniqueEdge]) &&
                  (anomalousTo[numAnom] == uniqueEdgeFrom[numUniqueEdge]))) &&
                (anomalousEdgeLabel[numAnom] == uniqueEdgeLabel[numUniqueEdge]))
            {
               found = true;
               numAnom = anomalousTids.size();
            }
         }
         if (!found)
         {
            anomalousTids.push_back(uniqueEdgeTid[numUniqueEdge]);
            anomalousFrom.push_back(uniqueEdgeFrom[numUniqueEdge]);
            anomalousTo.push_back(uniqueEdgeTo[numUniqueEdge]);
            anomalousEdgeLabel.push_back(uniqueEdgeLabel[numUniqueEdge]);
            anomalousProbability.push_back(uniqueEdgeProbability[numUniqueEdge]);
         }
      }
   }

   // And finally, output associated best instance as well as the anomalous edge
   remove(anom_sub_name);
   for (unsigned int numAnomalousTid = 0; numAnomalousTid < anomalousTids.size(); numAnomalousTid++)
   {
      outputAnomalousInstance(anomalousFrom[numAnomalousTid],
                              anomalousTo[numAnomalousTid],
                              anomalousEdgeLabel[numAnomalousTid],
                              anomalousTids[numAnomalousTid],
                              anomalousProbability[numAnomalousTid]);
   }
}


//******************************************************************************
// NAME:    outputAnomalousInstance
//
// INPUTS:  fromVertex - source vertex of anomalous edge
//          toVertex - destination vertex of anomalous edge
//          edgeLabel - label of anomalous edge
//          tidNumber - TID number of anomalous edge
//          probability - probability of existence for anomalous edge
//
// RETURN:  none
//
// PURPOSE:  This function outputs the entire anamolous instances for the
//           specified anomalous edge.  The anomalous instance is written to 
//           the file anom_sub_name.
//
// NOTE:  What is actually written to the anom_sub_name file is a little confusing.
//        However, when you visualize it in a picture of the graph, you will
//        notice that the normative vertices and edges are correct, and the 
//        anomalous vertex and edge is correct.  The issue is the way it is
//        grouped/broken up in the anom_sub_name file.
//
//******************************************************************************

void GBAD::outputAnomalousInstance(NodeId fromVertex, NodeId toVertex, 
                                   EdgeLabel edgeLabel, Tid tidNumber,
                                   double probability)
{
   // Then, get all vertices and edges of the best instance associated with this anomaly
   vector<NodeId> associatedVertices;
   vector<NodeLabel> associatedLabels;
   vector<NodeId> associatedFrom;
   vector<NodeId> associatedTo;
   vector<EdgeLabel> associatedEdgeLabel;

   associatedVertices.push_back(fromVertex);
   associatedLabels.push_back(database.nodelabels[database.trees[tidNumber]->nodes[fromVertex].nodelabel].inputlabel);
   associatedVertices.push_back(toVertex);
   associatedLabels.push_back(database.nodelabels[database.trees[tidNumber]->nodes[toVertex].nodelabel].inputlabel);
   bool found;
   bool verticesToAdd = true;
   bool fromVertexAnomalous = true;
   bool toVertexAnomalous = true;

   while (verticesToAdd)
   {
      verticesToAdd = false;
      for (unsigned int numBestInstanceEntries=0; 
           numBestInstanceEntries < bestTids.size(); 
           numBestInstanceEntries++)
      {
         if (bestTids[numBestInstanceEntries] == tidNumber)
         {
            // If this best node is the from node from the anomalous edge,
            // then don't mark it anomalous (because it is part of the best
            // substructure
            if ((fromVertex == bestFromnodeids[numBestInstanceEntries]) ||
                (fromVertex == bestTonodeids[numBestInstanceEntries]))
               fromVertexAnomalous = false;
            if ((toVertex == bestFromnodeids[numBestInstanceEntries]) ||
                (toVertex == bestTonodeids[numBestInstanceEntries]))
               toVertexAnomalous = false;
       
            for (unsigned int i=0; i < associatedVertices.size(); i++)
            {
               if (bestFromnodeids[numBestInstanceEntries] == associatedVertices[i])
               {
                  found = false;
                  for (unsigned int j=0; j < associatedVertices.size(); j++)
                  {
                     if (bestTonodeids[numBestInstanceEntries] == associatedVertices[j])
                     {
                        found = true;
                        j = associatedVertices.size();
                     }
                  }
                  if (!found)
                  {
                     associatedVertices.push_back(bestTonodeids[numBestInstanceEntries]);
                     associatedLabels.push_back(database.nodelabels[database.trees[tidNumber]->nodes[bestTonodeids[numBestInstanceEntries]].nodelabel].inputlabel);
                     verticesToAdd = true;
                  }
                  // Add edge
                  found = false;
                  for (unsigned int j=0; j < associatedEdgeLabel.size(); j++)
                  {
                     if ((bestFromnodeids[numBestInstanceEntries] == associatedFrom[j]) &&
                         (bestTonodeids[numBestInstanceEntries] == associatedTo[j]) &&
                         (bestEdgelabels[numBestInstanceEntries] == associatedEdgeLabel[j]))
                     {
                        found = true;
                        j = associatedEdgeLabel.size();
                     }
                  }
                  if (!found)
                  {
                     associatedFrom.push_back(bestFromnodeids[numBestInstanceEntries]);
                     associatedTo.push_back(bestTonodeids[numBestInstanceEntries]);
                     associatedEdgeLabel.push_back(bestEdgelabels[numBestInstanceEntries]);
                  }
               }
               if (bestTonodeids[numBestInstanceEntries] == associatedVertices[i])
               {
                  found = false;
                  for (unsigned int j=0; j < associatedVertices.size(); j++)
                  {
                     if (bestFromnodeids[numBestInstanceEntries] == associatedVertices[j])
                     {
                        found = true;
                        j = associatedVertices.size();
                     }
                  }
                  if (!found)
                  {
                     associatedVertices.push_back(bestFromnodeids[numBestInstanceEntries]);
                     associatedLabels.push_back(database.nodelabels[database.trees[tidNumber]->nodes[bestFromnodeids[numBestInstanceEntries]].nodelabel].inputlabel);
                     verticesToAdd = true;
                  }
                  // Add edge
                  found = false;
                  for (unsigned int j=0; j < associatedEdgeLabel.size(); j++)
                  {
                     if ((bestFromnodeids[numBestInstanceEntries] == associatedFrom[j]) &&
                         (bestTonodeids[numBestInstanceEntries] == associatedTo[j]) &&
                         (bestEdgelabels[numBestInstanceEntries] == associatedEdgeLabel[j]))
                     {
                        found = true;
                        j = associatedEdgeLabel.size();
                     }
                  }
                  if (!found)
                  {
                     associatedFrom.push_back(bestFromnodeids[numBestInstanceEntries]);
                     associatedTo.push_back(bestTonodeids[numBestInstanceEntries]);
                     associatedEdgeLabel.push_back(bestEdgelabels[numBestInstanceEntries]);
                  }
               }
            }
         }
      }
   }

   //
   // If the substructure is the same size as the best substructure (or
   // smaller) it can't be anomalous BECAUSE this function is called by the
   // gbadPROB algorithm, which is looking for substructures larger than the
   // best substructure (because they have an extra edge/vertex)
   //
   // In reality, the size should be no more than 2 bigger than the best
   // substructure, because the largest anomalous substructure can not contain
   // more than one extra vertex and one extra edge.
   //
   int anomSize = associatedVertices.size() + associatedEdgeLabel.size();
   if (anomSize < bestSize)
      return;

   //
   // And finally, output all of the associated vertices and edges, 
   // plus the anomalous vertex and edge
   //
   // open file for reporting anomalies
   fstream anom_sub_file;
   anom_sub_file.open (anom_sub_name, fstream::out | fstream::app);

   anom_sub_file << "% transaction containing anomalous structure: " 
                 << (tidNumber + 1) << endl;
   anom_sub_file << "% probability: " << fixed << probability << endl;

   anom_sub_file << "v " << associatedVertices[0] << " " << associatedLabels[0];
   if (!fromVertexAnomalous)
   {
      anomNodes[make_pair(tidNumber, associatedVertices[0])] = (char*)("orange");
      anom_sub_file << endl;
   }
   else
   {
      anom_sub_file << "  <--- anomaly" << endl;
      anomNodes[make_pair(tidNumber, associatedVertices[0])] = (char*)("red");
   }
   anom_sub_file << "v " << associatedVertices[1] << " " << associatedLabels[1];
   if (!toVertexAnomalous)
   {
      anomNodes[make_pair(tidNumber, associatedVertices[1])] = (char*)("orange");
      anom_sub_file << endl;
   }
   else
   {
      anom_sub_file << "  <--- anomaly" << endl;
      anomNodes[make_pair(tidNumber, associatedVertices[1])] = (char*)("red");
   }

   for (unsigned int i=2; i<associatedVertices.size(); i++)
      anom_sub_file << "v " << associatedVertices[i] << " " 
                    << associatedLabels[i] << endl;
   for (unsigned int i=0; i<associatedEdgeLabel.size(); i++)
      anom_sub_file << "e " << associatedFrom[i] << " " 
                    << associatedTo[i] << " " << associatedEdgeLabel[i] << endl;
   anom_sub_file << "e " << fromVertex << " " << toVertex << " " 
                 << edgeLabel << "  <--- anomaly" << endl;
   anomEdges[make_pair(tidNumber, make_pair(fromVertex, toVertex))] = (char*)("red");

}


//******************************************************************************
// NAME:    outputAnomalousInstanceWithMissingInfo
//
// INPUTS:  fromVertex - source vertex of anomalous edge
//          toVertex - destination vertex of anomalous edge
//          edgeLabel - label of anomalous edge
//          tidNumber - TID number of anomalous edge
//          legoccurrences - all edges associated with a subgraph
//
// RETURN:  none
//
// PURPOSE:  This function outputs the entire anamolous instance starting from
//           a specified edge.  The anomalous instance is written to 
//           the file anom_sub_name.
//
//******************************************************************************

void GBAD::outputAnomalousInstanceWithMissingInfo(NodeId fromVertex, 
                                                  NodeId toVertex, 
                                                  EdgeLabel edgeLabel, 
                                                  Tid tidNumber,
                                                  LegOccurrences &legoccurrences) 
{
   vector<NodeId> anomVertex;
   vector<NodeLabel> anomVertexLabel;
   vector<NodeId> anomFrom;
   vector<NodeLabel> anomFromLabel;
   vector<NodeId> anomTo;
   vector<NodeLabel> anomToLabel;
   vector<EdgeLabel> anomEdgeLabel;
   vector<Tid> anomVertexTid;
   vector<Tid> anomEdgeTid;
   NodeId fromnodeid;
   NodeId tonodeid;
   InputNodeLabel tolabel, fromlabel;
   EdgeLabel edgelabel;

   // store initially known anomalous edge
   anomVertex.push_back(fromVertex);
   anomVertexLabel.push_back(database.nodelabels[database.trees[tidNumber]->nodes[fromVertex].nodelabel].inputlabel);
   anomVertexTid.push_back(tidNumber);
   unsigned int sizeOfAnomInstance = 1;

   bool found;
   bool addedVerticesAndEdges = true;

   // NOTE:  We can skip the top level because the anomalous edges would never
   //        have been there.
   //
   // First, search through legoccurrences for all edges directly or indirectly
   // related to the anomalous edge
   //
   //
   // NOTE This first if-then statement is the slowest part of the processing...
   //      it takes anywhere from 0.02 to 0.03 seconds, with many calls to this
   //      function...
   //
   if (legoccurrences.parent != NULL)
   {
      // First, get vertices associated with the anomalous instance
      while (addedVerticesAndEdges)
      {
         addedVerticesAndEdges = false;
         for (unsigned int numAnom=0; numAnom < anomVertex.size(); numAnom++)
         {
            LegOccurrences parent = * (legoccurrences.parent);
            while (parent.elements[0].fromnodeid != maxsize)   // maxsize indicates end of info
            {
               for (unsigned int numParentOccs=0; numParentOccs < parent.elements.size(); numParentOccs++)
               {
                  if (parent.elements[numParentOccs].tid == tidNumber)
                  {
                     // check for edges with this anomalous instance vertex
                     fromnodeid = parent.elements[numParentOccs].fromnodeid;
                     tonodeid = parent.elements[numParentOccs].tonodeid;
                     if ((fromnodeid == anomVertex[numAnom]) &&
                         (tidNumber == anomVertexTid[numAnom]))
                     {
                        // Does the other end of the edge (the to vertex) already exist?
                        found = false;
                        for (unsigned int n=0; n < anomVertex.size(); n++)
                        {
                           if (tonodeid == anomVertex[n])
                           {
                              found = true;
                              n = anomVertex.size();
                           }
                        }
                        if (!found)
                        {
                           anomVertex.push_back(tonodeid);
                           tolabel = database.nodelabels[database.trees[tidNumber]->nodes[tonodeid].nodelabel].inputlabel;
                           anomVertexLabel.push_back(tolabel);
                           addedVerticesAndEdges = true;
                           anomVertexTid.push_back(tidNumber);
                           sizeOfAnomInstance++;
                        }
                     }
                     if ((tonodeid == anomVertex[numAnom]) &&
                         (tidNumber == anomVertexTid[numAnom]))
                     {
                        // Does the other end of the edge (the from vertex) already exist?
                        found = false;
                        for (unsigned int n=0; n < anomVertex.size(); n++)
                        {
                           if (fromnodeid == anomVertex[n])
                           {
                              found = true;
                              n = anomVertex.size();
                           }
                        }
                        if (!found)
                        {
                           anomVertex.push_back(fromnodeid);
                           fromlabel = database.nodelabels[database.trees[tidNumber]->nodes[fromnodeid].nodelabel].inputlabel;
                           anomVertexLabel.push_back(fromlabel);
                           addedVerticesAndEdges = true;
                           anomVertexTid.push_back(tidNumber);
                           sizeOfAnomInstance++;
                        }
                     }
                     // Perhaps the edge is new...
                     if (((fromnodeid == anomVertex[numAnom]) ||
                          (tonodeid == anomVertex[numAnom])) &&
                         (tidNumber == anomVertexTid[numAnom]))
                     {
                        edgelabel = getEdgeLabelFromDatabaseTree(tidNumber,
                                                                 fromnodeid,
                                                                 tonodeid);
                        found = false;
                        for (unsigned int numAnomEdge=0; numAnomEdge < anomEdgeLabel.size(); numAnomEdge++)
                        {
                           if ((anomFrom[numAnomEdge] == fromnodeid) &&
                               (anomTo[numAnomEdge] == tonodeid) &&
                               (anomEdgeLabel[numAnomEdge] == edgelabel) &&
                               (anomEdgeTid[numAnomEdge] == tidNumber))
                           {
                              found = true;
                              numAnomEdge = anomEdgeLabel.size();
                           }
                        }
                        if (!found)
                        {
                           fromlabel = database.nodelabels[database.trees[tidNumber]->nodes[fromnodeid].nodelabel].inputlabel;
                           tolabel = database.nodelabels[database.trees[tidNumber]->nodes[tonodeid].nodelabel].inputlabel;
                           anomFrom.push_back(fromnodeid);
                           anomFromLabel.push_back(fromlabel);
                           anomTo.push_back(tonodeid);
                           anomToLabel.push_back(tolabel);
                           anomEdgeLabel.push_back(edgelabel);
                           addedVerticesAndEdges = true;
                           anomEdgeTid.push_back(tidNumber);
                           sizeOfAnomInstance++;
                        }
                     }
                  }
               }
               if (parent.parent != NULL)
                  parent = * (parent.parent);
            }
         }
      }
   }

   // Second, after getting the best tid number, get all of the edges of a good
   // instance that are not one of the edges from the anomalous instance
   Tid goodTidNumber = bestTids[0];

   vector<NodeId> goodFromNodeid;
   vector<InputNodeLabel> goodFromLabel;
   vector<NodeId> goodToNodeid;
   vector<InputNodeLabel> goodToLabel;
   vector<EdgeLabel> goodEdgeLabel;
   vector<Tid> goodTid;
   for (unsigned int numOccs=0; numOccs<legoccurrences.elements.size(); numOccs++)
   {
      if (legoccurrences.elements[numOccs].tid == goodTidNumber)
      {
         fromnodeid = legoccurrences.elements[numOccs].fromnodeid;
         fromlabel = database.nodelabels[database.trees[goodTidNumber]->nodes[fromnodeid].nodelabel].inputlabel;
         tonodeid = legoccurrences.elements[numOccs].tonodeid;
         tolabel = database.nodelabels[database.trees[goodTidNumber]->nodes[tonodeid].nodelabel].inputlabel;
         edgelabel = getEdgeLabelFromDatabaseTree(goodTidNumber,
                                                  fromnodeid,
                                                  tonodeid);
         found = false;
         for (unsigned int numAnomEdge=0; numAnomEdge < anomEdgeLabel.size(); numAnomEdge++)
         {
            if ((fromlabel == anomFromLabel[numAnomEdge]) &&
                (tolabel == anomToLabel[numAnomEdge]) &&
                (edgelabel == anomEdgeLabel[numAnomEdge]) &&
                (tidNumber == anomEdgeTid[numAnomEdge]))
            {
               found = true;
               numAnomEdge = anomEdgeLabel.size();
            }
         }
         if (!found)
         {
            goodFromNodeid.push_back(fromnodeid);
            goodFromLabel.push_back(fromlabel);
            goodToNodeid.push_back(tonodeid);
            goodToLabel.push_back(tolabel);
            goodEdgeLabel.push_back(edgelabel);
            goodTid.push_back(tidNumber);
         }
         numOccs = legoccurrences.elements.size();
      }
   }
   if (legoccurrences.parent != NULL)
   {
      LegOccurrences parent = * (legoccurrences.parent);
      while (parent.elements[0].fromnodeid != maxsize)
      {
         for (unsigned int numParentOccs=0; numParentOccs < parent.elements.size(); numParentOccs++)
         {
            if (parent.elements[numParentOccs].tid == goodTidNumber)
            {
               fromnodeid = parent.elements[numParentOccs].fromnodeid;
               fromlabel = database.nodelabels[database.trees[goodTidNumber]->nodes[fromnodeid].nodelabel].inputlabel;
               tonodeid = parent.elements[numParentOccs].tonodeid;
               tolabel = database.nodelabels[database.trees[goodTidNumber]->nodes[tonodeid].nodelabel].inputlabel;
               edgelabel = getEdgeLabelFromDatabaseTree(goodTidNumber,
                                                        fromnodeid,
                                                        tonodeid);
               found = false;
               for (unsigned int numAnomEdge=0; numAnomEdge < anomEdgeLabel.size(); numAnomEdge++)
               {
                  if ((fromlabel == anomFromLabel[numAnomEdge]) &&
                      (tolabel == anomToLabel[numAnomEdge]) &&
                      (edgelabel == anomEdgeLabel[numAnomEdge]) &&
                      (tidNumber == anomEdgeTid[numAnomEdge]))
                  {
                     found = true;
                     numAnomEdge = anomEdgeLabel.size();
                  }
               }
               if (!found)
               {
                  for (unsigned int numGoodEdge=0; numGoodEdge < goodEdgeLabel.size(); numGoodEdge++)
                  {
                     if ((fromlabel == goodFromLabel[numGoodEdge]) &&
                         (tolabel == goodToLabel[numGoodEdge]) &&
                         (edgelabel == goodEdgeLabel[numGoodEdge]) &&
                         (tidNumber == goodTid[numGoodEdge]))
                     {
                        found = true;
                        numGoodEdge = goodEdgeLabel.size();
                     }
                  }
                  if (!found)
                  {
                     goodFromNodeid.push_back(fromnodeid);
                     goodFromLabel.push_back(fromlabel);
                     goodToNodeid.push_back(tonodeid);
                     goodToLabel.push_back(tolabel);
                     goodEdgeLabel.push_back(edgelabel);
                     goodTid.push_back(tidNumber);
                  }
               }
               numParentOccs = parent.elements.size();
            }
         }
         if (parent.parent != NULL)
            parent = * (parent.parent);
      }
   }

   // Third, determine which of these good edges are also available
   // in this anomalous transaction (i.e., are connected)
   for (unsigned int fromnodeid = 0; fromnodeid < database.trees[tidNumber]->nodes.size(); fromnodeid++)
   {
      fromlabel = database.nodelabels[database.trees[tidNumber]->nodes[fromnodeid].nodelabel].inputlabel;
      for (int numEdge = 0; numEdge < database.trees[tidNumber]->nodes[fromnodeid].edges.size(); numEdge++)
      {
         tonodeid = database.trees[tidNumber]->nodes[fromnodeid].edges[numEdge].tonode;
         tolabel = database.nodelabels[database.trees[tidNumber]->nodes[tonodeid].nodelabel].inputlabel;
         edgelabel = getEdgeLabelFromDatabaseTree(tidNumber,
                                                  fromnodeid,
                                                  tonodeid);
         for (unsigned int numAnomVertices=0; numAnomVertices < anomVertex.size(); numAnomVertices++)
         {
            if (((anomVertex[numAnomVertices] == fromnodeid) ||
                 (anomVertex[numAnomVertices] == tonodeid)) &&
                (anomVertexTid[numAnomVertices] == tidNumber))
            {
               for (unsigned int numGoodEdge=0; numGoodEdge < goodEdgeLabel.size(); numGoodEdge++)
               {
                  if ((fromlabel == goodFromLabel[numGoodEdge]) &&
                      (tolabel == goodToLabel[numGoodEdge]) &&
                      (edgelabel == goodEdgeLabel[numGoodEdge])&&
                      (tidNumber == anomVertexTid[numAnomVertices]))
                  {
                     found = false;
                     for (unsigned int n=0; n < anomVertex.size(); n++)
                     {
                        if (fromnodeid == anomVertex[n])
                        {
                           found = true;
                           n = anomVertex.size();
                        }
                     }
                     if (!found)
                     {
                        anomVertex.push_back(fromnodeid);
                        anomVertexLabel.push_back(fromlabel);
                        anomVertexTid.push_back(tidNumber);
                        sizeOfAnomInstance++;
                     }
                     found = false;
                     for (unsigned int n=0; n < anomVertex.size(); n++)
                     {
                        if (tonodeid == anomVertex[n])
                        {
                           found = true;
                           n = anomVertex.size();
                        }
                     }
                     if (!found)
                     {
                        anomVertex.push_back(tonodeid);
                        anomVertexLabel.push_back(tolabel);
                        anomVertexTid.push_back(tidNumber);
                        sizeOfAnomInstance++;
                     }
                     found = false;
                     for (unsigned int numAnomEdge=0; numAnomEdge < anomEdgeLabel.size(); numAnomEdge++)
                     {
                        if ((anomFrom[numAnomEdge] == fromnodeid) &&
                            (anomTo[numAnomEdge] == tonodeid) &&
                            (anomEdgeLabel[numAnomEdge] == edgelabel) &&
                            (anomEdgeTid[numAnomEdge] == tidNumber))
                           found = true;
                     }
                     if (!found)
                     {
                        anomFrom.push_back(fromnodeid);
                        anomFromLabel.push_back(fromlabel);
                        anomTo.push_back(tonodeid);
                        anomToLabel.push_back(tolabel);
                        anomEdgeLabel.push_back(edgelabel);
                        anomEdgeTid.push_back(tidNumber);
                        sizeOfAnomInstance++;
                     }
                  }
               }
            }
         }
      }
   }

   // If they are the same size, can't be anomalous (with this algorithm)
   if ((int)sizeOfAnomInstance >= bestSize)
   {
      return;
   }

   // Determine anomalous value of this instance
   double anomValue = 1.0 - ((double)sizeOfAnomInstance / (double)bestSize);

   // Is it within the user specified threshold?
   if (anomValue > matchThreshold)
   {
      return;
   }

   if (anomValue > bestAnomValue)
   {
      return;
   }
     
   else if (anomValue < bestAnomValue)
   {
      cout << "removing old " << anom_sub_name <<" file..." << endl;
      remove(anom_sub_name);   // better anomaly - get rid of any previous ones
      bestAnomValue = anomValue;
   }

   // And finally, output the vertices and edges for the anomalous
   // instance

   // open file for reporting anomalies
   fstream anom_sub_file;
   anom_sub_file.open (anom_sub_name, fstream::out | fstream::app);

   anom_sub_file << "% transaction containing anomalous structure: " 
                 << (tidNumber + 1) << endl;
   anom_sub_file << "% anomalous value: " << fixed << anomValue << endl;

   for (unsigned int i=0; i<anomVertex.size(); i++)
   {
      anomNodes[make_pair((int)tidNumber, anomVertex[i])] = (char*)("orange");
      
      anom_sub_file << "v " << anomVertex[i] << " " 
                    << anomVertexLabel[i] << endl;
   }
   for (unsigned int i=0; i<anomEdgeLabel.size(); i++)
   {
   	  anomEdges[make_pair((int)tidNumber, make_pair(anomFrom[i], anomTo[i]))] = (char*)("orange");
   	  
      anom_sub_file << "e " << anomFrom[i] << " " 
                    << anomTo[i] << " " << anomEdgeLabel[i] << endl;
   }

   anom_sub_file.close();
}


//******************************************************************************
// NAME:    getBestTidNumber
//
// INPUTS:  legoccurrences - all edges associated with a subgraph
//
// RETURN:  Tid
//
// PURPOSE:  This function returns the Tid number for the subgraph that has
//           the highest number of occurrences (and is probably best).
//
//******************************************************************************

Tid GBAD::getBestTidNumber(LegOccurrences &legoccurrences) 
{
   bool found;
   vector<Tid> uniqueTids;
   vector<unsigned int> uniqueTidCounts;
   // First, increment the count for each occurrence of a unique tid
   for (unsigned int numOccs=0; numOccs<legoccurrences.elements.size(); numOccs++)
   {
      found = false;
      for (unsigned int n=0; n < uniqueTids.size(); n++)
      {
         if (uniqueTids[n] == legoccurrences.elements[numOccs].tid)
         {
            uniqueTidCounts[n]++;
            found = true;
            n = uniqueTids.size();
         }
      }
      if (!found)
      {
         uniqueTids.push_back(legoccurrences.elements[numOccs].tid);
         uniqueTidCounts.push_back(1);
      }
   }
   // Then, see which one has the higest count (i.e., number of occurrences),
   // and that will be the best TID
   Tid bestTidNumber = 0;
   for (unsigned int n=0; n < uniqueTids.size(); n++)
   {
      if (uniqueTidCounts[n] > bestTidNumber)
         bestTidNumber = uniqueTids[n];
   }

   return bestTidNumber;
}


//******************************************************************************
// NAME:    relDiff
//
// INPUTS:  double - first number
//          double - second number
//
// RETURN:  difference between two numbers
//
// PURPOSE: Due to the inaccuracies associated with comparing real numbers,
//          this function returns the difference between two numbers.
//
//******************************************************************************
double GBAD::relDiff(double a, double b)
{
   double c = Abs(a);
   double d = Abs(b);

   d = Max(c, d);

   return d == 0.0 ? 0.0 : Abs(a - b) / d;
}
