import os
import zipfile
import requests
import pandas as pd
from datetime import datetime, timezone

def download_and_clean_binance_ticks():
    # Configuration
    symbol = "ETHUSDT"
    # Choose a specific historical date you want to backtest
    print("Enter date you want to retrive aggTrades from\n" \
    "Format: yyyy-mm-dd")
    target_date = input()
    
    # Binance public data vault URL structure for daily spot aggregate trades
    base_url = "https://data.binance.vision/data/spot/daily/aggTrades"
    file_name = f"{symbol}-aggTrades-{target_date}.zip"
    url = f"{base_url}/{symbol}/{file_name}" #<- need to include file_name or else you get error

    os.makedirs("./data", exist_ok=True)
    zip_path = f"./data/{file_name}"
    extracted_csv_path = f"./data/{symbol}-aggTrades-{target_date}.csv"
    final_output_path = f"./data/{symbol.lower()}_ticks_clean_{target_date}.csv"

    #Downloading the csv file
    #The csv-file will be extracted and be in the data-folder
    if not os.path.exists(final_output_path):
        print(f"Downloading historical ticks for {symbol} on {target_date}...")
        response = requests.get(url)
        
        if response.status_code != 200:
            print(f"[ERROR] Failed to download data. Status code: {response.status_code}")
            print(f"Make sure the date {target_date} exists on Binance data vision.")
            return
        
        with open(zip_path, "wb") as f:
            f.write(response.content)
        print("Download complete. Extracting archive...")

        #2. Unzip the CSV inside
        with zipfile.ZipFile(zip_path, 'r') as zip_ref:
            zip_ref.extractall("./data")
        
        #Cleanup the zip file to save space
        os.remove(zip_path)
    else:
        print("Cleaned tick file already exists. Skipping download.")

    # 3. Process and Clean with Pandas
    print("Parsing and formatting data for C++ engine")
    
    # Binance aggTrades columns: 
    # [agg_trade_id, price, quantity, first_trade_id, last_trade_id, timestamp, is_buyer_maker, was_best_price]
    col_names = [
        "agg_trade_id", "price", "quantity", "first_trade_id", 
        "last_trade_id", "timestamp", "is_buyer_maker", "was_best_price"
    ]
    
    df = pd.read_csv(extracted_csv_path, header=None, names=col_names)

    # In a matching engine context, if is_buyer_maker is True, the aggressor (taker) was a Seller.
    # Therefore, the incoming aggressive side driving the match can be mapped.
    # Let's map it cleanly to: timestamp, side, price, quantity
    df["side"] = df["is_buyer_maker"].apply(lambda x: "SELL" if x else "BUY")
    
    # Select and reorder columns required for your C++ engine ingestion loop
    clean_df = df[["timestamp", "side", "price", "quantity"]].copy()
    
    # Sort chronologically just in case
    clean_df = clean_df.sort_values("timestamp").reset_index(drop=True)

    # Save to final ultra-light format for C++
    clean_df.to_csv(final_output_path, index=False)
    
    # Remove intermediate raw unzipped file to keep workspace clean
    if os.path.exists(extracted_csv_path):
        os.remove(extracted_csv_path)

    print(f"Success! Cleaned {len(clean_df):,} ticks saved to: {final_output_path}")

if __name__ == "__main__":
    download_and_clean_binance_ticks()