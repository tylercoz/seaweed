import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot as plt
import pandas as pd
from io import BytesIO
import base64
from flask import Flask, render_template, request, redirect
import csv
from datetime import datetime, timedelta

app = Flask(__name__)

def read_and_process_data():
    # Read the CSV file
    df = pd.read_csv('data.csv', header=None, names=['PH', 'Conductivity(uS/cm)', 'TDS', 'Salinity(ppt)', 'SpecificGravity', 'WaterTemp(C)', 'EnvTemp(C)', 'Humidity(%)', 'Timestamp'])

    # Convert timestamp to datetime format
    df['Timestamp'] = pd.to_datetime(df['Timestamp'], format='%Y-%m-%d %H:%M:%S')

    # Sort data by timestamp
    df.sort_values(by='Timestamp', inplace=True)
    
    # Filter data for the last 24 hours
    last_24_hours = df[df['Timestamp'] > datetime.now() - timedelta(hours=24)]
    
    return last_24_hours

def update_plots():
    # Read and process data
    last_24_hours = read_and_process_data()

    # Columns to include in the plot   
    columns_to_plot = ['PH', 'Salinity(ppt)', 'WaterTemp(C)', 'EnvTemp(C)', 'Humidity(%)']
    
    # Read setpoints
    with open('setpoints.csv', 'r') as csvfile:
        reader = csv.reader(csvfile)
        first_row = next(reader)
        first_5_values = [float(entry) for entry in first_row[:5]]
    
    # Render plots for each element---
    plot_urls = {}
    for column in columns_to_plot:  # Plot only selected columns
        if column not in last_24_hours.columns:
            continue  # Skip if column not in DataFrame

        plt.figure(figsize=(23, 6))  # Adjust figure size as needed
        plt.plot(last_24_hours['Timestamp'], last_24_hours[column])
        plt.xlabel('Date(Day-Time)')
        plt.ylabel(column)
        plt.grid()
        plt.xticks(rotation=45)
        plt.axhline(y=first_5_values[i], color='r', linestyle='--', label='Setpoint')

        plt.legend()
        plt.tight_layout()
        
        img = BytesIO()
        plt.savefig(img, format='png')
        img.seek(0)
        plot_urls[column] = base64.b64encode(img.getvalue()).decode()
        plt.close()

    return plot_urls

@app.route('/')
def index():
    # Read and process data
    last_24_hours = read_and_process_data()
    
    # Render table
    table_html = last_24_hours.to_html(index=False)
    
    # Render plots
    plot_urls = update_plots()

    return render_template('index1.html', plot_urls=plot_urls, table=table_html)

@app.route('/setpoints', methods=['POST'])
def setpoints():
    # Get setpoint values from the form
    ph = request.form['ph']
    salinity = request.form['salinity']
    water_temp = request.form['water_temp']
    env_temp = request.form['env_temp']
    humidity = request.form['humidity']
    nutrients_time = request.form['nutrients_time']
    light_on_time = request.form['light_on_time']
    light_off_time = request.form['light_off_time']
    filter_on_time = request.form['filter_on_time']

    # Save setpoint values to a CSV file     
    with open('setpoints.csv', 'r') as csvfile:
        reader = csv.reader(csvfile)
        rows = list(reader)
    with open('setpoints.csv', 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow([ph, salinity, water_temp, env_temp, humidity, nutrients_time, light_on_time, light_off_time, filter_on_time])
        for row in rows[1:]:
            writer.writerow(row)
    
    return redirect('/')

if __name__ == '__main__':
    app.run(debug=True)
