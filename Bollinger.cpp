#include <iostream>
#include <vector>
#include <cmath>
#include <filesystem>
#include <fstream>
#include "rapidjson-master/include/rapidjson/document.h"
#include "rapidjson-master/include/rapidjson/filereadstream.h"
using namespace rapidjson;
namespace fs = std::filesystem;
double calculateSMA(const std::vector<double>& prices, int period) {
    double sum = 0.0;
    for (int i = prices.size() - period; i < prices.size(); ++i) {
        sum += prices[i];
    }
    return sum / period;
}


double calculateSD(const std::vector<double>& prices, int period) {
    double mean = calculateSMA(prices, period);
    double sum = 0.0;
    for (int i = prices.size() - period; i < prices.size(); ++i) {
        sum += pow(prices[i] - mean, 2);
    }
    return sqrt(sum / period);
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <selected_file.json>" << std::endl;
        return 1;
    }
    std::string selectedFile = argv[1];
    std::cout <<selectedFile;
    FILE* file = fopen(selectedFile.c_str(), "r");
    if (!file) {
        std::cerr << "Failed to open " <<selectedFile<< std::endl;
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


    
    int period = 20; // Number of days for moving average and standard deviation
    double numSD = 2.0; // Number of standard deviations for the bands

    
    std::vector<double> smaValues;
    std::vector<double> sdValues;
    for (int i = period - 1; i < closeValues.size(); ++i) {
        double sma = calculateSMA(closeValues, i + 1);
        double sd = calculateSD(closeValues, i + 1);
        smaValues.push_back(sma);
        sdValues.push_back(sd);
    }

    
    std::vector<double> upperBands;
    std::vector<double> lowerBands;
    for (int i = 0; i < smaValues.size(); ++i) {
        double upperBand = smaValues[i] + numSD * sdValues[i];
        double lowerBand = smaValues[i] - numSD * sdValues[i];
        upperBands.push_back(upperBand);
        lowerBands.push_back(lowerBand);
    }

    
    for (int i = 0; i < upperBands.size(); ++i) {
        std::cout << "Day " << i + 1 << ": Upper Band = " << upperBands[i] << ", Lower Band = " << lowerBands[i] << std::endl;
    }

    
    std::vector<std::string> signals;
    for (int i = 0; i < closeValues.size(); ++i) {
        if (closeValues[i] > upperBands[i]) {
            signals.push_back("Sell");
        } else if (closeValues[i] < lowerBands[i]) {
            signals.push_back("Buy");
        } else {
            signals.push_back("Hold");
        }
    }

    
    for (int i = 0; i < signals.size(); ++i) {
        std::cout << "Day " << i + 1 << ": " << signals[i] << std::endl;
    }

    std::ofstream outFile("signals_bollinger.txt");
    if (outFile.is_open()) {
        for (int i = 0; i < signals.size(); ++i) {
            outFile << "Day " << i + 1 << ": " << signals[i] << std::endl;
        }
        outFile.close();
    } else {
        std::cerr << "Failed to open signals_bollinger.txt for writing" << std::endl;
        return 1;
    }

    return 0;
}