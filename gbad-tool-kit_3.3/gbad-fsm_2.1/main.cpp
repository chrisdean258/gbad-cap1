//******************************************************************************
// main.cpp
//
// GBAD-FSM, version 1.0
//
// Date      Name        Description
// ========  =========   ========================================================
// 09/28/10  Eberle      Initial version, taken from GASTON.
//                       Courtesy of Siegfried Nijssen,snijssen@liacs.nl, 01/2004
// 03/04/11  Eberle      Updated version number.
// 04/28/11  Massengill  Removed need to specify /dev/null.
//                       Options changed from single letters to (-mps, -mdl, -prob).
// 05/16/11  Massengill  Added "-mst" before mst value in input.
//                       argv index changed from "optind" to regular int value so
//                       the program can compile in windows.
//                       Removed -m,-t,-p input options.
// 05/23/11  Massengill  Consolidated the two phases into one phase.
// 06/08/11  Eberle      Updated version number to 1.0, fixed line spacing, and
//                       reworded parameter-help verbage.
// 06/23/11  Massengill  Options added to rename any output files.
//
//******************************************************************************

#include <iostream>
#include <fstream>
#include "database.h"
#include "path.h"
#include "misc.h"
#include "graphstate.h"
#include "legoccurrence.h"
#include "gbad.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <sstream>

using namespace std;

Frequency minfreq = 1;
double matchThreshold = 0.0;
bool mdlOption = false;
bool mpsOption = false;
bool probOption = false;
bool prob = false;
Database database;
Statistics statistics;
bool dooutput = false;
int phase = 3;
unsigned int maxsize = MAX_PATTERN_SIZE;    // safe default for the largest allowed pattern
int bestSize = 0;
int bestSizeTimesFreq = 0;
unsigned int bestNumNodes = 0;
int numBestInstances = 0;
unsigned int numberOfTids = 0;
double bestAnomValue = 1.0;
FILE *output;
bool saveBestSubInst;
bool phase1=false;
bool phase2=false;
const char* best_sub_g_name = "best_sub.g";
const char* best_sub_inst_name = "best_sub.inst";
const char* anom_sub_name = "anom_sub.g";
char* dot_file_name = NULL;


//******************************************************************************
// NAME:    print
//
// INPUTS:  none
//
// RETURN:  none
//
// PURPOSE: The following function prints information about the frequent
//          cycles, graphs and trees.
//******************************************************************************

void Statistics::print () 
{
   int total = 0, total2 = 0, total3 = 0;
   for ( unsigned int i = 0; i < frequenttreenumbers.size (); i++ ) 
   {
      cout << "Frequent " << i + 2
           << " cyclic graphs: " << frequentgraphnumbers[i]
           << " real trees: " << frequenttreenumbers[i]
           << " paths: " << frequentpathnumbers[i]
           << " total: " << frequentgraphnumbers[i] + frequenttreenumbers[i] + frequentpathnumbers[i] << endl;
      total += frequentgraphnumbers[i];
      total2 += frequenttreenumbers[i];
      total3 += frequentpathnumbers[i];
   }
   cout << "TOTAL:" << endl
        << "Frequent cyclic graphs: " << total << " real trees: " << total2 << " paths: " << total3 << " total: " << total + total2 + total3 << endl;
}


//******************************************************************************
// NAME:    puti
//
// INPUTS:  (File *f) - name of file to be written to
//          (int i) - integer value
//
// RETURN:  none
//
// PURPOSE: This function prints the string representation of a number to the
//          specified file.
//******************************************************************************

void puti ( FILE *f, int i ) 
{
   char array[100];
   int k = 0;
   do {
      array[k] = ( i % 10 ) + '0';
      i /= 10;
      k++;
   } while ( i != 0 );
   do 
   {
      k--;
      putc ( array[k], f );
   } while ( k );
}


//******************************************************************************
// NAME:    main
//
// INPUTS:  (int argc) - number of arguments to program
//          (char **argv) - array of strings of arguments to program
//
// RETURN:  (int) - 0 if all is well
//
// PURPOSE: Main GBAD-FSM function that processes command-line arguments
//          and either (1) initiates discovery of the normative pattern, or 
//          (2) initiates discovery of potential anomalous patterns.
//
// NOTE:  During the discovery phase (of the normative pattern), this version
//        requires that an output file (of instances) must be specified.... 
//        I want to remove that from being mandatory, because the instances are
//        not needed for anomaly detection.
//
//******************************************************************************

