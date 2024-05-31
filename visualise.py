import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import datetime
import json
# Read data from GOOG.json file
with open("GOOG.json", "r") as f:
    data = json.load(f)

# Extract relevant data
dates = []
closes = []
with open("signals.txt", "r") as f:
    signals = [int(line.strip().split()[1]) for line in f]

for date, values in data["Time Series (Daily)"].items():
    dates.append(datetime.datetime.strptime(date, "%Y-%m-%d"))
    closes.append(float(values["4. close"]))

# Create plot
fig, ax = plt.subplots(figsize=(12, 6))
ax.plot(dates, closes, label="Close Price", color="black")

# Add buy/sell signals
for i in range(len(signals)):
    if signals[i] == 1:
        ax.scatter(dates[i], closes[i], color='green', marker='^', label='Buy Signal')
    elif signals[i] == -1:
        ax.scatter(dates[i], closes[i], color='red', marker='v', label='Sell Signal')

# Format date on x-axis
ax.xaxis.set_major_formatter(mdates.DateFormatter("%Y-%m-%d"))
plt.xticks(rotation=45)
plt.xlabel("Date")
plt.ylabel("Close Price")
plt.title("Stock Price with Buy/Sell Signals")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()


import matplotlib.pyplot as plt
closes = []
# Read signals from the file
with open("signals.txt", "r") as file:
    lines = file.readlines()

with open("GOOG.json", "r") as f:
    data = json.load(f)

for date,values in data["Time Series (Daily)"].items():
    closes.append(float(values['4. close']))

# Extract days and signals
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
        plt.scatter(day, closes[day - 1], color='green', label='Buy Signal', marker='^', s=100)
    elif signal == 'Sell':
        plt.scatter(day, closes[day - 1], color='red', label='Sell Signal', marker='v', s=100)
plt.xlabel('Days')
plt.ylabel('Stock Price')
plt.title('Stock Price with Buy/Sell Signals')
plt.legend()
plt.grid(True)
plt.show()
