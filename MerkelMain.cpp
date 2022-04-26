#include "MerkelMain.h"
#include <iostream>

MerkelMain::MerkelMain()
{
}

void MerkelMain::init()
{
    int input;
    while (true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}

void MerkelMain::loadOrderBook()
{
    std::vector<OrderBookEntry> orders;
    orders.push_back(OrderBookEntry{1000,
                                    0.02,
                                    "2020/03/17 17:01:24.884492",
                                    "BTC/USDT",
                                    OrderBookType::bid});

    orders.push_back(OrderBookEntry{2000,
                                    0.02,
                                    "2020/03/17 17:01:24.884492",
                                    "BTC/USDT",
                                    OrderBookType::bid});
}

void MerkelMain::printMenu()
{
    // 1 print help
    std::cout << "1 : Print Help" << std::endl;
    // 2 print exchange stats
    std::cout << "2 : Print exchange stats" << std::endl;
    // 3 make an offer
    std::cout << "1 : Make an offer" << std::endl;
    // 4 make a bid
    std::cout << "4 : Make a bid" << std::endl;
    // 5 print wallet
    std::cout << "5 : Print wallet" << std::endl;
    // 6 Continue
    std::cout << "6 : Continue" << std::endl;

    std::cout << "===========" << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make a bid. " << std::endl;
}

void MerkelMain::printMarketStats()
{
    std::cout << "Market looks good" << std::endl;
}

void MerkelMain::enterOffer()
{
    std::cout << "Make an offer - enter the amount" << std::endl;
}

void MerkelMain::enterBid()
{
    std::cout << "Make a bid - enter the amount" << std::endl;
}

void MerkelMain::printWallet()
{
    std::cout << "Your wallet is empty" << std::endl;
}

void MerkelMain::nextTimeframe()
{
    std::cout << "Going to next timeframe" << std::endl;
}

int MerkelMain::getUserOption()
{
    std::cout << "Type in 1 - 6" << std::endl;
    int userOption;
    std::cin >> userOption;
    return userOption;
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 0) // bad input
    {
        std::cout << "Invalid choice, choose 1 - 6" << std::endl;
    }
    if (userOption == 1) // bad input
    {
        printHelp();
    }
    if (userOption == 2) // bad input
    {
        printMarketStats();
    }
    if (userOption == 3) // bad input
    {
        enterOffer();
    }
    if (userOption == 4) // bad input
    {
        enterBid();
    }
    if (userOption == 5) // bad input
    {
        printWallet();
    }
    if (userOption == 6) // bad input
    {
        nextTimeframe();
    }
    std::cout << "You chose : " << userOption << std::endl;
}