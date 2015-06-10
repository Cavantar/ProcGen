#include "Includes.h"
#include "Game.h"
#include "SimpleParser.h"

int main(int argc, char **argv)
{
  
  // glutInit(&argc, argv);
  
  // Game game;
  // game.setupAndStart();
  
  VariableMap variableMap;
  variableMap["Map1"] = 4.5f;
  variableMap["Map2"] = 3.0f;
  
  SimpleParser simpleParser(variableMap);
  
  std::string expression;
  std::cout << "Enter an expression: ";
  
  getline(std::cin, expression);
  float result = simpleParser.evaluateExpression(expression);
  
  std::cout << expression << " = " << result << std::endl;
  
  system("pause");
  return 0;
}
