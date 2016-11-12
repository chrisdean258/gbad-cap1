#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

#ifndef MAX_STR
#define MAX_STR 1024
#endif

void format_label(char *label, char *buffer)
{
	int ii;
	int jj = strlen(label);
	char *endptr;
	
	for(ii=0; ii < jj; ii++)
	{
		if(label[ii] != ' ' || label[ii] != '\t' || label[ii] != '\r' || label[ii] != '\n')
		{
			break;
		}
	}
	
	
	for(jj--; jj > ii; jj--)
	{
		if(label[jj] != ' ' || label[jj] != '\t' || label[jj] != '\r' || label[jj] != '\n')
		{
			break;
		}
	}
	
	label[jj+1] = '\0';
	label = &(label[ii]);
	
	strtod(label, &endptr);
	
	if(endptr[0] != '\0')
	{
		if((label[0] == '"') && (label[jj-ii] == '"'))
		{
			sprintf(buffer, "%s", label);
		}
		else
		{
			sprintf(buffer, "\"%s\"", label);
		}
	}
	else
	{
		sprintf(buffer, "%s", label);
	}
}

void convert(FILE *in, FILE *out, char *line, char *buff)
{
	char *tmp;
	char *token;
	char *v1;
	char *v2;
	char *label;
	off_t pos;
	unsigned int xp = 1;
	
	while(!feof(in))
	{
		pos = ftell(in);
		if(fgets(line, MAX_STR, in) != NULL)
		{
			tmp = line;
			
			token = strtok_r(tmp, " \t\r\n", &tmp);
			if(token != NULL)
			{
				if(strcasecmp(token, "XP") == 0)
				{
					fprintf(out, "XP # %u\n", xp++);
				}
				else if(strcasecmp(token, "v") == 0)
				{
					if(xp == 1)
					{
						fprintf(out, "XP # %u\n", xp++);
					}
					
					v1 = strtok_r(NULL, " \t\r\n", &tmp);
					label = strtok_r(NULL, "\r\n", &tmp);
					
					format_label(label, buff);
					
					fprintf(out, "v %s %s\n", v1, buff);
				}
				else if(!strcasecmp(token, "e") || !strcasecmp(token, "d") || !strcasecmp(token, "u"))
				{
					if(xp == 1)
					{
						fprintf(out, "XP # %u\n", xp++);
					}
					
					v1 = strtok_r(NULL, " \t\r\n", &tmp);
					v2 = strtok_r(NULL, " \t\r\n", &tmp);
					label = strtok_r(NULL, "\r\n", &tmp);
					
					printf("LABEL (%s)\n", label);
					format_label(label, buff);
					printf("BUFF (%s)\n", buff);
					
					fprintf(out, "%s %s %s %s\n", token, v1, v2, buff);
				}
				else
				{
					if(fseek(in, pos, SEEK_SET))
					{
						perror("seek");
						break;
					}
					
					if(fgets(line, MAX_STR, in) == NULL)
					{
						perror("fgets");
						break;
					}
					
					fprintf(out, "// %s", line);
				}
			}
		}
	}
}

void usage(char *exe_name)
{
	printf("Usage: %s <input_files>\n", exe_name);
	printf("  This program converts the given input files assumed to be\n");
	printf("  in the subdue graph format to the new gbad-mdl / gbad-fsm\n");
	printf("  graph format.  The converted graphs are written to\n");
	printf("  <input>.g.  For example, running the following command\n");
	printf("  would convert the file graph.g and write the result in a\n");
	printf("  a file called graph.g.g:\n");
	printf("    %s graph.g\n", exe_name);

}

int main(int argc, char *argv[])
{
	int ii;
	char line[MAX_STR];
	char buff[MAX_STR];
	
	FILE *input;
	FILE *output;
	
	if(argc < 2)
	{
		usage(argv[0]);
	}
	else if(strcasecmp(argv[1], "--help") == 0 || strcasecmp(argv[1], "-help") == 0)
	{
		usage(argv[0]);
	}
	else
	{
		for(ii=1; ii < argc; ii++)
		{
			input = fopen(argv[ii], "r");
			if(input == NULL)
			{
				perror(argv[ii]);
				continue;
			}
			
			snprintf(line, MAX_STR, "%s.g", argv[ii]);
			output = fopen(line, "w");
			if(output == NULL)
			{
				perror(line);
				
				fclose(input);
				continue;
			}
			
			convert(input, output, line, buff);
		}
	}
	
	return 0;
}
