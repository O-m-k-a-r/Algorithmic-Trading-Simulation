#Alphavantage API -> Stock -> Open, High, Low, Close, Volume
#Trading Strategies :MACS,Moving Averages, ...(Each Trading Strategy will generate some buy and sell signal)
#Based on the signals, simulate trading
#Keep track of Portfolio value
#Evaluate Performance using performance metrics such as returns, Sharpe ratio, and maximum drawdown



import streamlit as st
import json
import matplotlib.pyplot as plt
import subprocess
import visualise2
import os
import requests
import pandas as pd

def fetch_historical_data(symbol):
    api_key = 'PKC4LO8DCIVFJXA0'
    #api_key = 'ZO3VA0Z9WTI71RYL'
    url = f'https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol={symbol}&apikey={api_key}'
    response = requests.get(url)
    data = response.json()
    #print(data)
    return data

def main():
    # Define Streamlit app layout
    st.title('Algorithmic Trading Simulation')
    # Take user input for stock symbol
    symbol = st.text_input('Enter Stock Symbol', 'AAPL')
    data = fetch_historical_data(symbol)
    st.write(data)
    if 'Time Series (Daily)' not in data:
        st.error('Failed to fetch data. Please check the stock symbol.')
        return
    with  open(f'{symbol}.json', 'w') as json_file:
        json.dump(data,json_file, indent =4)
    
    # Convert data to DataFrame
    df = pd.DataFrame.from_dict(data['Time Series (Daily)'], orient='index')
    df.index = pd.to_datetime(df.index)
    df = df.astype(float)

    # Display some data
    st.write(f"Data for {symbol}:")
    st.write(df.head())

    # Load stock data
    # with open('AAPL.json', 'r') as f:
    #     stock_data = json.load(f)
    
    stock_data = [f for f in os.listdir('.') if f.endswith('.json')]

    # Check if there are any JSON files
    if not stock_data:
        st.write("No JSON files found in the current directory")
        return
    # Select stock
    stock_choice = st.selectbox('Select Stock', list(stock_data))
    st.write(stock_choice)

    # Select trading algorithm
    algorithm_choice = st.selectbox('Select Trading Algorithm', ['Bollinger Bands', 'KDJ', 'MACD', 'RSI'])
    
    # Simulate trading based on selected stock and algorithm
    if st.button('Simulate Trading'):
        # Run C++ algorithm and get output
        result = subprocess.run(['Bollinger.exe', stock_choice], capture_output=True, text=True)
        output = result.stdout
        # Process output and display results
        st.write(output)
    if st.button('Visualise'):
            plot = visualise2.visualize_stock_data()
            st.pyplot(plot)
             
main()