void resetVars()
{
   minfreq = 1;
   matchThreshold = 0.0;
   dooutput = false;
   phase = 3;
   maxsize = MAX_PATTERN_SIZE;    // safe default for the largest allowed pattern
   bestSize = 0;
   bestSizeTimesFreq = 0;
   bestNumNodes = 0;
   numBestInstances = 0;
   numberOfTids = 0;
   bestAnomValue = 1.0;
   Database d;
   database=d;
   clearLegOccs();
   graphstate.clearGraphState();
}

int runNum = 0;

//******************************************************************************
// Runs either phase. The arguments must be passed in phase I or phase II format.
//******************************************************************************
int run ( int argc, char** argv) 
{
   int    i;
   double argumentExists;
   double doubleArg;
   // start clock for performance timings
   clock_t t1 = clock ();

   int optind=1;
   // process command-line options
   i = 1;
   while (i < (argc - 2))// || (prob && (i < (argc - 1)) && phase2))
   {
      // Debug-only
      cout << "argv[" << i << "] = " << argv[i] << endl;
     
      if (strcmp(argv[i], "-mdl") == 0)
      {
         i++;
         phase = 4;
         argumentExists = sscanf(argv[i], "%lf", &doubleArg);
         if ((argumentExists != 1) || (doubleArg <= (double) 0.0) ||
             (doubleArg >= (double) 1.0))
         {
            fprintf(stderr, "%s: GBAD-MDL threshold must be greater than 0.0 and less than 1.0\n", argv[0]);
            return 1;
         }
         else
            cout << "GBAD-MDL option chosen." << endl;
         matchThreshold = doubleArg;
         i++;
         optind = optind + 2;
         mdlOption = true;

         // Debug-only
         cout << "   matchThreshold (MDL) = " << matchThreshold << endl;
      }
      else if (strcmp(argv[i], "-mps") == 0)
      {
         i++;
         phase = 4;
         argumentExists = sscanf(argv[i], "%lf", &doubleArg);
         if ((argumentExists != 1) || (doubleArg <= (double) 0.0) ||
             (doubleArg >= (double) 1.0))
         {
            fprintf(stderr, "%s: GBAD-MPS threshold must be greater than 0.0 and less than 1.0\n", argv[0]);
            return 1;
         }
         else
            cout << "GBAD-MPS option chosen." << endl;
         matchThreshold = doubleArg;
         i++;
         optind = optind + 2;
         mpsOption = true;

         // Debug-only
         cout << "   matchThreshold (MPS) = " << matchThreshold << endl;
      }
      else if (strcmp(argv[i], "-prob") == 0)
      {
         i++;
         phase = 4;
         cout << "GBAD-P option chosen." << endl;
         optind = optind + 1;
         probOption = true;
      }
      else
      {
         i++;
      }
   }

   if (strcmp(argv[optind], "-mst") == 0)
   {
      optind = optind + 1;
      minfreq = atoi ( argv[optind] );
      
      if(minfreq <= 0)
      {
         cerr << "ERROR:  invalid mst value (" << minfreq << "), mst must be greater than 0" << endl;
         return 1;
      }

      // Debug-only
      cout << "minfreq = " << minfreq << endl;
   }

   // open input file (graph) for reading
   cerr << "Read" << endl;

   // Debug-only
   cout << "input graph file: " << argv[optind+1] << endl;

   FILE *input = fopen ( argv[optind+1], "r" );
   // If unable to open input file, need to give an error message and abort
   if (input == NULL)
   {
      fprintf(stderr, "ERROR:  Unable to open specified input file: %s\n", argv[optind+1]);
      return 1;
   }
   
   if ( argc - optind == 3 && argc > 4) 
   {
      dooutput = true;
      // Debug-only
      cout << "instances file: " << argv[optind+2] << endl;
      output = fopen ( argv[optind+2], "w" );
   }
   else
   {
      cout << "No instances file specified" << endl;
   }

   database.read ( input );
   rewind(input);
   Database testDatabase;
   testDatabase.read(input);
   fclose ( input );

   cerr << "Edgecount" << endl;
   database.edgecount ();

   cerr << "Reorder" << endl;
   database.reorder ();

   initLegStatics ();
   graphstate.init ();

   if ((mdlOption) || (mpsOption) || (probOption))
      cout << "Searching for anomalies..." << endl;

   // If it is the "searching for anomalies" phase, and the GBAD-P option was
   // chosen (-r), then no need to examine the graph again - just use
   // the database tree and best_sub.inst info to find the most anomalous
   // extra edge
   if ((phase == 4) && (probOption))
   {
      gbad.gbadPROB();
   }
   // Else, use the FSM algorithm to find the normative pattern...
   else
   {
      for ( unsigned int i = 0; i < database.nodelabels.size (); i++ ) 
      {
         if (((database.nodelabels[i].frequency >= minfreq) &&
              (database.nodelabels[i].frequentedgelabels.size())) ||
             ((database.nodelabels[i].frequency < minfreq) &&
              (database.nodelabels[i].frequency > 0) &&
              (phase == 4) && (mdlOption)))
         {
            Path path ( i );
   
            // Debug-only
            cout << "   Processing path(" << i << "): " << path << "..." << endl;
   
            path.expand ();
         }
      }
   }
   //
   // If this is an anomaly detection phase, print out the best anomalous
   // value (if there is one).
   //
   if (phase == 4)
   {
      if ((bestAnomValue <= 1.0) && (bestAnomValue > 0.000000))
         cout << "Anomalous Value: " << fixed << bestAnomValue << endl;
      else
         cout << "Anomalous Value: None" << endl;
   }
   clock_t t2 = clock ();

   statistics.print ();
   cout << "Approximate total runtime: " << ( (float) t2 - t1 ) / CLOCKS_PER_SEC << "s" << endl;

   if (dooutput)
      fclose ( output );
   
   if(dot_file_name != NULL)
   {
      string dotFile(dot_file_name);
      testDatabase.toDot(dotFile, gbad.anomNodes, gbad.anomEdges, gbad.normNodes, gbad.normEdges);
   }
   
   return 0;
}

