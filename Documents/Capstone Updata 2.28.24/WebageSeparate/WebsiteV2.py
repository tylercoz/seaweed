# -*- coding: utf-8 -*-
"""
Created on Tue Feb 20 20:20:27 2024

@author: reeda  
"""

import matplotlib.pyplot as plt
import pandas as pd
from io import BytesIO
import base64
from flask import Flask, render_template, request, redirect
import csv
from datetime import datetime, timedelta

app = Flask(__name__)

@app.route('/')
def index():
    # Read the CSV file
    df = pd.read_csv('data.csv', header=None, names=['PH', 'Conductivity(uS/cm)', 'TDS', 'Salinity(ppt)', 'SpecificGravity', 'WaterTemp(C)', 'EnvTemp(C)', 'Humidity(%)', 'Timestamp'])

    # Convert timestamp to datetime format
    df['Timestamp'] = pd.to_datetime(df['Timestamp'], format='%Y-%m-%d %H:%M:%S')

    # Filter data for the last 24 hours
    last_24_hours = df[df['Timestamp'] > datetime.now() - timedelta(hours=24)]
    

    # Render plots for each element
    plot_urls = {}
    for column in last_24_hours.columns[:-1]:  # Exclude timestamp
        plt.figure(figsize=(10, 4))  # Adjust figure size as needed
        plt.plot(last_24_hours['Timestamp'], last_24_hours[column])
        plt.title(column)  # Set title for each plot
        
        #plt.axhline(y=6, color='r', linestyle='--', label='Setpoint')
        
        plt.xlabel('Time')
        plt.ylabel('Value')
        plt.xticks(rotation=45)
        plt.legend()
        
        img = BytesIO()
        plt.savefig(img, format='png')
        img.seek(0)
        plot_url = base64.b64encode(img.getvalue()).decode()
        plt.close()
        plot_urls[column] = plot_url
        
        

    # Render table
    table_html = last_24_hours.to_html(index=False)

    return render_template('index1.html', plot_urls=plot_urls, table=table_html)

# Rest of the code for setpoints form and saving the data to CSV file...


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

 #Stop


if __name__ == '__main__':
    app.run(debug=True)