# -*- coding: utf-8 -*-
"""
Created on Tue Feb 20 22:08:34 2024

@author: reeda
"""

import csv

# Open the CSV file in read mode
with open('setpoints.csv', 'r') as csvfile:
    # Create a CSV reader object
    reader = csv.reader(csvfile)
    
    # Read the first row
    first_row = next(reader)
    
    # Take the first 5 values
    first_5_values = [float(entry) for entry in first_row[:5]]

print(first_5_values)