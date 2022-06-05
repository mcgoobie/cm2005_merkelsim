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
  std::vector<string> inputCommand;
  knownCommands = {"help", "prod", "min", "max", "avg", "predict", "time", "step", "exit"};
  currentTime = orderBook.getEarliestTime();
  saveAvailableCurrency();
  cout << "advisorbot> Please enter a command, or help for a list of commands. " << endl;

  // Insert 10 BTC into user wallet.
  // wallet.insertCurrency("BTC", 10);

  while (true)
  {
    inputCommand = promptUserInput();
    processUserInput(inputCommand);
  }
}

std::vector<string> tokenise(string userInput, string delimiter)
{
  std::vector<string> tokens;
  signed int start, end;
  string token;
  start = userInput.find_first_not_of(delimiter, 0);

  cout << endl;
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
    cout << "Parsed tokens : " << token << endl;
    start = end + 1;
  } while (end > 0);
  cout << endl;

  return tokens;
}

std::vector<string> AdvisorBot::promptUserInput()
{
  string userInput = "";
  std::vector<string> userCommand;
  string line;

  cout << "advisorbot> ";
  std::getline(std::cin, line);

  try
  {
    userInput = line;
    userCommand = tokenise(userInput, " ");
  }
  catch (const std::exception &e)
  {
    //
  }
  return userCommand;
}

bool AdvisorBot::validateUserInput(string &inputCommand, std::vector<std::string> &cmds)
{
  return std::find(cmds.begin(), cmds.end(), inputCommand) != cmds.end();
}

void AdvisorBot::processUserInput(std::vector<string> inputCommand)
{
  if (inputCommand.empty())
  {
    return;
    cout << "advisorbot> Please enter a command, or help for a list of commands. " << endl;
  }
  // Validates user input as a exsisting cmd w/ 1 arguement.
  if (validateUserInput(inputCommand[0], knownCommands))
  {
    if (inputCommand[0].compare(knownCommands[0]) == 0 && inputCommand.size() == 1)
    {
      cout << "The available commands are, help, help [cmd], prod, min, max, avg, predict, time and step. " << endl;
    }
    // Checks if help command has additional arguements
    if (inputCommand[0].compare(knownCommands[0]) == 0 && inputCommand.size() > 1)
    {
      if (checkHelpArguements(inputCommand, knownCommands))
        fetchHelpCmdParams(helpParams);
      else
        notACommandError(inputCommand);
    }
    if (inputCommand[0].compare(knownCommands[1]) == 0)
    {
      cout << "======Retreiving list of available currency for sale======" << endl;
      listAvailableCurrency();
    }
    // if condition for 'min' : first token is 'min' and only 3 tokens in command : 1:min 2:ETH/BTC 3:OrderBookType
    if (inputCommand[0].compare(knownCommands[2]) == 0 && inputCommand.size() == 3)
    {
      findMinPrice(inputCommand);
    }
    // if condition for 'max' : first token is 'max' and only 3 tokens in command : 1:max 2:ETH/BTC 3:OrderBookType
    if (inputCommand[0].compare(knownCommands[3]) == 0 && inputCommand.size() == 3)
    {
      findMaxPrice(inputCommand);
    }
    // if condition for 'avg' : first token is 'avg' and only 4 tokens in command : 1:avg 2:ETH/BTC 3:OrderBookType 4:NoOfTimesteps
    if (inputCommand[0].compare(knownCommands[4]) == 0 && inputCommand.size() == 4)
    {
      cout << "avg" << endl;
      findAvgPrice(inputCommand);
    }
    if (inputCommand[0].compare(knownCommands[5]) == 0)
    {
      cout << "predict" << endl;
    }
    if (inputCommand[0].compare(knownCommands[6]) == 0)
    {
      std::cout << "advisorbot> Current time in dataset is : " << currentTime << std::endl;
    }
    if (inputCommand[0].compare(knownCommands[7]) == 0)
    {
      nextTimeStep();
    }
    if (inputCommand[0].compare(knownCommands[8]) == 0)
    {
      // Workaround to using ctrl+c to end the program, as it takes it as a command.
      cout << "advisorbot> Shutting down..." << endl;
      std::exit(0);
    }
  }
  // Checks if userInput is left empty
  else if (inputCommand[0].empty() || inputCommand[0].find_first_not_of(' ') == std::string::npos)
  {
    return;
    cout << "advisorbot> Please enter a command, or help for a list of commands. " << endl;
  }
  else
  {
    notACommandError(inputCommand);
  }
}

