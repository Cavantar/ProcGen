#include "Includes.h"
#include "Game.h"
#include "SimpleParser.h"

int main(int argc, char **argv)
{
  
#if 1
  
  glutInit(&argc, argv);
  
  Game game;
  game.setupAndStart();
  
#else
  
  VariableMap variableMap;
  variableMap["x"] = 4.5f;
  variableMap["y"] = 3.0f;
  
  SimpleParser simpleParser;
  simpleParser.setVariableMap(&variableMap);
  
  std::string expression;
  
  do {
    
    std::cout << "Enter an expression: ";
    
    getline(std::cin, expression);
    if(expression == "exit") break;
    float result = simpleParser.evaluateExpression(expression);
    
    std::cout << expression << " = " << result << std::endl;
    
  } while (expression != "exit");
  
  system("pause");
  
#endif
  
  return 0;
}
