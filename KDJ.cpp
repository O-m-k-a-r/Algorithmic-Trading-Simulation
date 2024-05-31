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

void extractPrices(const Value& document, std::vector<double>& closes, std::vector<double>& highs, std::vector<double>& lows) {
    for (Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr) {
        const Value& value = itr->value;
        closes.push_back(std::stod(value["4. close"].GetString()));
        highs.push_back(std::stod(value["2. high"].GetString()));
        lows.push_back(std::stod(value["3. low"].GetString()));
    }
}


void calculateKDJ(const std::vector<double>& closeValues, const std::vector<double>& highValues, const std::vector<double>& lowValues,
                  int period, std::vector<double>& kValues, std::vector<double>& dValues, std::vector<double>& jValues) {
    for (int i = period - 1; i < closeValues.size(); ++i) {
        double highestHigh = *std::max_element(highValues.begin() + i - period + 1, highValues.begin() + i + 1);
        double lowestLow = *std::min_element(lowValues.begin() + i - period + 1, lowValues.begin() + i + 1);
        double close = closeValues[i];

        double k = 100 * (close - lowestLow) / (highestHigh - lowestLow);
        kValues.push_back(k);

        if (i >= period * 2 - 1) {
            double sumK = 0;
            for (int j = i - period + 1; j <= i; ++j) {
                sumK += kValues[j];
            }
            double d = sumK / period;
            dValues.push_back(d);
            jValues.push_back(3 * d - 2 * k);
        }
    }
}
void generateSignals(const std::vector<double>& kValues, const std::vector<double>& dValues, std::vector<std::string>& signals) {
    for (int i = 1; i < kValues.size(); ++i) {
        if (kValues[i] > dValues[i] && kValues[i - 1] <= dValues[i - 1] && kValues[i] < 20) {
            signals.push_back("Buy");
        } else if (kValues[i] < dValues[i] && kValues[i - 1] >= dValues[i - 1] && kValues[i] > 80) {
            signals.push_back("Sell");
        } else {
            signals.push_back("Hold");
        }
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
    
    extractPrices(document["Time Series (Daily)"], closes, highs, lows);

    // Parameters
    int period = 9;

    
    std::vector<double> kValues;
    std::vector<double> dValues;
    std::vector<double> jValues;
    calculateKDJ(closes, highs, lows, period, kValues, dValues, jValues);

    // Print KDJ values
    for (int i = 0; i < kValues.size(); ++i) {
        std::cout << "Day " << i + period + 2 << ": K = " << kValues[i] << ", D = " << dValues[i] << ", J = " << jValues[i] << std::endl;
    }

    std::vector<std::string> signals;
generateSignals(kValues, dValues, signals);

for (int i = 0; i < signals.size(); ++i) {
    std::cout << "Day " << i + 1 << ": " << signals[i] << std::endl;
}
std::ofstream outFile("signals_KDJ.txt");
    if (outFile.is_open()) {
        for (int i = 0; i < signals.size(); ++i) {
            outFile << "Day " << i + 1 << ": " << signals[i] << std::endl;
        }
        outFile.close();
    } else {
        std::cerr << "Failed to open signals_bollinger.txt for writing" << std::endl;
        return 1;
    }

}