void print_parameters(int argc, char *argv[])
{
   int ii;
   char *graph = NULL;
   char *mst = NULL;
   char *phase = (char *)"";
   char *subInstOut = NULL;
   char *bestSubG = (char *)"best_sub.g";
   char *bestSubInstOut = NULL;
   char *nameAnomSub = (char *)"anom_sub.g";
   char *detection = (char *)"NONE";
   char *dot = NULL;
   char detectionParam[256];
   
   detectionParam[0] = '\0';
   
   
   cout << "GBAD-FSM, Graph-Based Anomaly Detection using a Frequent Subgraph Miner" << endl;
   cout << "Version 2.1" << endl;
   for(ii=1; ii < argc; ii++)
   {
      if(strcmp(argv[ii], "-graph") == 0)
      {
         ii++;
         if(argc > ii)
         {
            graph = argv[ii];
         }
      }
      else if(strcmp(argv[ii], "-mst") == 0)
      {
         ii++;
         if(argc > ii)
         {
            mst = argv[ii];
         }
      }
      else if(strcmp(argv[ii], "-phase") == 0)
      {
         ii++;
         if(argc > ii)
         {
            phase = argv[ii];
         }
      }
      else if(strcmp(argv[ii], "-subInstOut") == 0)
      {
         ii++;
         if(argc > ii)
         {
            subInstOut = argv[ii];
         }
      }
      
      else if(strcmp(argv[ii], "-nameBestSubG") == 0)
      {
         ii++;
         if(argc > ii)
         {
            bestSubG = argv[ii];
         }
      }
      else if(strcmp(argv[ii], "-bestSubInstOut") == 0)
      {
         if(bestSubInstOut != NULL)
         {
            bestSubInstOut = (char *)"best_sub.inst";
         }
      }
      else if(strcmp(argv[ii], "-nameBestSubINST") == 0)
      {
         ii++;
         if(argc > ii)
         {
            bestSubInstOut = argv[ii];
         }
      }
      else if(strcmp(argv[ii], "-nameAnomSub") == 0)
      {
         ii++;
         if(argc > ii)
         {
            nameAnomSub = argv[ii];
         }
      }
      else if(strcmp(argv[ii], "-mps") == 0)
      {
         ii++;
         if(argc > ii)
         {
            detection = (char *)"Maximum Partial";
            sprintf(detectionParam, "  Maximum Partial Sub threshold.. %f\n", atof(argv[ii]));
         }
      }
      else if(strcmp(argv[ii], "-mdl") == 0)
      {
         ii++;
         if(argc > ii)
         {
			 detection = (char *)"Information Theoretic";
			 sprintf(detectionParam, "Information Theoretic threshold.. %f\n", atof(argv[ii]));
		 }
      }
      else if(strcmp(argv[ii], "-prob") == 0)
      {
         ii++;
         if(argc > ii)
         {
			detection = (char *)"Probabilistic";
			 sprintf(detectionParam, "  Probabilistic Iterations....... %i\n", atoi(argv[ii]));
		 }
      }
      else if(strcmp(argv[ii], "-dot") == 0)
      {
         ii++;
         if(argc > ii)
         {
         	dot = argv[ii];
         }
      }
   }
   
   printf("Parameters:\n");
   
   if(graph != NULL)
   {
      printf("  Input file..................... %s\n", graph);
   }
   
   if(dot != NULL)
   {
      printf("  Dot file....................... %s\n", dot);
   }
   
   if(mst != NULL)
   {
      printf("  Minimum Support Threshold...... %s\n", mst);
   }
   
   if(strcmp(phase, "") != 0) 
   {
      printf("  Phase.......................... %s\n", phase);
   }
   
   if(detection != NULL)
   {
      printf("  Anomaly Detection method....... %s\n", detection);
      printf("%s", detectionParam);
   }
   
   if(subInstOut != NULL)
   {
      printf("  Substructure Instances File.... %s\n", subInstOut);
   }
   
   if(bestSubG != NULL)
   {
      printf("  Best Substructure File......... %s\n", bestSubG);
   }
   
   if(bestSubInstOut != NULL)
   {
      printf("  Best Substructure Instances.... %s\n", bestSubInstOut);
   }
   
   if(nameAnomSub != NULL)
   {
      printf("  Anomalous Substructure File.... %s\n", nameAnomSub);
   }
   
   printf("\n\n");
}

