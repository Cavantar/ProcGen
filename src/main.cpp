#include "Includes.h"
#include "Game.h"
#include <algorithm>

bool isNumberOrItsPart(char c)
{
  if((c >= '0' && c <= '9') || c == '.') return true;
  return false;
}

typedef std::list<std::string> StringList;
std::string removeWhiteSpace(std::string str)
{
  str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
  return str;
}

enum ENTRY_TYPE{
  ET_NUMBER,
  ET_VARIABLE,
  ET_PARENTHESIS,
  ET_OPERATOR,
  ET_INVALID
};

ENTRY_TYPE getEntryType(char c)
{
  if(isNumberOrItsPart(c)) return ET_NUMBER;
  
  if((c >= 'a' && c <= 'z') ||
     (c >= 'A' && c <= 'Z')) return ET_VARIABLE;
  
  if(c == '(' || c == ')') return ET_PARENTHESIS;
  
  if(c == '+' || c == '-' ||
     c == '*' || c == '/') return ET_OPERATOR;

  assert(0);
  return ET_INVALID;
}

struct Entry {
  ENTRY_TYPE type;
  std::string value;
};

Entry popEntry(std::string& expression)
{
  Entry resultEntry;
  int endIndex = 0;
  
  resultEntry.type = getEntryType(expression[0]);
  assert(resultEntry.type != ET_INVALID);
  
  while(1)
  {
    resultEntry.value += expression[endIndex];
    endIndex++;

    if(resultEntry.type == ET_OPERATOR || resultEntry.type == ET_PARENTHESIS) break;
    if(endIndex == expression.size()) break;
    if(resultEntry.type != getEntryType(expression[endIndex])) break;
  }
  
  expression.erase(0, endIndex);
  return resultEntry;
}

typedef std::list<Entry> EntryList;

EntryList convertToReversePolish(std::string& expression)
{
  EntryList result;
  std::list<char> operatorStack;
  
  while(expression.size() != 0)
  {
    Entry entry = popEntry(expression);
    
    switch(entry.type)
    {
    case ET_NUMBER:
    case ET_VARIABLE:
      {
	result.push_back(entry);
      } break;
    case ET_PARENTHESIS:
      {
	if(operatorStack.size() != 0)
	{
	  Entry operatorEntry = { ET_OPERATOR, std::string(1, operatorStack.front()) } ;
	  result.push_back(operatorEntry);
	  
	  operatorStack.pop_front();
	}
      } break;
    case ET_OPERATOR:
      {
	operatorStack.push_back(entry.value[0]);
      } break;
    }
  }

  while(operatorStack.size() != 0)
  {
    Entry operatorEntry = { ET_OPERATOR, std::string(1, operatorStack.front()) } ;
    result.push_back(operatorEntry);
    operatorStack.pop_front();
  }
  
  return result;
};

float evaluateExpression(std::string expression)
{
  float variables[] = {0, 2, 4, 5};
  expression = removeWhiteSpace(expression);
  std::cout << expression << std::endl;
  
  EntryList reversePolish = convertToReversePolish(expression);
  
  std::cout << "\nReverse Polish: \n";
  std::cout << "-----------------\n\n";
  for(auto i = reversePolish.begin(); i != reversePolish.end(); i++)
  {
    std::cout << i->value << std::endl;
  }
  std::cout << "\n-----------------\n\n";
  
  return variables[3];
}

int main(int argc, char **argv) {

  // glutInit(&argc, argv);
  
  // Game game;
  // game.setupAndStart();


  std::string expression;

  std::cout << "Enter an expression: ";
  
  getline(std::cin, expression);
  
  float result = evaluateExpression(expression);
  
  std::cout << expression << " = " << result << std::endl;
  
  system("pause");
  return 0;
}
