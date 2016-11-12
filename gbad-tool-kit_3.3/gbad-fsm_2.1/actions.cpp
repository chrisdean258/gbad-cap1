#include <iostream>

#include "Label.h"

#include "actions.h"
#include "y.tab.h"

extern "C"
{
	#include "lex.yy.h"
}

int yyparse(void *arg);
void yyerror(void *arg, const char *str);

char *GP_file_name = NULL;

#define ERR_STR_LEN 64

char GP_err_str[ERR_STR_LEN];

void GP_read_graph(FILE *input, Graph_Info *GP_info)
{
	GP_file_name = (char *)"";
	FILE *tmp = yyin;
	yyin = input;
	yyparse(GP_info);
	yyin = tmp;
}

void GP_add_xp(void *arg, int num)
{
	//printf("XP # %d\n", num);
	
	Graph_Info *GP_info = (Graph_Info *)arg;
	
	if(num != (int)GP_info->vertices.size()+1)
	{
		snprintf(GP_err_str, ERR_STR_LEN, "invalid XP number, found %d, expecting %lu",
			num, (long unsigned int)GP_info->vertices.size()+1);
		yyerror(NULL, GP_err_str);
		exit(1);
	}
	
	GP_info->vertices.push_back(vector< pair<unsigned int, int> >());
	GP_info->edges.push_back(vector< pair< pair<unsigned int, unsigned int>, int> >());
}

void GP_add_ps(void *arg, int num)
{
	//printf("PS # %d\n", num);
	
	yyerror(NULL, "unsupported graph type, found PS, expecting XP.");
	exit(1);
}

void GP_add_vertex_i(void *arg, int v, int label)
{
	int labelIndex;
	Graph_Info *GP_info = (Graph_Info *)arg;
	
	if (v != (int)(GP_info->vertices.back().size() + 1))
	{
		snprintf(GP_err_str, ERR_STR_LEN, "invalid vertex number, found %d, expecting %lu",
			v, (long unsigned int)(GP_info->vertices.back().size() + 1));
		yyerror(NULL, GP_err_str);
		exit(1);
	}
	
	labelIndex = GP_info->labelList->addLabel(label);
	GP_info->vertices.back().push_back( make_pair((unsigned int)v, labelIndex) );
}

void GP_add_vertex_f(void *arg, int v, double label)
{
	int labelIndex;
	Graph_Info *GP_info = (Graph_Info *)arg;
	
	if (v != (int)(GP_info->vertices.back().size() + 1))
	{
		snprintf(GP_err_str, ERR_STR_LEN, "invalid vertex number, found %d, expecting %lu",
			v, (long unsigned int)(GP_info->vertices.back().size() + 1));
		yyerror(NULL, GP_err_str);
		exit(1);
	}
	
	labelIndex = GP_info->labelList->addLabel(label);
	GP_info->vertices.back().push_back( make_pair((unsigned int)v, labelIndex) );
}

void GP_add_vertex_s(void *arg, int v, char *label)
{
	int labelIndex;
	Graph_Info *GP_info = (Graph_Info *)arg;
	
	if (v != (int)(GP_info->vertices.back().size() + 1))
	{
		snprintf(GP_err_str, ERR_STR_LEN, "invalid vertex number, found %d, expecting %lu",
			v, (long unsigned int)(GP_info->vertices.back().size() + 1));
		yyerror(NULL, GP_err_str);
		exit(1);
	}
	
	labelIndex = GP_info->labelList->addLabel(label);
	GP_info->vertices.back().push_back( make_pair((unsigned int)v, labelIndex) );

}

void GP_add_edge_i(void *arg, int type, int src, int dst, int label)
{
	int labelIndex;
	Graph_Info *GP_info = (Graph_Info *)arg;
	
	if (src > (int)GP_info->vertices.back().size()+1) 
	{
		snprintf(GP_err_str, ERR_STR_LEN, "undefined source vertex number, found %d",
			src);
		yyerror(NULL, GP_err_str);
		exit(1);
	}
	
	if (dst > (int)GP_info->vertices.back().size()+1) 
	{
		snprintf(GP_err_str, ERR_STR_LEN, "undefined target vertex number, found %d",
			dst);
		yyerror(NULL, GP_err_str);
		exit(1);
	}
	
	if(type == D_EDGE)
	{
		yyerror(NULL, "unsupported edge type, found directed edge, converting to undirected edge");
		exit(1);
	}
	
	labelIndex = GP_info->labelList->addLabel(label);
	GP_info->edges.back().push_back(make_pair(make_pair(src, dst), labelIndex));
}

void GP_add_edge_f(void *arg, int type, int src, int dst, double label)
{
	int labelIndex;
	Graph_Info *GP_info = (Graph_Info *)arg;
	
	if (src > (int)GP_info->vertices.back().size()+1) 
	{
		snprintf(GP_err_str, ERR_STR_LEN, "undefined source vertex number, found %d",
			src);
		yyerror(NULL, GP_err_str);
		exit(1);
	}
	
	if (dst > (int)GP_info->vertices.back().size()+1) 
	{
		snprintf(GP_err_str, ERR_STR_LEN, "undefined target vertex number, found %d",
			src);
		yyerror(NULL, GP_err_str);
		exit(1);
	}
	
	if(type == D_EDGE)
	{
		yyerror(NULL, "unsupported edge type, found directed edge, converting to undirected edge");
		exit(1);
	}
	
	labelIndex = GP_info->labelList->addLabel(label);
	GP_info->edges.back().push_back(make_pair(make_pair(src, dst), labelIndex));
}

void GP_add_edge_s(void *arg, int type, int src, int dst, char *label)
{
	int labelIndex;
	Graph_Info *GP_info = (Graph_Info *)arg;
	
	if (src > (int)GP_info->vertices.back().size()+1) 
	{
		snprintf(GP_err_str, ERR_STR_LEN, "undefined source vertex number, found %d",
			src);
		yyerror(NULL, GP_err_str);
		exit(1);
	}
	
	if (dst > (int)GP_info->vertices.back().size()+1) 
	{
		snprintf(GP_err_str, ERR_STR_LEN, "undefined target vertex number, found %d",
			src);
		yyerror(NULL, GP_err_str);
		exit(1);
	}
	
	if(type == D_EDGE)
	{
		yyerror(NULL, "unsupported edge type, found directed edge, converting to undirected edge");
		exit(1);
	}
	
	labelIndex = GP_info->labelList->addLabel(label);
	GP_info->edges.back().push_back(make_pair(make_pair(src, dst), labelIndex));
}
