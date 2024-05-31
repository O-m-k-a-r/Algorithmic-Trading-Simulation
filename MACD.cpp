#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <algorithm>
#include "rapidjson-master/include/rapidjson/document.h"
#include "rapidjson-master/include/rapidjson/filereadstream.h"
using namespace rapidjson;

std::vector<double> calculateEMA(const std::vector<double>& data, int period) {
    std::vector<double> emaValues;
    double multiplier = 2.0 / (period + 1);
    double ema = data[0]; // EMA for the first data point is the same as the data point itself
    emaValues.push_back(ema);
    for (int i = 1; i < data.size(); ++i) {
        ema = (data[i] - ema) * multiplier + ema;
        emaValues.push_back(ema);
    }
    return emaValues;
}

std::vector<double> calculateMACDLine(const std::vector<double>& closeValues, int shortEMA, int longEMA) {
    std::vector<double> shortEMAValues = calculateEMA(closeValues, shortEMA);
    std::vector<double> longEMAValues = calculateEMA(closeValues, longEMA);

    std::vector<double> macdLine;
    for (int i = 0; i < longEMAValues.size(); ++i) {
        macdLine.push_back(shortEMAValues[i] - longEMAValues[i]);
    }
    return macdLine;
}


std::vector<double> calculateSignalLine(const std::vector<double>& macdLine, int signalEMA) {
    return calculateEMA(macdLine, signalEMA);
}


std::vector<double> calculateMACDHistogram(const std::vector<double>& macdLine, const std::vector<double>& signalLine) {
    std::vector<double> macdHistogram;
    for (int i = 0; i < macdLine.size(); ++i) {
        macdHistogram.push_back(macdLine[i] - signalLine[i]);
    }
    return macdHistogram;
}
std::vector<std::string> generateSignals(const std::vector<double>& macdLine, const std::vector<double>& signalLine) {
    std::vector<std::string> signals(macdLine.size(), "None");
    for (int i = 1; i < macdLine.size(); ++i) {
        if (macdLine[i] > signalLine[i] && macdLine[i - 1] <= signalLine[i - 1]) {
            signals[i] = "Buy";
        } else if (macdLine[i] < signalLine[i] && macdLine[i - 1] >= signalLine[i - 1]) {
            signals[i] = "Sell";
        }
    }
    return signals;
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

    
    int shortEMA = 12;
    int longEMA = 26;
    int signalEMA = 9;

    
    std::vector<double> macdLine = calculateMACDLine(closeValues, shortEMA, longEMA);

    
    std::vector<double> signalLine = calculateSignalLine(macdLine, signalEMA);

    
    std::vector<double> macdHistogram = calculateMACDHistogram(macdLine, signalLine);

    
    for (int i = 0; i < macdHistogram.size(); ++i) {
        std::cout << "Day " << i + longEMA + signalEMA + 1 << ": MACD Histogram = " << macdHistogram[i] << std::endl;
    }

    

    
    std::vector<std::string> signals = generateSignals(macdLine, signalLine);
    for (int i = 0; i < signals.size(); ++i) {
        if (signals[i] != "None") {
            std::cout << "Day " << i + longEMA + signalEMA + 1 << ": " << signals[i] << " Signal" << std::endl;
        }
    }


    return 0;
}
