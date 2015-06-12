#pragma once

#include <iostream>
#include <list>
#include <unordered_map>

#define DEBUG_PARSER false

enum ENTRY_TYPE{
  ET_NUMBER,
  ET_VARIABLE,
  ET_FUNCTION,
  ET_PARENTHESIS,
  ET_SEPARATOR, // [,]
  ET_OPERATOR,
  ET_INVALID
};

struct Entry {
  ENTRY_TYPE type;
  std::string value;
};

typedef std::list<std::string> StringList;
typedef std::list<Entry> EntryList;
typedef std::unordered_map<std::string, float> VariableMap;

class SimpleParser {
public:
  float evaluateExpression(std::string expression) const;
  void setVariableMap(const VariableMap* variableMap) { this->variableMap = variableMap; }
  EntryList getReversePolish(std::string expression) const;
  std::string removeWhiteSpace(std::string str) const;
  float evaluateExpression(EntryList& reversePolish) const;
  
private:
  static const int maxOpStackSize = 16;
  const VariableMap* variableMap;
  
  bool isNumberOrItsPart(char c) const;
  
  ENTRY_TYPE getEntryType(char c) const;
  Entry popEntry(std::string& expression)const;
};
