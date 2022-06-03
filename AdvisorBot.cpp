#include "AdvisorBot.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "HelpCmds.h"
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
  knownCommands = {"help", "prod", "min", "max", "avg", "predict", "time", "step", "exit"};
  currentTime = orderBook.getEarliestTime();

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

void AdvisorBot::processUserInput(string userInput)
{
  // Validates user input as a exsisting cmd w/ 1 arguement.
  if (validateUserInput(userInput, knownCommands))
  {
    if (userInput.compare(knownCommands[0]) == 0)
    {
      cout << "The available commands are, help, prod, min, max, avg, predict, time and step. " << endl;
    }
    if (userInput.compare(knownCommands[1]) == 0)
    {
      cout << "======Retreiving list of available currency for sale======" << endl;
      fetchAvailableCurrency();
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
      std::cout << "advisorbot> Current time in dataset is : " << currentTime << std::endl;
    }
    if (userInput.compare(knownCommands[7]) == 0)
    {
      nextTimeStep();
    }
    if (userInput.compare(knownCommands[8]) == 0)
    {
      // Workaround to using ctrl+c to end the program, as it takes it as a command.
      cout << "advisorbot> Shutting down..." << endl;
      std::exit(0);
    }
  }
  // Checks if userInput is left empty
  else if (userInput.empty() || userInput.find_first_not_of(' ') == std::string::npos)
  {
    return;
    cout << "advisorbot> Please enter a command, or help for a list of commands. " << endl;
  }
  // Checks if help command has additional arguements
  else if (checkHelpArguements(userInput, knownCommands))
  {
    fetchHelpCmdParams(helpParams);
  }

  else
  {
    cout << "advisorbot> '" << userInput << "' is not a command. For information on available commands, type 'help'. " << endl;
  }
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

  // check if help is the first keyword in the command and that there is only 2 tokens, 1 for help and 1 for the opt-arg1.
  if ("help" == tokens.front() && tokens.size() == 2)
  {
    // check if command is in the format of help w/ an existing cmd as the following keyword.
    string cmd = tokens[1];
    cout << "arguement cmd is : " << cmd << endl;
    it = std::find(cmds.begin(), cmds.end(), cmd);
    if (it != cmds.end())
    {
      // store help and following arguement keyword in params vector.
      helpParams.push_back(tokens[0]);
      helpParams.push_back(tokens[1]);
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

void AdvisorBot::fetchHelpCmdParams(std::vector<string> &helpParams)
{
  // Compare 2nd token in helpParams vector to known commands and return proper syntax statements regarding it.
  if (helpParams[1].compare(knownCommands[0]) == 0)
  {
    helpCmds.getHelpCmdSyntax();
  }
  if (helpParams[1].compare(knownCommands[1]) == 0)
  {
    helpCmds.getProdCmdSyntax();
  }
  if (helpParams[1].compare(knownCommands[2]) == 0)
  {
    helpCmds.getMinCmdSyntax();
  }
  if (helpParams[1].compare(knownCommands[3]) == 0)
  {
    helpCmds.getMaxCmdSyntax();
  }
  if (helpParams[1].compare(knownCommands[4]) == 0)
  {
    helpCmds.getAvgCmdSyntax();
  }
  if (helpParams[1].compare(knownCommands[5]) == 0)
  {
    helpCmds.getPredictCmdSyntax();
  }
  if (helpParams[1].compare(knownCommands[6]) == 0)
  {
    helpCmds.getTimeCmdSyntax();
  }
  if (helpParams[1].compare(knownCommands[7]) == 0)
  {
    helpCmds.getStepCmdSyntax();
  }
  if (helpParams[1].compare(knownCommands[8]) == 0)
  {
    helpCmds.getExitCmdSyntax();
  }
}

void AdvisorBot::fetchAvailableCurrency()
{
  std::vector<string> productTypes;
  // getKnownProducts will retreive all available products.
  for (std::string const &p : orderBook.getKnownProducts())
  {
    productTypes.push_back(p);
  }

  for (int i = 0; i != productTypes.size(); ++i)
  {
    // iterate and print all productTypes, if productType is last in list, omit comma seperator.
    if (&productTypes[i] != &productTypes.back())
    {
      cout << productTypes[i] << ", ";
    }
    else
    {
      cout << productTypes[i] << endl;
    }
  }
}

void AdvisorBot::nextTimeStep()
{
  std::cout << "Going to next timeframe" << std::endl;
  for (std::string p : orderBook.getKnownProducts())
  {
    std::cout << "matching " << p << std::endl;
    std::vector<OrderBookEntry> sales = orderBook.matchAskToBids(p, currentTime);
    std::cout << "Sales: " << sales.size() << std::endl;
    // for (OrderBookEntry &sale : sales)
    // {
    //   std::cout << "Sale Price : " << sale.price << " amount " << sale.amount << std::endl;
    //   if (sale.username == "simuser")
    //   {
    //     // update the wallet
    //     wallet.processSale(sale);
    //   }
    // }
  }
  currentTime = orderBook.getNextTime(currentTime);
}
