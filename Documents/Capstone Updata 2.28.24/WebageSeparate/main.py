
import multiprocessing
import serial
import time
import smtplib
import csv
from email.message import EmailMessage
import os 
from datetime import datetime, timedelta
#import matplotlib.pyplot as plt---===
import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot as plt
import pandas as pd
from io import BytesIO
import base64
from flask import Flask, render_template, request, redirect
import csv


def run_file(file_name):
    exec(open(file_name).read())

if __name__ == "__main__":
    file_names = ["WebsiteV3.py", "RPICodeSaveandSet.py"]  # List of Python files to run---
    
    processes = []
    for file_name in file_names:
        process = multiprocessing.Process(target=run_file, args=(file_name,))
        processes.append(process)
        process.start()

    for process in processes:
        process.join()
