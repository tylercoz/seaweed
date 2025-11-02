# -*- coding: utf-8 -*-
"""
Created on Tue Feb 20 20:20:27 2024
@author: reeda a
"""
#import matplotlib.pyplot as plt--
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
@app.route('/')
def index():
# Read the CSV file------
df = pd.read_csv('data.csv', header=None, names=['PH', 'Conductivity(uS/cm)',
'TDS', 'Salinity(ppt)', 'SpecificGravity', 'WaterTemp(C)', 'EnvTemp(C)',
'Humidity(%)', 'Timestamp'])
# Convert timestamp to datetime format
df['Timestamp'] = pd.to_datetime(df['Timestamp'], format='%Y-%m-%d %H:%M:%S')
# Filter data for the last 24 hours
last_24_hours = df[df['Timestamp'] > datetime.now() - timedelta(hours=24)]
# Columns to include in the plot
columns_to_plot = ['PH', 'Salinity(ppt)', 'WaterTemp(C)', 'EnvTemp(C)',
'Humidity(%)']
with open('setpoints.csv', 'r') as csvfile:
# Create a CSV reader object
reader = csv.reader(csvfile)
# Read the first row--
first_row = next(reader)
# Take the first 5 values
first_5_values = [float(entry) for entry in first_row[:5]]
i = 0
# Create plots for each element---
plot_urls = {}
for column in columns_to_plot: # Plot only selected columns
if column not in last_24_hours.columns:
continue # Skip if column not in DataFrame
plt.figure(figsize=(23, 6)) # Adjust figure size as needed
plt.plot(last_24_hours['Timestamp'], last_24_hours[column])
#plt.title(column) # Set title for each plot
plt.xlabel('Date(Day-Time)')
plt.ylabel(column)
plt.grid()
plt.xticks(rotation=45)
plt.axhline(y= first_5_values[i], color='r', linestyle='--',
label='Setpoint')
i = i + 1
plt.legend()
plt.tight_layout()
img = BytesIO()
plt.savefig(img, format='png')
img.seek(0)
plot_urls[column] = base64.b64encode(img.getvalue()).decode()
plt.close()
# Create table---
table_html = last_24_hours.to_html(index=False)
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
writer.writerow([ph, salinity, water_temp, env_temp, humidity,
nutrients_time, light_on_time, light_off_time, filter_on_time])
for row in rows[1:]:
writer.writerow(row)
return redirect('/')
if __name__ == '__main__':
app.run(debug=True)
