#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

std::vector<std::string> readSignals(const std::string& filename) {
    std::vector<std::string> signals;
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            signals.push_back(line.substr(line.find(":") + 2)); // Extract the signal after ": "
        }
        file.close();
    }
    return signals;
}

std::vector<std::string> mergeSignals(const std::vector<std::vector<std::string>>& allSignals) {
    std::vector<std::string> mergedSignals;
    for (int i = 0; i < allSignals[0].size(); ++i) {
        int buyCount = 0;
        int sellCount = 0;
        for (const auto& signals : allSignals) {
            if (signals[i] == "Buy") {
                buyCount++;
            } else if (signals[i] == "Sell") {
                sellCount++;
            }
        }
        if (buyCount > sellCount) {
            mergedSignals.push_back("Buy");
        } else if (sellCount > buyCount) {
            mergedSignals.push_back("Sell");
        } else {
            
            mergedSignals.push_back("Buy");
        }
    }
    return mergedSignals;
}


std::vector<std::string> applyDivideAndConquer(const std::vector<std::string>& mergedSignals, int segmentSize) {
    std::vector<std::string> finalSignals;
    for (int i = 0; i < mergedSignals.size(); i += segmentSize) {
        // Apply your Divide and Conquer approach to the merged signals
        std::vector<std::string> segment(mergedSignals.begin() + i, mergedSignals.begin() + std::min<int>(i + segmentSize, mergedSignals.size()));
        int buyCount = std::count(segment.begin(), segment.end(), "Buy");
        int sellCount = std::count(segment.begin(), segment.end(), "Sell");
        finalSignals.push_back(buyCount > sellCount ? "Buy" : "Sell");
    }
    return finalSignals;
}

int main() {
    
    std::vector<std::string> bollingerSignals = readSignals("signals_bollinger.txt");
    std::vector<std::string> kdjSignals = readSignals("signals_KDJ.txt");
    std::vector<std::string> rsiSignals = readSignals("signals_rsi.txt");
    std::vector<std::string> sampleSignals = readSignals("signals.txt");

    
    std::vector<std::vector<std::string>> allSignals = {bollingerSignals, kdjSignals, rsiSignals, sampleSignals};
    std::vector<std::string> mergedSignals = mergeSignals(allSignals);

    
    int segmentSize = 5; // Set segment size
    std::vector<std::string> finalSignals = applyDivideAndConquer(mergedSignals, segmentSize);
    

    
    for (int i = 0; i < finalSignals.size(); ++i) {
        std::cout << "Segment " << i + 1 << ": " << finalSignals[i] << std::endl;
    }

    return 0;
}
