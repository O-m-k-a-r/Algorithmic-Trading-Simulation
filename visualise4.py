import matplotlib.pyplot as plt
import json
# Read signals from signals_KDJ.txt
with open('signals_KDJ.txt', 'r') as f:
    signals = f.readlines()

open_prices = []  # Replace ... with your actual data
close_prices = []
high_prices = []
low_prices = []
volumes = []

with open("GOOG.json", "r") as f:
    data = json.load(f)
for date,values in data["Time Series (Daily)"].items():
    close_prices.append(float(values['4. close']))
    
# Initialize lists to store buy and sell points
buy_points = []
sell_points = []

# Simulate trading and identify buy and sell points
for day, signal in enumerate(signals, start=1):
    action = signal.strip().split(': ')[1]
    if action == 'Buy':
        buy_points.append(day)
    elif action == 'Sell':
        sell_points.append(day)

# Plot the stock price
plt.figure(figsize=(14, 7))
plt.plot(close_prices, label='Close Price', color='b')

# Mark buy and sell points on the plot
plt.scatter(buy_points, [close_prices[i-1] for i in buy_points], color='g', marker='^')
plt.scatter(sell_points, [close_prices[i-1] for i in sell_points], color='r', marker='v')

# Add labels and title
plt.xlabel('Days')
plt.ylabel('Stock Price')
plt.title('Stock Price with Buy and Sell Signals')
plt.legend()
plt.grid(True)
plt.show()    
    

# Initialize variables
portfolio = 10000  # Initial portfolio value
shares = 0  # Initial number of shares

# Simulate trading
for day, signal in enumerate(signals, start=1):
    action = signal.strip().split(': ')[1]
    if action == 'Buy':
        shares_to_buy = portfolio // close_prices[day - 1]
        shares += shares_to_buy
        portfolio -= shares_to_buy * close_prices[day - 1]
        print(f'Day {day}: Bought {shares_to_buy} shares at price {close_prices[day - 1]}.')
    elif action == 'Sell':
        portfolio += shares * close_prices[day - 1]
        shares = 0
        print(f'Day {day}: Sold all shares at price {close_prices[day - 1]}.')

# Calculate final portfolio value
final_value = portfolio + shares * close_prices[-1]
print(f'Final portfolio value: {final_value}')
