#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <algorithm>
#include "rapidjson-master/include/rapidjson/document.h"
#include "rapidjson-master/include/rapidjson/filereadstream.h"
using namespace rapidjson;
#include <iostream>
#include <vector>
std::pair<double, double> calculateStochasticOscillator(const std::vector<double>& close,
                                                        const std::vector<double>& high,
                                                        const std::vector<double>& low,
                                                        int period) {
    int n = close.size();
    double highestHigh = 0.0, lowestLow = 0.0;
    
    for (int i = n - period; i < n; ++i) {
        if (i == n - period) {
            highestHigh = high[i];
            lowestLow = low[i];
        } else {
            if (high[i] > highestHigh) highestHigh = high[i];
            if (low[i] < lowestLow) lowestLow = low[i];
        }
    }
    
    double stochasticK = ((close[n - 1] - lowestLow) / (highestHigh - lowestLow)) * 100.0;
    
    double sum = 0.0;
    for (int i = n - period; i < n; ++i) {
        sum += ((close[i] - lowestLow) / (highestHigh - lowestLow)) * 100.0;
    }
    double stochasticD = sum / period;
    
    return std::make_pair(stochasticK, stochasticD);
}

void generateSignals(const std::vector<double>& close,
                     const std::vector<double>& high,
                     const std::vector<double>& low,
                     int period,
                     double buyThreshold,
                     double sellThreshold) {
    std::pair<double, double> stochasticOscillator = calculateStochasticOscillator(close, high, low, period);
    double stochasticK = stochasticOscillator.first;
    double stochasticD = stochasticOscillator.second;
    
    std::cout << "%K: " << stochasticK << ", %D: " << stochasticD << std::endl;
    std::cout << "Buy Threshold: " << buyThreshold << ", Sell Threshold: " << sellThreshold << std::endl;
    
    if (stochasticK > stochasticD && stochasticK < buyThreshold) {
        std::cout << "Buy Signal Generated" << std::endl;
    } else if (stochasticK < stochasticD && stochasticK > sellThreshold) {
        std::cout << "Sell Signal Generated" << std::endl;
    } else {
        std::cout << "No Signal Generated" << std::endl;
    }
}
void extractPrices(const Value& document, std::vector<double>& closes, std::vector<double>& highs, std::vector<double>& lows) {
    for (Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr) {
        const Value& value = itr->value;
        closes.push_back(std::stod(value["4. close"].GetString()));
        highs.push_back(std::stod(value["2. high"].GetString()));
        lows.push_back(std::stod(value["3. low"].GetString()));
    }
}


int main()
{
    FILE* file = fopen("AAPL.json", "r");
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

    std::vector<double> closes;
    std::vector<double> highs;
    std::vector<double> lows;
    int period = 14;
    extractPrices(document["Time Series (Daily)"], closes, highs, lows);
    double buyThreshold = 20.0; // Buy threshold for stochastic oscillator
    double sellThreshold = 80.0; // Sell threshold for stochastic oscillator
    
    generateSignals(closes, highs, lows, period, buyThreshold, sellThreshold);

    

}