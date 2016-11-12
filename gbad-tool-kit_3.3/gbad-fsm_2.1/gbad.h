//******************************************************************************
// gbad.h
//
// GBAD-FSM, version 1.0
//
// Date      Name       Description
// ========  =========  ========================================================
// 09/28/10  Eberle     Initial version.
// 03/04/11  Eberle     Add method to return num of instances (getNumInstances).
// 04/28/11  Eberle     Changed name of method saveAnomSub() to 
//                      outputAnomalousModifications().
//
//******************************************************************************

#ifndef GBAD_H
#define GBAD_H

#include "misc.h"
#include <vector>
#include <iostream>
#include "database.h"
#include "closeleg.h"
#include "patterntree.h"
using namespace std;

// global variables
extern int bestSize;            // size of best substructure (so far)
extern int bestSizeTimesFreq;
extern unsigned int bestNumNodes;  // size of best substructure (so far)
extern int numBestInstances;    // number of instances of the best sub
extern double matchThreshold;   // matching threshold
extern bool mdlOption;          // flag indicating GBAD-MDL algorithm chosen
extern bool mpsOption;          // flag indicating GBAD-MPS algorithm chosen
extern bool probOption;         // flag indicating GBAD-P algorithm chosen
extern double bestAnomValue;    // lowest (best) anomalous value found

// debug-only
extern int counter;

class GBAD;

extern GBAD gbad;

class GBAD 
{
   public:
      // global structures
      vector<Tid> bestTids;
      vector<NodeId> bestFromnodeids;
      vector<NodeId> bestTonodeids;
      vector<EdgeLabel> bestEdgelabels;
      vector<OccurrenceId> bestOccurrenceids;
      vector<NodeId> anomalousNodes;
      vector<NodeLabel> bestNodeLabels;
      vector<Tid> anomalousTIDs;
      vector<OccurrenceId> anomalousOccurrenceIds;
      struct Substructure {
         vector<int> vertexLabel;
         vector<int> fromNode;
         vector<int> toNode;
         vector<EdgeLabel> edgeLabel;
      };
      struct InstanceSubstructure {
         vector<NodeId> fromNode;
         vector<NodeId> toNode;
      };
      
      // GBAD function prototype
      bool graphMatch(double *);
      void findBestSub(LegOccurrences &);
      void saveBestSub(LegOccurrences &);

      void readBestSub();
      bool overlapsBestSubInstances(LegOccurrences &);

      InputEdgeLabel getEdgeLabelFromDatabaseTree(Tid, NodeId, NodeId);

      void gbadMDL (LegOccurrences &);
      void gbadMPS (LegOccurrences &);
      void gbadPROB ();
      void outputAnomalousModifications(LegOccurrences &, bool, double);
      void outputAnomalousInstance(NodeId, NodeId, EdgeLabel, Tid, double);
      void outputAnomalousInstanceWithMissingInfo(NodeId, NodeId, EdgeLabel, 
                                                  Tid, LegOccurrences &);
      Tid getBestTidNumber(LegOccurrences &);
      double relDiff(double, double);
      int getNumInstances(LegOccurrences &);
      
      map<pair<int,int>, char *> anomNodes;
      map< pair<int, pair<int,int> >, char *> anomEdges;
      
      map<pair<int,int>, char *> normNodes;
      map< pair<int, pair<int,int> >, char *> normEdges;
};

#endif
