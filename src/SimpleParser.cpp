#include "SimpleParser.h"
#include <algorithm>
#include <assert.h>
#include <string>

bool SimpleParser::isNumberOrItsPart(char c) const 
{
  if((c >= '0' && c <= '9') || c == '.') return true;
  return false;
}

std::string SimpleParser::removeWhiteSpace(std::string str) const
{
  str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
  return str;
}

ENTRY_TYPE SimpleParser::getEntryType(char c) const
{
  if(isNumberOrItsPart(c)) return ET_NUMBER;
  
  if((c >= 'a' && c <= 'z') ||
     (c >= 'A' && c <= 'Z')) return ET_VARIABLE;
  
  if(c == '(' || c == ')') return ET_PARENTHESIS;
  
  if(c == '+' || c == '-' ||
     c == '*' || c == '/' || c == '^') return ET_OPERATOR;
    
  if(c == ',') return ET_SEPARATOR;
  
  assert(0);
  return ET_INVALID;
}

Entry SimpleParser::popEntry(std::string& expression) const
{
  Entry resultEntry;
  int endIndex = 0;
  
  resultEntry.type = getEntryType(expression[0]);
  assert(resultEntry.type != ET_INVALID);
  
  while(1)
  {
    resultEntry.value += expression[endIndex];
    endIndex++;
    
    // Operators and parenthesis are only one character long
    if(resultEntry.type == ET_OPERATOR || resultEntry.type == ET_PARENTHESIS ||
       resultEntry.type == ET_SEPARATOR) break;
    
    // Making Sure We won't go out of bounds
    if(endIndex == expression.size()) break;
    
    if(resultEntry.type == ET_VARIABLE && getEntryType(expression[endIndex]) == ET_NUMBER) continue;
    
    // Breaking when we come to the other entry
    if(resultEntry.type != getEntryType(expression[endIndex])) break;
  }
  
  // We get function characters just like variables so we can just check if current variable name is function name
  if(resultEntry.type == ET_VARIABLE &&
     (resultEntry.value == "min" || resultEntry.value == "max" ||
      resultEntry.value == "sin" || resultEntry.value == "cos" ||
      resultEntry.value == "abs"))
  {
    resultEntry.type = ET_FUNCTION;
  }
  
  expression.erase(0, endIndex);
  return resultEntry;
}

EntryList SimpleParser::getReversePolish(std::string expression) const
{
  expression = removeWhiteSpace(expression);

  EntryList result;
  int stackIndex = 0;
  EntryList operationStack[maxOpStackSize];
  
  while(expression.size() != 0)
  {
    Entry entry = popEntry(expression);

    //std::cout << stackIndex << std::endl;
    
    switch(entry.type)
    {
    case ET_NUMBER:
    case ET_VARIABLE:
      {
	result.push_back(entry);
      } break;
    case ET_PARENTHESIS:
      {
	if(entry.value == "(") ++stackIndex;
	else if(entry.value == ")")
	{
	  // Getting All Operators from Current Stack
	  while(operationStack[stackIndex].size())
	  {
	    result.push_back(operationStack[stackIndex].back());
	    operationStack[stackIndex].pop_back();
	  }
	  
	  --stackIndex;
	  if(operationStack[stackIndex].size() != 0)
	  {
	    result.push_back(operationStack[stackIndex].back());
	    operationStack[stackIndex].pop_back();
	  }
	}
      } break;
    case ET_SEPARATOR:
      {
	if(entry.value == ",")
	{
	  while(operationStack[stackIndex].size())
	  {
	    result.push_back(operationStack[stackIndex].back());
	    operationStack[stackIndex].pop_back();
	  }

	}
      } break;
    case ET_FUNCTION:
    case ET_OPERATOR:
      {
	operationStack[stackIndex].push_back(entry);
      } break;
    }

    // std::cout << "Size: " << result.size() << " " << entry.value << std::endl;
  }
  
  while(operationStack[stackIndex].size() != 0)
  {
    result.push_back(operationStack[stackIndex].back());
    operationStack[stackIndex].pop_back();
  }

  if(DEBUG_PARSER)
  {
    std::cout << "\nReverse Polish: \n";
    std::cout << "-----------------\n\n";
    for(auto i = result.begin(); i != result.end(); i++)
    {
      std::cout << i->value << std::endl;
    }
    std::cout << "\n-----------------\n\n";
  }

  return result;
};

float SimpleParser::evaluateExpression(EntryList& reversePolish) const
{
  Entry entry = reversePolish.back();
  reversePolish.pop_back();
  
  switch(entry.type)
  {
  case ET_NUMBER:
    {
      return std::stof(entry.value);
    } break;
  case ET_VARIABLE:
    {
      std::string variableName = entry.value;
      if(variableMap->count(variableName))
      {
	return variableMap->at(variableName);
      }
      std::cout << "Coulnd find variable: " << variableName << std::endl;
      return 0;
    } break;
  case ET_OPERATOR:
    {
      // This pops things
      float right = evaluateExpression(reversePolish);
      
      // And this too
      float left = evaluateExpression(reversePolish);

      if(DEBUG_PARSER)
      {
	std::cout << "Left: " << left << std::endl;
	std::cout << "Right: "<< right << std::endl;
	std::cout << "Operator: "<< entry.value[0] << std::endl;
      }
      
      char operatorC = entry.value[0];
      switch(operatorC)
      {
      case '+': return left + right;
      case '-': return left - right;
      case '*': return left * right;
      case '/': return left / right;
      case '^': return pow(left, right);
      }
    } break;
  case ET_FUNCTION:
    {
      
      std::string function = entry.value;
      
      if(function == "min" || function == "max")
      {
	// This pops things
	float right = evaluateExpression(reversePolish);
      
	// And this too
	float left = evaluateExpression(reversePolish);

	
	if(DEBUG_PARSER)
	{
	  std::cout << "Left: " << left << std::endl;
	  std::cout << "Right: "<< right << std::endl;
	  std::cout << "Function: "<< function << std::endl;
	}

	if(function == "min") return std::min(left, right);
	else if(function == "max") return std::max(left, right);
      }
      else
      {
	float argument = evaluateExpression(reversePolish);
	
	if(DEBUG_PARSER)
	{
	  std::cout << "Argument: " << argument << std::endl;
	  std::cout << "Function: "<< function << std::endl;
	}
	
	if(function == "sin") return sin(argument);
	else if(function == "cos") return cos(argument);
	else if(function == "abs") return abs(argument);
	else std::cout << "Function: " << function << " doesn't exist" << std::endl;
	
      }
    } break;
  }
  
  return std::stof(entry.value);
}

float SimpleParser::evaluateExpression(std::string expression) const
{
  EntryList reversePolish = getReversePolish(expression);
  
  float result; 
  result = evaluateExpression(reversePolish);
  return result;
}  

