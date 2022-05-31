#include "AdvisorBot.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>

using std::cout;
using std::endl;
using std::string;

AdvisorBot::AdvisorBot()
{
}

void AdvisorBot::init()
{
  string userInput;
  // currentTime = orderBook.getEarliestTime();

  // Insert 10 BTC into user wallet.
  // wallet.insertCurrency("BTC", 10);

  while (true)
  {
    userInput = promptUserInput();
    processUserInput(userInput);
  }
}

string AdvisorBot::promptUserInput()
{
  cout << "advisorbot> Please enter a command, or help for a list of commands. " << endl;
  string userInput = "";
  string line;

  std::getline(std::cin, line);

  try
  {
    userInput = line;
  }
  catch (const std::exception &e)
  {
    //
  }
  return userInput;
}

bool AdvisorBot::validateUserInput(string &userInput, std::vector<std::string> &cmds)
{
  return std::find(cmds.begin(), cmds.end(), userInput) != cmds.end();
}

std::vector<std::string> tokenise(string userInput, string delimiter)
{
  std::vector<std::string> tokens;
  signed int start, end;
  std::string token;
  start = userInput.find_first_not_of(delimiter, 0);

  do
  {
    end = userInput.find_first_of(delimiter, start);
    if (start == userInput.length() || start == end)
      break;
    if (end >= 0)
      token = userInput.substr(start, end - start);
    else
      token = userInput.substr(start, userInput.length() - start);
    tokens.push_back(token);
    start = end + 1;
  } while (end > 0);

  return tokens;
}

bool AdvisorBot::checkHelpArguements(string &userInput, std::vector<std::string> &cmds)
{
  // prepares an empty helpParams vector
  helpParams.clear();
  // split cmd into tokens to read each char
  std::vector<string> tokens = tokenise(userInput, " ");

  std::vector<string>::iterator it;

  // check if help is the first keyword in the command.
  if ("help" == tokens.front())
  {
    // check if command is in the format of help w/ an existing cmd as the following keyword.
    string cmd = tokens[1];
    it = std::find(cmds.begin(), cmds.end(), cmd);
    if (it != cmds.end())
    {
      // store help and following arguement keyword in params vector.
      helpParams.push_back(tokens[0]);
      helpParams.push_back(tokens[1]);

      cout << "Help params are : " << tokens[0] << " " << tokens[1] << endl;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
  return true;
}

void AdvisorBot::processUserInput(string userInput)
{
  std::vector<std::string> knownCommands{"help", "prod", "min", "max", "avg", "predict", "time", "step", "exit"};

  // Validates user input as a exsisting cmd w/ 1 arguement.
  if (validateUserInput(userInput, knownCommands))
  {
    if (userInput.compare(knownCommands[0]) == 0)
    {
      cout << "The available commands are, help, prod, min, max, avg, predict, time and step. " << endl;
    }
    if (userInput.compare(knownCommands[1]) == 0)
    {
      cout << "prod" << endl;
    }
    if (userInput.compare(knownCommands[2]) == 0)
    {
      cout << "min" << endl;
    }
    if (userInput.compare(knownCommands[3]) == 0)
    {
      cout << "max" << endl;
    }
    if (userInput.compare(knownCommands[4]) == 0)
    {
      cout << "avg" << endl;
    }
    if (userInput.compare(knownCommands[5]) == 0)
    {
      cout << "predict" << endl;
    }
    if (userInput.compare(knownCommands[6]) == 0)
    {
      cout << "time" << endl;
    }
    if (userInput.compare(knownCommands[7]) == 0)
    {
      cout << "step" << endl;
    }
    if (userInput.compare(knownCommands[8]) == 0)
    {
      // Workaround to using ctrl+c to end the program, as it takes it as a command.
      cout << "advisorbot> Shutting down..." << endl;
      std::exit(0);
    }
  }
  // Checks if help command has additional arguements
  else if (checkHelpArguements(userInput, knownCommands))
  {
    // cout << "checkHelpArguements::" << helpParams[0] << helpParams[1] << endl;
  }
  // Checks if userInput is left empty
  else if (userInput.empty())
  {
    return;
    cout << "advisorbot> Please enter a command, or help for a list of commands. " << endl;
  }
  else
  {
    cout << "advisorbot> '" << userInput << "' is not a command. For information on available commands, type 'help'. " << endl;
  }
}

void AdvisorBot::printHelp()
{
}