import matplotlib.pyplot as plt
import json

def visualize_stock_data():
    # Read signals from the file
    with open("signals_bollinger.txt", "r") as file:
        lines = file.readlines()

    with open("GOOG.json", "r") as f:
        data = json.load(f)

    closes = []
    for date, values in data["Time Series (Daily)"].items():
        closes.append(float(values['4. close']))

    days = []
    signals = []
    for line in lines:
        parts = line.split(":")
        days.append(int(parts[0].split()[1]))
        signals.append(parts[1].strip())

    # Plot the stock price along with the signals
    plt.figure(figsize=(14, 7))
    plt.plot(range(1, len(closes) + 1), closes, label='Stock Price', color='blue')
    for day, signal in zip(days, signals):
        if signal == 'Buy':
            plt.scatter(day, closes[day - 1], color='green', marker='^', s=100)
        elif signal == 'Sell':
            plt.scatter(day, closes[day - 1], color='red', marker='v', s=100)
    plt.xlabel('Days')
    plt.ylabel('Stock Price')
    plt.title('Stock Price with Buy/Sell Signals')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    #plt.show()
    # Display the plot in Streamlit
    return plt

visualize_stock_data()