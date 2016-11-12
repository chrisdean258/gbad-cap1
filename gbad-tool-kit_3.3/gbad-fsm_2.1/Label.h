#ifndef _LABEL_H_
#define _LABEL_H_

#include <iostream>

using namespace std;

// Label types
#define STRING_LABEL  0
#define NUMERIC_LABEL 1
#define NULL_LABEL 2

class Label
{
	protected:
		// one of STRING_LABEL or NUMERIC_LABEL
		char labelType;
		
		double numericLabel;
		string stringLabel;

	
	public:
		Label(int numericLabel)
		{
			labelType = NUMERIC_LABEL;
			Label::numericLabel = (double)numericLabel;
			stringLabel = "";
		};
		
		Label(double numericLabel)
		{
			labelType = NUMERIC_LABEL;
			Label::numericLabel = numericLabel;
			stringLabel = "";
		};
		
		Label(char *stringLabel)
		{
			labelType = STRING_LABEL;
			Label::stringLabel = string(stringLabel);
		};
		
		Label(string &stringLabel)
		{
			labelType = STRING_LABEL;
			Label::stringLabel = string(stringLabel);
		};
		
		Label(const Label &label)
		{
			labelType = label.labelType;
			numericLabel = label.numericLabel;
			stringLabel = label.stringLabel;
		};
		
		Label()
		{
			labelType = NULL_LABEL;
			stringLabel = "";
		}
		
		Label& operator=(const Label &other)
		{
			labelType = other.labelType;
			numericLabel = other.numericLabel;
			stringLabel = other.stringLabel;
			return *this;
		};
		
		~Label()
		{
			
		};
		
		int compare(const Label &other) const
		{
			if(Label::labelType != other.labelType)
			{
				if(Label::labelType < other.labelType)
				{
					return -1;
				}
				else
				{
					return 1;
				}
			}
			else if(Label::labelType == NUMERIC_LABEL)
			{
				if(Label::numericLabel == other.numericLabel)
				{
					return 0;
				}
				else
				{
					return (Label::numericLabel < other.numericLabel) ? -1 : 1;
				}
			}
			else if(Label::labelType == STRING_LABEL)
			{
				if(Label::stringLabel == other.stringLabel)
				{
					return 0;
				}
				else
				{
					return (Label::stringLabel < other.stringLabel) ? -1 : 1;
				}
			}
			else
			{
				return 0;
			}
		};
		
		bool operator==(const Label &other) const
		{
			return compare(other) == 0;
		}
		
		bool operator!=(const Label &other) const
		{
			return !(*this == other);
		}
		
		bool operator<(const Label &other) const
		{
			return compare(other) < 0;
		}
		
		bool operator>(const Label &other) const
		{
			return compare(other) > 0;
		}
		
		bool operator<=(const Label &other) const
		{
			return compare(other) <= 0;
		}

		bool operator>=(const Label &other) const
		{
			return compare(other) >= 0;
		};
		
		friend ostream &operator<<(ostream &stream, Label other)
		{
			if(other.labelType == NUMERIC_LABEL)
			{
				stream << other.numericLabel;
			}
			else if(other.labelType == STRING_LABEL)
			{
				stream << other.stringLabel;
			}
			else
			{
				stream << "(NULL)";
			}
			
			return stream;
		};
};

#endif