//***********************************************************************************
// Rearranges the input into phase I and/or phase II format and calls the run function.
//***********************************************************************************
int main ( int argc, char** argv ) 
{
   print_parameters(argc, argv);
   
   /*
   cout << "GBAD-FSM, Graph-Based Anomaly Detection using a Frequent Subgraph Miner" << endl;
   cout << "Version 1.0" << endl;
   */
   cout << "maxsize (default) = " << maxsize << endl;
   cout << "Reading options..." << endl;
   cout << "argc = " << argc << endl;
   
   char* phaseIargs[8];
   char* phaseIIargs[7];
   saveBestSubInst=false;
   int algorithmIndex=-1;
   //bool prob=false;
   bool mdl=false;
   int mstIndex=-1;
   int graphIndex=-1;
   int subInstOutIndex=-1;
   int bestSubInstOutIndex=-1;
   int phaseNumIndex=-1;
   int i=1;
   while (i<argc)
   {
      if (strcmp(argv[i], "-mps") == 0)
      {
         algorithmIndex=i;
      }
      if (strcmp(argv[i], "-mdl") == 0)
      {
         algorithmIndex=i;
         mdl=true;
      }
      if (strcmp(argv[i], "-prob") == 0)
      {
         algorithmIndex=i;
         prob=true;
      }
      if (strcmp(argv[i], "-mst") == 0)
      {
         mstIndex=i;
      }
      if (strcmp(argv[i], "-graph") == 0)
      {
         graphIndex=i;
      }
      if (strcmp(argv[i], "-subInstOut") == 0)
      {
         subInstOutIndex=i;
      }
      if (strcmp(argv[i], "-bestSubInstOut") == 0)
      {
         bestSubInstOutIndex=i;
         saveBestSubInst=true;
      }
      if (strcmp(argv[i], "-phase") == 0)
      {
         phaseNumIndex=i;
      }
      if (strcmp(argv[i], "-nameBestSubG") == 0)
      {
         best_sub_g_name = argv[i+1];
         cout << "Best substructure info will be saved to: " << best_sub_g_name << endl;
         i++;
      }
      if (strcmp(argv[i], "-nameBestSubINST") == 0)
      {
         best_sub_inst_name = argv[i+1];
         cout << "Best substructure instance info will be saved to: " << best_sub_inst_name << endl;
         i++;
      }
      if (strcmp(argv[i], "-nameAnomSub") == 0)
      {
         anom_sub_name = argv[i+1];
         cout << "Anomolous substructures info will be saved to: " << anom_sub_name << endl;
         i++;
      }
      if (strcmp(argv[i], "-dot") == 0)
      {
         dot_file_name = argv[i+1];
         i++;
      }
      i++;
   }
    
   phaseIargs[0]=argv[0];//both phases begin with the executable command
   phaseIIargs[0]=argv[0];
    
   int p1Index=1;
   int p2Index=1;
    
   if (algorithmIndex>-1)//add the gbad algorithm and its threshold to the phase 2 parameters
   {
      phaseIIargs[p2Index]=argv[algorithmIndex];
      p2Index++;
      if(!prob)//the -prob option should not have a threshold
      {
         phaseIIargs[p2Index]=argv[algorithmIndex+1];
         p2Index++;
      }
   }
   if (mstIndex>-1)//add the mst option and its threshold to both phases' parameters
   {
      phaseIargs[p1Index]=argv[mstIndex];
      p1Index++;
      phaseIargs[p1Index]=argv[mstIndex+1];
      p1Index++;
      phaseIIargs[p2Index]=argv[mstIndex];
      p2Index++;
      if (mdl)//the -mdl option needs an mst threshold of 1
      {
        char one[]={'1'};
        phaseIIargs[p2Index]=one;
        p2Index++;
      }
      else
      {
        phaseIIargs[p2Index]=argv[mstIndex+1];
        p2Index++;
      }
   }
   if (graphIndex>-1)//add the graph location to both phases' parameters
   {
      phaseIargs[p1Index]=argv[graphIndex+1];
      p1Index++;
      phaseIIargs[p2Index]=argv[graphIndex+1];
      p2Index++;
   }
   if (subInstOutIndex>-1)//determine whether to output all substructure instances
   {
      phaseIargs[p1Index]=argv[subInstOutIndex+1];
      p1Index++;
   }
   if (bestSubInstOutIndex>-1)//determine whether to output best_sub.inst
   {
      saveBestSubInst=true;
   }
   
   //determine which phase to run
   if (phaseNumIndex>-1)
   {
      int phaseNum=atoi(argv[phaseNumIndex+1]);
      if (phaseNum<1||phaseNum>2)
      {
         cerr << "Invalid phase option: " << phaseNum << endl;
         return 1;
      }
        
      //run phase I
      if (phaseNum==1)
      {
         if (p1Index < 4 || p1Index > 8) 
        {
        cerr << "Phase I Parameters: "<<endl;
        cerr << "gbad-fsm [-mst <mst value>] [-graph <graph input file>] -phase 1"<<endl;
        cerr << "options:    -subInstOut <output>  (output the substructure instances)"<<endl;
        cerr << "            -bestSubInstOut  (write the best substructure instances to the best_sub.inst file)"<<endl;
        return 1;
        }
        else
        {
           phase1=true;
           cout << "Running phase I." << endl;
           run(p1Index,phaseIargs);
        }
      }
      //run phase II
      else if (phaseNum==2)
      {
         saveBestSubInst=true;
         if (p2Index < 4 || p2Index > 6) 
         {
            cerr << "Phase II Parameters:"<<endl;
            cerr << "gbad-fsm [<gbad algorithm> <gbad threshold>] [-mst <mst value>] [-graph <graph input file>] -phase 2"<<endl;
            cerr << "options:    -subInstOut <output>  (output the substructure instances)"<<endl;
            cerr << "            -bestSubInstOut  (write the best substructure instances to the best_sub.inst file)"<<endl;
            cerr << "            -mdl  (the MDL algorithm)" << endl;
            cerr << "            -mps  (the MPS algorithm)" << endl;
            cerr << "            -prob (the Prob algorithm)" << endl;
            return 1;
         }
         else
         {
            phase2=true;
            cout << "Running phase II." << endl;
            run(p2Index,phaseIIargs);
         }
      }
   }
   //run both phases
   else
   {
      // read user-provided options
      if (argc < 6) 
      {
        cerr << "GBAD Parameters:"<<endl;
        cerr << "gbad-fsm [<gbad algorithm> <gbad threshold>] [-mst <mst value>] [-graph <graph input file>] "<<endl;
        cerr << "options:    -subInstOut <output>  (output the substructure instances)"<<endl;
        cerr << "            -bestSubInstOut  (write the best substructure instances to the best_sub.inst file)"<<endl;
        cerr << "            -phase <number>  (run phase 1 or 2 separately)"<<endl;
        cerr << "            -mdl  (the MDL algorithm)" << endl;
        cerr << "            -mps  (the MPS algorithm)" << endl;
        cerr << "            -prob (the Prob algorithm)" << endl;
        return 1;
      }
      else
      {
        cout << "Running both phases." << endl;
        phase1=true;
        run(p1Index, phaseIargs);
        resetVars();
        phase1=false;
        phase2=true;
        run(p2Index, phaseIIargs);
      }
   }
   
   return 0;
}
