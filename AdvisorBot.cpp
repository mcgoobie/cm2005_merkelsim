#include "AdvisorBot.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "HelpCmds.h"
#include "CurrentTrend.h"
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
  knownCommands = {"help", "prod", "min", "max", "avg", "predict", "time", "step", "current"};
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
      cout << "-Help-" << endl;
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
      cout << "-Prod-" << endl;
      cout << "======Retreiving list of available currency for sale======" << endl;
      listAvailableCurrency();
    }
    // if condition for 'min' : first token is 'min' and only 3 tokens in command : 1:min 2:ETH/BTC 3:OrderBookType
    if (inputCommand[0].compare(knownCommands[2]) == 0 && inputCommand.size() == 3)
    {
      cout << "-Min-" << endl;
      findMinPrice(inputCommand);
    }
    // if condition for 'max' : first token is 'max' and only 3 tokens in command : 1:max 2:ETH/BTC 3:OrderBookType
    if (inputCommand[0].compare(knownCommands[3]) == 0 && inputCommand.size() == 3)
    {
      cout << "-Max-" << endl;
      findMaxPrice(inputCommand);
    }
    // if condition for 'avg' : first token is 'avg' and only 4 tokens in command : 1:avg 2:ETH/BTC 3:OrderBookType 4:NoOfTimesteps
    // User can only get avg of past timeframes that are available from the current timeframe in the dataset
    if (inputCommand[0].compare(knownCommands[4]) == 0 && inputCommand.size() == 4)
    {
      cout << "-Avg-" << endl;
      findAvgPrice(inputCommand, pastTimeFrames);
    }
    if (inputCommand[0].compare(knownCommands[5]) == 0 && inputCommand.size() == 4)
    {
      cout << "-Predict-" << endl;
      predictWeightedMovingAvg(inputCommand, pastTimeFrames);
    }
    if (inputCommand[0].compare(knownCommands[6]) == 0)
    {
      cout << "-Time-" << endl;
      std::cout << "advisorbot> " << currentTime << std::endl;
    }
    if (inputCommand[0].compare(knownCommands[7]) == 0)
    {
      cout << "-Step-" << endl;
      nextTimeStep();
    }
    if (inputCommand[0].compare(knownCommands[8]) == 0)
    {
      cout << "-Current-" << endl;
      // List top 3 most traded currencies and the highest price of each
      getCurrentTrends(inputCommand);
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

void AdvisorBot::findAvgPrice(std::vector<string> &inputCommand, std::vector<string> &pastTimeFrames)
{
  string currency = inputCommand[1];
  string type = inputCommand[2];

  // convert no of timestep from string to int.
  int noOfTimestep = std::stoi(inputCommand[3]);

  // Store list of timesteps in order of most recent first to earliest in the dataset (backwards).
  std::vector<string> recentTimeSteps(pastTimeFrames.rbegin(), pastTimeFrames.rend());
  std::vector<OrderBookEntry> entries;

  if (noOfTimestep > recentTimeSteps.size())
  {
    // Checks if user specified timestep amount is larger than the size of recentTimeSteps list, if so, pull from earliest point in dataset instead.
    noOfTimestep = recentTimeSteps.size();
    cout << "advisorbot> Bot will only fetch average from " << noOfTimestep
         << " timesteps ago, as timestep amount specified in command predates the earliest time in the dataset. " << endl;
  }
  if (validateUserInput(currency, productTypes) && ((type == "bid") || (type == "ask")))
  {
    if (type == "ask")
    {
      for (int i = 0; i != noOfTimestep; ++i)
      {
        entries = orderBook.getOrders(OrderBookType::ask, currency, recentTimeSteps[i]);
      }
    }
    if (type == "bid")
    {
      for (int i = 0; i != noOfTimestep; ++i)
        entries = orderBook.getOrders(OrderBookType::bid, currency, recentTimeSteps[i]);
    }
    cout << " The average " << currency << " " << type << " price over the last " << noOfTimestep << " timesteps was " << OrderBook::getAvgPrice(entries, currentTime) << endl;
  }
}

void AdvisorBot::predictWeightedMovingAvg(std::vector<string> &inputCommand, std::vector<string> &pastTimeFrames)
{
  string minMax = inputCommand[1];
  string currency = inputCommand[2];
  string type = inputCommand[3];
  std::vector<OrderBookEntry> entries;
  std::vector<double> minMaxPrices;

  if (validateUserInput(currency, productTypes) && ((type == "bid") || (type == "ask")))
  {
    if (type == "ask")
    {
      for (int i = 0; i != pastTimeFrames.size(); ++i)
      {
        entries = orderBook.getOrders(OrderBookType::ask, currency, pastTimeFrames[i]);

        if (minMax == "min")
        {
          cout << "c" << endl;
          minMaxPrices.push_back(OrderBook::getLowPrice(entries));
        }
        else if (minMax == "max")
          minMaxPrices.push_back(OrderBook::getHighPrice(entries));
      }
    }
    if (type == "bid")
    {
      for (int i = 0; i != pastTimeFrames.size(); ++i)
      {
        entries = orderBook.getOrders(OrderBookType::bid, currency, pastTimeFrames[i]);

        if (minMax == "min")
          minMaxPrices.push_back(OrderBook::getLowPrice(entries));
        else if (minMax == "max")
          minMaxPrices.push_back(OrderBook::getHighPrice(entries));
      }
    }
  }
  cout << "advisorbot> The average " << currency << " " << type << " price over the last " << pastTimeFrames.size()
       << " timesteps was " << OrderBook::getWeightedMovingAvg(minMaxPrices) << endl;
}

void AdvisorBot::getCurrentTrends(std::vector<std::string> &inputCommand)
{
  string minMax = inputCommand[1];
  string type = inputCommand[2];
  std::vector<OrderBookEntry> entries1, entriesTemp;
  std::vector<std::vector<OrderBookEntry> > comparisonList;

  if (((minMax == "min") || (minMax == "max")) && ((type == "bid") || (type == "ask")))
  {
    if (type == "ask")
    {
      entries1 = orderBook.getOrders(OrderBookType::ask, productTypes[0], currentTime);
      cout << "getCurrentTrends::entry1.size() = " << entries1.size() << " and product is " << productTypes[0] << endl;
      comparisonList.push_back(entries1);
      for (int i = 0; i != productTypes.size() - 1; ++i)
      {
        entriesTemp = orderBook.getOrders(OrderBookType::ask, productTypes[i + 1], currentTime);
        comparisonList.push_back(entriesTemp);
        cout << "getCurrentTrends::entryTemp.size() = " << entriesTemp.size() << " and product is " << productTypes[i + 1] << endl;

        if (comparisonList[0].size() > comparisonList[1].size())
          comparisonList.erase(comparisonList.begin() + 1);
        if (comparisonList[0].size() < comparisonList[1].size())
          comparisonList.erase(comparisonList.begin());
      }

      cout << "Product with most amount of sales at this timestep is : " << comparisonList[0][0].product << endl;
    }
    // if (type == "bid")
    // {
    //   entries1 = orderBook.getOrders(OrderBookType::ask, productTypes[1], currentTime);
    //   entriesTemp = orderBook.getOrders(OrderBookType::ask, productTypes[2], currentTime);
    // }
  }

  // string minMax = inputCommand[1];
  // string type = inputCommand[2];
  // std::vector<OrderBookEntry> entries1, entries2, entries3, entries4, entries5, allEntries, popularProduct;

  // if (((minMax == "min") || (minMax == "max")) && ((type == "bid") || (type == "ask")))
  // {
  //   if (type == "ask")
  //   {
  //     entries1 = orderBook.getOrders(OrderBookType::ask, productTypes[1], currentTime);
  //     entries2 = orderBook.getOrders(OrderBookType::ask, productTypes[2], currentTime);
  //     entries3 = orderBook.getOrders(OrderBookType::ask, productTypes[3], currentTime);
  //     entries4 = orderBook.getOrders(OrderBookType::ask, productTypes[4], currentTime);
  //     entries5 = orderBook.getOrders(OrderBookType::ask, productTypes[5], currentTime);
  //   }
  //   if (type == "bid")
  //   {
  //     entries1 = orderBook.getOrders(OrderBookType::bid, productTypes[1], currentTime);
  //     entries2 = orderBook.getOrders(OrderBookType::bid, productTypes[2], currentTime);
  //     entries3 = orderBook.getOrders(OrderBookType::bid, productTypes[3], currentTime);
  //     entries4 = orderBook.getOrders(OrderBookType::bid, productTypes[4], currentTime);
  //     entries5 = orderBook.getOrders(OrderBookType::bid, productTypes[5], currentTime);
  //   }
  // }

  // if (entries1.size() < entries2.size())
  //   popularProduct
}

void AdvisorBot::nextTimeStep()
{
  pastTimeFrames.push_back(currentTime);

  currentTime = orderBook.getNextTime(currentTime);
  cout << "advisorbot> now at " << currentTime << endl;
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