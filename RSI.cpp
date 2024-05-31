#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include "rapidjson-master/include/rapidjson/document.h"
#include "rapidjson-master/include/rapidjson/filereadstream.h"
using namespace rapidjson;
// Function to calculate the Relative Strength Index (RSI) and generate buy/sell signals
std::vector<std::pair<double, std::string>> calculateRSI(const std::vector<double>& closeValues, int period) {
    std::vector<std::pair<double, std::string>> rsiSignals;

    std::vector<double> changes;
    for (int i = 1; i < closeValues.size(); ++i) {
        changes.push_back(closeValues[i] - closeValues[i - 1]);
    }

    std::vector<double> gains;
    std::vector<double> losses;
    for (int i = 0; i < changes.size(); ++i) {
        if (changes[i] > 0) {
            gains.push_back(changes[i]);
            losses.push_back(0);
        } else {
            gains.push_back(0);
            losses.push_back(-changes[i]);
        }
    }

    double avgGain = 0.0;
    double avgLoss = 0.0;
    for (int i = 0; i < period; ++i) {
        avgGain += gains[i];
        avgLoss += losses[i];
    }
    avgGain /= period;
    avgLoss /= period;

    double rs = avgGain / avgLoss;
    double rsi = 100.0 - (100.0 / (1.0 + rs));
    rsiSignals.push_back(std::make_pair(rsi, ""));

    for (int i = period; i < closeValues.size(); ++i) {
        avgGain = ((avgGain * (period - 1)) + (gains[i])) / period;
        avgLoss = ((avgLoss * (period - 1)) + (losses[i])) / period;
        rs = avgGain / avgLoss;
        rsi = 100.0 - (100.0 / (1.0 + rs));

        // Generate buy/sell signals
        if (rsi > 70) {
            rsiSignals.push_back(std::make_pair(rsi, "Sell"));
        } else if (rsi < 30) {
            rsiSignals.push_back(std::make_pair(rsi, "Buy"));
        } else {
            rsiSignals.push_back(std::make_pair(rsi, "Hold"));
        }
    }

    return rsiSignals;
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
    
    std::vector<std::pair<double, std::string>> rsiSignals = calculateRSI(closeValues, 14);

    
    for (int i = 0; i < rsiSignals.size(); ++i) {
        std::cout << "Day " << i + 1 << ": RSI = " << rsiSignals[i].first;
        if (!rsiSignals[i].second.empty()) {
            std::cout << ", Signal = " << rsiSignals[i].second;
        }
        std::cout << std::endl;
    }

    
    std::ofstream outFile("signals_rsi.txt");

    
    if (!outFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    
    for (int i = 1; i <= rsiSignals.size(); ++i) {
        outFile << "Day " << i<<": ";
        if (!rsiSignals[i].second.empty()) {
            outFile <<rsiSignals[i].second;
        }
        outFile << std::endl;
    }

    
    outFile.close();
    return 0;
}
