//******************************************************************************
// legoccurrence.h
//
// GBAD-FSM, version 0.1
//
// Date      Name       Description
// ========  =========  ========================================================
// 09/28/10  Eberle     Initial version, taken from GASTON.
//                      Courtesy of Siegfried Nijssen,snijssen@liacs.nl, 01/2004
//
//******************************************************************************


#ifndef LEGOCCURRENCE_H
#define LEGOCCURRENCE_H

#include <iostream>
#include <vector>
#include "misc.h"

using namespace std;

typedef unsigned int OccurrenceId;

struct LegOccurrence 
{
   Tid tid;
   OccurrenceId occurrenceid;
   NodeId tonodeid, fromnodeid;

   LegOccurrence ( Tid tid, OccurrenceId occurrenceid, NodeId tonodeid, NodeId fromnodeid ):
      tid ( tid ), occurrenceid ( occurrenceid ), tonodeid ( tonodeid ), fromnodeid ( fromnodeid ) { }
   LegOccurrence () {}

   friend ostream &operator<< ( ostream &stream, LegOccurrence &occ );
};

struct LegOccurrences;
typedef LegOccurrences *LegOccurrencesPtr;

struct LegOccurrences 
{
   vector<LegOccurrence> elements;
   LegOccurrencesPtr parent;
   int number;                     // number of vertices
   short unsigned int maxdegree;
   Frequency frequency;
   Frequency selfjoin;
   LegOccurrences () : frequency ( 0 ), selfjoin ( 0 ) { }
};

ostream &operator<< ( ostream &stream, vector<LegOccurrence> &occs );

extern LegOccurrences legoccurrences;

// returns the join if this join is frequent. The returned array may be swapped.
LegOccurrencesPtr join ( LegOccurrences &legoccsdata1, NodeId connectingnode, LegOccurrences &legoccsdata2 );
LegOccurrencesPtr join ( LegOccurrences &legoccsdata );

extern vector<LegOccurrences> candidatelegsoccurrences; // for each frequent possible edge, the occurrences found, used by extend
extern vector<Frequency> candidatelegsfrequencies;

void initLegStatics ();

void extend ( LegOccurrences &legoccurrencesdata ); // fills the global arrays above
void extend ( LegOccurrences &legoccurrencesdata, EdgeLabel minlabel, EdgeLabel neglect );

void sanityCheck ( LegOccurrencesPtr legoccurrencesptr );
void clearLegOccs();

#endif
