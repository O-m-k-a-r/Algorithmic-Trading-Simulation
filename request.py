#Alphavantage API -> Stock -> Open, High, Low, Close, Volume
#Trading Strategies :MACS,Moving Averages, ...(Each Trading Strategy will generate some buy and sell signal)
#Based on the signals, simulate trading
#Keep track of Portfolio value
#Evaluate Performance using performance metrics such as returns, Sharpe ratio, and maximum drawdown


import requests
import matplotlib.pyplot as plt
import json
def fetch_historical_data(symbol):
    api_key = 'PKC4LO8DCIVFJXA0'
    #api_key = 'ZO3VA0Z9WTI71RYL'
    url = f'https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol={symbol}&apikey={api_key}'
    response = requests.get(url)
    data = response.json()
    #print(data)
    return data

symbol = 'SAIL'
data = fetch_historical_data(symbol)
with  open(f'{symbol}.json', 'w') as json_file:
    json.dump(data,json_file, indent =4)

