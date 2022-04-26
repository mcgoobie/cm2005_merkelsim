class MerkelMain
{
public:
    MerkelMain();
    /** Call this to start the sim */
    void init();

private: /** Private functions only accessible through MerkelMain */
    void loadOrderBook();
    void printMenu();
    void printHelp();
    void printMarketStats();
    void enterOffer();
    void enterBid();
    void printWallet();
    void nextTimeframe();
    int getUserOption();
    void processUserOption(int userOption);
};