bool AdvisorBot::checkHelpArguements(std::vector<std::string> &inputCommand, std::vector<std::string> &cmds)
{
  // prepares an empty helpParams vector
  helpParams.clear();
  std::vector<string>::iterator it;
  // check if help is the first keyword in the command and that there is only 2 tokens, 1 for help and 1 for the opt-arg1.
  if ("help" == inputCommand.front() && inputCommand.size() == 2)
  {
    // check if command is in the format of help w/ an existing cmd as the following keyword.
    string cmd = inputCommand[1];
    it = std::find(cmds.begin(), cmds.end(), cmd);
    if (it != cmds.end())
    {
      // store help and following arguement keyword in params vector.
      helpParams.push_back(inputCommand[0]);
      helpParams.push_back(inputCommand[1]);
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

void AdvisorBot::saveAvailableCurrency()
{
  // getKnownProducts will retreive all available products and store them in productTypes vector.
  for (std::string const &p : orderBook.getKnownProducts())
  {
    productTypes.push_back(p);
  }
}

void AdvisorBot::listAvailableCurrency()
{
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

void AdvisorBot::findMinPrice(std::vector<string> &inputCommand)
{
  string currency = inputCommand[1];
  string type = inputCommand[2];
  std::vector<OrderBookEntry> entries;

  // check if currecy is a valid type
  if (validateUserInput(currency, productTypes) && ((type == "bid") || (type == "ask")))
  {
    if (type == "ask")
      entries = orderBook.getOrders(OrderBookType::ask, currency, currentTime);
    if (type == "bid")
      entries = orderBook.getOrders(OrderBookType::bid, currency, currentTime);

    cout << "advisorbot> The min " << type << " for " << currency << " is " << OrderBook::getLowPrice(entries) << endl;
  }
  else
  {
    cout << "advisorbot> Failed to fetch Min price for " << currency << ", make sure command syntax is correct. For more help, enter 'help min' ." << endl;
    return;
  }
}

void AdvisorBot::findMaxPrice(std::vector<string> &inputCommand)
{
  string currency = inputCommand[1];
  string type = inputCommand[2];
  std::vector<OrderBookEntry> entries;

  // check if currecy is a valid type
  if (validateUserInput(currency, productTypes) && ((type == "bid") || (type == "ask")))
  {
    if (type == "ask")
      entries = orderBook.getOrders(OrderBookType::ask, currency, currentTime);
    if (type == "bid")
      entries = orderBook.getOrders(OrderBookType::bid, currency, currentTime);

    cout << "advisorbot> The min " << type << " for " << currency << " is " << OrderBook::getHighPrice(entries) << endl;
  }
  else
  {
    cout << "advisorbot> Failed to fetch Max price for " << currency << ", make sure command syntax is correct. For more help, enter 'help max' ." << endl;
    return;
  }
}

void AdvisorBot::findAvgPrice(std::vector<std::string> &inputCommand)
{
  string currency = inputCommand[1];
  string type = inputCommand[2];
  // convert timestep from string to int.
  int noOfTimestep = std::stoi(inputCommand[3]);
  int no = 10;
  std::vector<OrderBookEntry> entries;
  if (validateUserInput(currency, productTypes) && ((type == "bid") || (type == "ask")))
  {
    if (type == "ask")
      entries = orderBook.getOrders(OrderBookType::ask, currency, currentTime);
    if (type == "bid")
      entries = orderBook.getOrders(OrderBookType::bid, currency, currentTime);

    OrderBook::getAvgPrice(entries, currentTime, noOfTimestep);
  }
  else
  {
    cout << "Something failed. " << endl;
  }
}

void AdvisorBot::nextTimeStep()
{
  std::cout << "Going to next timestep" << std::endl;
  currentTime = orderBook.getNextTime(currentTime);
}

void AdvisorBot::notACommandError(std::vector<string> &inputCommand)
{
  // If user types in an unrecognized command. Advisorbot will warn the user that their command is invalid.
  cout << "advisorbot> '";
  for (int i = 0; i != inputCommand.size(); ++i)
  {
    if (inputCommand[i] != inputCommand.back())
    {
      cout << inputCommand[i] << " ";
    }
    else
    {
      cout << inputCommand[i];
    }
  }
  cout << "' is not a command. " << endl;
}