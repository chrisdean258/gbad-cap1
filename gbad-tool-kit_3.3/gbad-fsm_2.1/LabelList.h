#include <iostream>
#include <map>

#include "Label.h"

using namespace std;

class LabelList
{
	protected:
		map<Label,int> labelMap;
		map<int,Label> indexMap;
	
	public:
		LabelList()
		{
			
		};
		
		~LabelList()
		{
			
		};
		
		int addLabel(int numericLabel)
		{
			Label label(numericLabel);
			return addLabel(label);
		};
		
		int addLabel(double numericLabel)
		{
			Label label(numericLabel);
			return addLabel(label);
		};
		
		int addLabel(char *stringLabel)
		{
			Label label(stringLabel);
			return addLabel(label);
		};
		
		int addLabel(string &stringLabel)
		{
			Label label(stringLabel);
			return addLabel(label);
		};
		
		int addLabel(Label label)
		{
			map<Label, int>::iterator iter = labelMap.find(label);
			
			if(iter == labelMap.end())
			{
				int index = labelMap.size();
				
				labelMap[label] = index;
				indexMap[index] = label;
			}
			
			return labelMap[label];
		};
		
		const Label *getLabel(int index)
		{
			map<int,Label>::iterator iter = indexMap.find(index);
			
			if(iter == indexMap.end())
			{
				return NULL;
			}
			else
			{
				return &(iter->second);
			}
		};
		
		friend ostream &operator<<(ostream &stream, LabelList other)
		{
			unsigned int ii;
			
			for(ii=0; ii < other.indexMap.size(); ii++)
			{
				stream << other.indexMap[ii] << " -> " << ii << endl;
			}
			
			return stream;
		};
};
