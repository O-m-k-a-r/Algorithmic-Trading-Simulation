// #include <iostream>
// #include <fstream>
// #include <vector>
// #include "rapidjson-master/include/rapidjson/document.h"
// #include "rapidjson-master/include/rapidjson/filereadstream.h"

// using namespace rapidjson;

// // Function to calculate the moving average
// double calculateMovingAverage(const std::vector<double>& prices, int startIdx, int period) {
//     double sum = 0.0;
//     for (int i = startIdx; i < startIdx + period; ++i) {
//         sum += prices[i];
//     }
//     return sum / period;
// }

// int main() {
//     // Read the AAPL.json file
//     FILE* file = fopen("AAPL.json", "r");
//     if (!file) {
//         std::cerr << "Failed to open AAPL.json" << std::endl;
//         return 1;
//     }
//     char buffer[65536];
//     FileReadStream fileStream(file, buffer, sizeof(buffer));
//     Document document;
//     document.ParseStream(fileStream);
//     fclose(file);

//     // Extract closing prices
//     std::vector<double> closingPrices;
//     const Value& data = document["Time Series (Daily)"];
//     for (SizeType i = 0; i < data.Size(); ++i) {
//         const Value& entry = data[i];
//         closingPrices.push_back(entry["4. close"].GetDouble());
//     }

//     // Calculate moving averages
//     int shortPeriod = 10;
//     int longPeriod = 50;
//     std::vector<double> shortMA;
//     std::vector<double> longMA;
//     for (int i = longPeriod - 1; i < closingPrices.size(); ++i) {
//         shortMA.push_back(calculateMovingAverage(closingPrices, i - shortPeriod + 1, shortPeriod));
//         longMA.push_back(calculateMovingAverage(closingPrices, i - longPeriod + 1, longPeriod));
//     }

//     // Generate buy/sell signals
//     std::vector<int> signals;
//     for (int i = 1; i < shortMA.size(); ++i) {
//         if (shortMA[i] > longMA[i] && shortMA[i - 1] <= longMA[i - 1]) {
//             signals.push_back(1); // Buy signal
//         } else if (shortMA[i] < longMA[i] && shortMA[i - 1] >= longMA[i - 1]) {
//             signals.push_back(-1); // Sell signal
//         } else {
//             signals.push_back(0); // No signal
//         }
//     }

//     // Simulate trading
//     double capital = 100000.0; // Initial capital
//     double shares = 0.0;
//     for (int i = 0; i < signals.size(); ++i) {
//         if (signals[i] == 1) {
//             shares += capital / closingPrices[i + longPeriod - 1];
//             capital = 0.0;
//         } else if (signals[i] == -1) {
//             capital += shares * closingPrices[i + longPeriod - 1];
//             shares = 0.0;
//         }
//     }

//     // Calculate final portfolio value
//     double portfolioValue = capital + shares * closingPrices.back();
//     std::cout << "Final portfolio value: " << portfolioValue << std::endl;

//     return 0;
// }


#include <iostream>
#include <fstream>
#include <vector>
#include "rapidjson-master/include/rapidjson/document.h"
#include "rapidjson-master/include/rapidjson/filereadstream.h"
using namespace rapidjson;


double calculateMovingAverage(const std::vector<double>& prices, int startIdx, int period) {
    double sum = 0.0;
    for (int i = startIdx; i < startIdx + period; ++i) {
        sum += prices[i];
    }
    return sum / period;
}


int main() {
    
    FILE* file = fopen("GOOG.json", "r");
    if (!file) {
        std::cerr << "Failed to open AAPL.json" << std::endl;
        return 1;
    }

    char buffer[65536];
    FileReadStream fileStream(file, buffer, sizeof(buffer));
    Document document;
    document.ParseStream(fileStream);
    fclose(file);

    
    const Value& timeSeries = document["Time Series (Daily)"];
    if (!timeSeries.IsObject()) {
        std::cerr << "Invalid JSON format: 'Time Series (Daily)' not found or not an object" << std::endl;
        return 1;
    }

    
    std::vector<double> closeValues;

    
    for (Value::ConstMemberIterator itr = timeSeries.MemberBegin(); itr != timeSeries.MemberEnd(); ++itr) {
        const Value& date = itr->value;
        if (!date.IsObject()) {
            std::cerr << "Invalid JSON format: date entry is not an object" << std::endl;
            return 1;
        }

        const Value& closeValue = date["4. close"];
        if (!closeValue.IsString()) {
            std::cerr << "Invalid JSON format: '4. close' value is not a string" << std::endl;
            return 1;
        }

        
        double close = std::stod(closeValue.GetString());
        closeValues.push_back(close);
    }

    
    int shortPeriod = 50;
    int longPeriod = 60;
    std::vector<double> shortMA;
    std::vector<double> longMA;
    for (int i = longPeriod - 1; i < closeValues.size(); ++i) {
        shortMA.push_back(calculateMovingAverage(closeValues, i - shortPeriod + 1, shortPeriod));
        longMA.push_back(calculateMovingAverage(closeValues, i - longPeriod + 1, longPeriod));
    }
    
    // for(const auto& sm: shortMA) {
    // std::cout<<"Short MA: "<< shortMA<<std::endl;
    // }
    // for(const auto& lm: longMA) {
    // std::cout<<"Long MA: "<< longMA<<std::endl;
    // }

    
    std::vector<int> signals;
    for (int i = 1; i < shortMA.size(); ++i) {
        if (shortMA[i] > longMA[i] && shortMA[i - 1] <= longMA[i - 1]) {
            signals.push_back(1); // Buy signal
            std::cout<<"Buy signal"<<std::endl;
        } else if (shortMA[i] < longMA[i] && shortMA[i - 1] >= longMA[i - 1]) {
            signals.push_back(-1); // Sell signal
            std::cout<<"Sell signal"<<std::endl;
        } else {
            signals.push_back(0); // No signal
            std::cout<<"No signal"<<std::endl;
        }
    }

     
    std::ofstream outFile("signals.txt");
    if (!outFile.is_open()) {
        std::cerr << "Failed to open signals.txt for writing" << std::endl;
        return 1;
    }

    for (int i = 1; i <=signals.size(); ++i) {
        if(signals[i]==1) {
            outFile <<"Day "<< i << ": " << "Buy" << std::endl;
        }
        else if(signals[i]==0) {
            outFile <<"Day "<< i << ": " << "Hold" << std::endl;
        }
        else {
            outFile <<"Day "<< i << ": " << "Sell" << std::endl;
        }
        
    }
    outFile.close();

    
    double capital = 100000.0; // Initial capital
    double shares = 0.0;
    for (int i = 0; i < signals.size(); ++i) {
        if (signals[i] == 1) {
            shares += capital / closeValues[i + longPeriod - 1];
            capital = 0.0;
        } else if (signals[i] == -1) {
            capital += shares * closeValues[i + longPeriod - 1];
            shares = 0.0;
        }
    }

    
    double portfolioValue = capital + shares * closeValues.back();
    std::cout << "Final portfolio value: " << portfolioValue << std::endl;

    return 0;
}
