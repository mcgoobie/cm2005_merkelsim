#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
#include "HelpCmds.h"

class AdvisorBot
{
public:
  AdvisorBot();
  /** Call this to start the sim */
  void init();

private:
  std::string promptUserInput();
  void processUserInput(std::string userInput);
  // Check if userInput matches any of the commands of advisorBot
  bool validateUserInput(std::string &userInput, std::vector<std::string> &arrayOfCmds);
  bool checkHelpArguements(std::string &userInput, std::vector<std::string> &arrayOfCmds);
  void fetchHelpCmdParams(std::vector<std::string> &helpParams);
  void printHelp();

  std::vector<std::string> helpParams;
  std::vector<std::string> knownCommands;
  HelpCmds helpCmds;
};