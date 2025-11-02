
import serial
import time
import smtplib
import csv
from email.message import EmailMessage
import os 
from datetime import datetime, timedelta

last_error_code = "xxx"
new_error_code = "yyy"


def email_alert(subject, body, to):
     
    msg = EmailMessage()
    msg.set_content(body)
    msg['subject'] = subject
    msg['to'] = to
    
    
    print("Send Fields Complete")
    user = "aquaponicssystemalarm@gmail.com"
    msg['from'] = user
    password = "tmjhhpyypzwoakxw"
    
    
    
    server = smtplib.SMTP("smtp.gmail.com",587)
    server.starttls()
    server.login(user,password)
    server.send_message(msg)
    server.quit()
   
    print("email alert run")

def set_last_email_timestamp():
    current_time = datetime.now()
    timestamp_str = current_time.strftime("%Y-%m-%d %H:%M:%S")
    with open("/home/seaweed/Documents/Capstone Updata 2.28.24/WebageSeparate/last_email_timestamp.txt", "w") as timestamp_file:  # /home/seaweed/Documents/Capstone Updata 2.28.24/WebageSeparate
        timestamp_file.write(timestamp_str)
        
def last_timestamp(): # get_last_email_timestamp()
    print("Getting Timestamp")
    try:
        with open("/home/seaweed/Documents/Capstone Updata 2.28.24/WebageSeparate/last_email_timestamp.txt", "r") as timestamp_file:
            timestamp_str = timestamp_file.read().strip()
            return datetime.strptime(timestamp_str, "%Y-%m-%d %H:%M:%S")
    except FileNotFoundError:
        return None

def should_send_email():
    last_email_timestamp = last_timestamp()   # last_email_timestamp = get_last_email_timestamp()
    print("Recieved TimeStamp")
    time_difference = datetime.now() - last_email_timestamp
    last_error = last_error_code
    error = new_error_code
    
    if last_email_timestamp is None :
        return True
    
    elif (time_difference.total_seconds() >= (5)) & (last_error != error) : 
        # add condition above to send email if time is longer and last error is same send mail
        return True
    else :
        return False
    
    
    
def get_current_time() :
    
    time = datetime.now()
    cur_time = time.strftime("%H:%M:%S")
    day = time.strftime("%A")
    #print(time)
    
    return cur_time, day

#-------------------------------Start Here ------------------------------------------------------

ser = serial.Serial('/dev/ttyACM0', 9600, timeout = 3.0)
# leave three seconds to communicate
time.sleep(5)
# buffer to read the data from the arduino 
ser.reset_input_buffer()
#print if serial working
print("Serial OK")

# email test
#email_alert("Check It Out", "Look what I got to work!! Emailing you from python", "stevehay@oregonstate.edu")

try: #try/ except to be able to close serial
# loop forever to read data from serial
    while True:
        
        message = ser.readline().decode('utf-8').rstrip()
        print(message)
        #time1,day1 = get_current_time()
        current_times = datetime.now()
        timestamp_reading = current_times.strftime("%Y-%m-%d %H:%M:%S")
        
        
        if message:
        
            #message = ser.read(5)
            value = (message)
            recieved = str(value)
        
            letterMatch = recieved[0]
        
            if letterMatch == 'r' :   
                   
                swrite = recieved[1:]
                file = open("/home/seaweed/Documents/Capstone Updata 2.28.24/WebageSeparate/data.csv","a")
                file.write(swrite + "," + timestamp_reading + "\n")
                print("Recieved Reading Data")
                
            elif letterMatch == 'e' :    
                
                last_error_code = new_error_code
                new_error_code = recieved[:4]
                ewrite = recieved[3:] + "," + timestamp_reading #  "," + time1 + "," + day1
                file = open("/home/seaweed/Documents/Capstone Updata 2.28.24/WebageSeparate/events.txt","a") 
                file.write(ewrite + "\n")
                print("Event or error message ")
                
                # Check if email should be sent based on the time elapsed ---------------------------------------------------------------------------------------------------------
                if (last_error_code != new_error_code): #if should_send_email():
                    # Send email and set the timestamp
                    #email_alert("URGENT, System Emergency", ewrite, "conwayd@oregonstate.edu")
                    
                    set_last_email_timestamp()
                    print("Email Sent")
            
            else :
                
                print("No data recieved")
                
                
        
        
        else :
            print('No Message')
            
                
        # program for sending out setpoints via serial to arduino
        
        file_set = open("/home/seaweed/Documents/Capstone Updata 2.28.24/WebageSeparate/setpoints.csv","r")    
        setpoints = file_set.read()
        #ser.write(setpoints.encode('utf-8'))
        file_set.close
        #print('Data Sent:' + setpoints)
        
        time,day = get_current_time()
        
        time_data = time + "\n"
        #ser.write(time_data.encode('utf-8'))
        #print(time_data)
        
        #print data together to serial up to 128 characters
        
        serial_data = "s" + time_data[0:5] + "," + setpoints
        ser.write(serial_data.encode('utf-8'))
        # print(serial_data) # uncomment if want to see what data is being sent
        
        
        
        
        
        
except KeyboardInterrupt: # ctrl c to close communication
    print("Close Serial Communication")
    ser.close()
    

    
#message = ser.readline().decode('utf-8').rstrip()
#value = (message)
#string = str(value)
#file = open("/home/pi/Documents/data.txt","a")
#file.write(string + "\n")
#print("Recieved Data")


