#include "OrderBook.h"
#include "CSVReader.h"
#include <map>
#include <iostream>
#include <algorithm>

/** construct, reading a csv file */
OrderBook::OrderBook(std::string filename)
{
  orders = CSVReader::readCSV(filename);
}

/** return a vector of all known products in the database */
std::vector<std::string> OrderBook::getKnownProducts()
{
  std::vector<std::string> products;

  std::map<std::string, bool> prodMap;

  for (OrderBookEntry &e : orders)
  {
    prodMap[e.product] = true;
  }

  // now flatten the map to a vector of strings
  for (auto const &e : prodMap)
  {
    products.push_back(e.first);
  }

  return products;
}

/** return vector of Orders according to the sent filters */
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, std::string product, std::string timestamp)
{
  std::vector<OrderBookEntry> orders_sub;
  for (OrderBookEntry &e : orders)
  {
    if (e.orderType == type && e.product == product && e.timestamp == timestamp)
    {
      orders_sub.push_back(e);
    }
  }
  return orders_sub;
}

double OrderBook::getHighPrice(std::vector<OrderBookEntry> &orders)
{
  double max = orders[0].price;
  for (OrderBookEntry &e : orders)
  {
    if (e.price > max)
      max = e.price;
  }
  return max;
};

double OrderBook::getAvgPrice(std::vector<OrderBookEntry> &orders, std::string currentTime)
{
  double totalPrice;
  double avgPrice;

  for (OrderBookEntry &e : orders)
  {
    // Find total price of all products in the listed timesteps
    totalPrice += e.price;
  }
  // Divide price by the total amount of orders to get the avg
  avgPrice = totalPrice / orders.size();
  return avgPrice;
};

double OrderBook::getLowPrice(std::vector<OrderBookEntry> &orders)
{
  double min = orders[0].price;
  for (OrderBookEntry &e : orders)
  {
    if (e.price < min)
      min = e.price;
  }
  return min;
};

double OrderBook::getWeightedMovingAvg(std::vector<double> &minMax)
{
  // Get the sum of the weightage of X prices in the list.
  int weightage = (minMax.size() * (minMax.size() + 1)) / 2;
  double weightedMovingAvg;
  float weightedPrice;

  for (int i = 0; i != minMax.size(); ++i)
  {
    weightedPrice = minMax[i] * ((i + 1.0) / weightage);
    weightedMovingAvg += weightedPrice;
  }

  return weightedMovingAvg;
};

// std::vector<OrderBookEntry> OrderBook::fetchTopThreeProducts(std::vector<OrderBookEntry> &orders, std::vector<std::string> &productTypes)
// {
//   std::vector<OrderBookEntry>::iterator it;
//   std::vector<OrderBookEntry> popularCurrency, curr1, curr2;

//   for (int i = 0; i != productTypes.size(); ++i)
//   {
//     std::string product1 = productTypes[i];
//     std::string product2 = productTypes[i + 1];

//     for (OrderBookEntry &e : orders)
//     {
//       if (e.product == product1)
//         curr1.push_back(e);
//       if (e.product == product2)
//         curr2.push_back(e);
//     }

//     if (curr1.size() > curr2.size())
//     {
//       for (OrderBookEntry &e : curr1)
//         popularCurrency.push_back(e);
//       curr2.clear;
//     }
//     else
//     {
//       for (OrderBookEntry &e : orders)
//       {
//         for (OrderBookEntry &e : curr2)
//           popularCurrency.push_back(e);
//         curr1.clear;
//       }

//       // std::cout << "fetchTopThreeProducts::Product : " << curr1[i].product << " w/ total of " << curr1.size() << " entries.  " << std::endl;
//     }

//     return curr1;
//   }

  std::string OrderBook::getEarliestTime()
  {
    return orders[0].timestamp;
  }

  std::string OrderBook::getNextTime(std::string timestamp)
  {
    std::string next_timestamp = "";
    for (OrderBookEntry &e : orders)
    {
      if (e.timestamp > timestamp)
      {
        next_timestamp = e.timestamp;
        break;
      }
    }
    if (next_timestamp == "")
    {
      next_timestamp = orders[0].timestamp;
    }
    return next_timestamp;
  }

  void OrderBook::insertOrder(OrderBookEntry & order)
  {
    orders.push_back(order);
    std::sort(orders.begin(), orders.end(), OrderBookEntry::compareByTimestamp);
  }

  std::vector<OrderBookEntry> OrderBook::matchAskToBids(std::string product, std::string timestamp)
  {
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask,
                                                 product,
                                                 timestamp);
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid,
                                                 product,
                                                 timestamp);

    std::vector<OrderBookEntry> sales;
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceDsc);
    for (OrderBookEntry &ask : asks)
    {
      for (OrderBookEntry &bid : bids)
      {
        if (bid.price >= ask.price)
        {
          OrderBookEntry sale{
              ask.price,
              0,
              timestamp,
              product,
              OrderBookType::asksale};

          if (bid.username == "simuser")
          {
            sale.username = "simuser";
            sale.orderType = OrderBookType::bidsale;
          }
          if (ask.username == "simuser")
          {
            sale.username = "simuser";
            sale.orderType = OrderBookType::asksale;
          }

          // if bid.amount == ask.amount: # bid completely clears ask
          if (bid.amount == ask.amount)
          {
            sale.amount = ask.amount;
            sales.push_back(sale);
            bid.amount = 0;
            break;
          }

          // if bid.amount > ask.amount: # ask is completely gone slice the bid
          if (bid.amount > ask.amount)
          {
            sale.amount = ask.amount;
            sales.push_back(sale);
            bid.amount = bid.amount - ask.amount;
            break;
          }

          // if bid.amount < ask.amount # bid is completely gone, slice the ask
          if (bid.amount < ask.amount && bid.amount > 0)
          {
            sale.amount = bid.amount;
            sales.push_back(sale);
            ask.amount = ask.amount - bid.amount;
            bid.amount = 0;
            continue;
          }
        }
      }
    }

    return sales;
  }