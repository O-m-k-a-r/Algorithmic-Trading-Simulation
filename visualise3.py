import matplotlib.pyplot as plt
import numpy as np
import json
closes = []
with open("GOOG.json", "r") as f:
    data = json.load(f)

for date,values in data["Time Series (Daily)"].items():
    closes.append(float(values['4. close']))
# Read RSI values and signals from the file
rsi_signals = []
with open('signals_rsi.txt', 'r') as file:
    for line in file:
        parts = line.strip().split(',')
        day = int(parts[0].split(':')[0].split()[1])
        rsi = float(parts[0].split('=')[1])
        signal = parts[1].split('=')[1].strip() if len(parts) > 1 else None
        rsi_signals.append((day, rsi, signal))
# Plot close price vs days
plt.figure(figsize=(14, 7))
plt.plot(range(1, len(rsi_signals)+1), closes, label='Close Price', color='b')

# Highlight Buy signals
for day, rsi, signal in rsi_signals:
    if signal == ' Buy':
        plt.scatter(day, closes[day-1], color='g', label='Buy Signal')

plt.xlabel('Days')
plt.ylabel('Close Price')
plt.title('Close Price vs Days with Buy Signals')
plt.legend()
plt.grid(True)
plt.show()
