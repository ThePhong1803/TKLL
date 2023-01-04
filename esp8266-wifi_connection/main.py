# main.py -- put your code here!
import dht
import time
import ntptime
import sys
from machine import Pin
from machine import UART
import network


d = dht.DHT11(Pin(2))
uart = UART(0, 9600)
SSID = "SSID"
PASS = "12345678"
wifi = None
day_of_week = (2,3,4,5,6,7,1)
msg = b''

def init():
    global d, uart
    d = dht.DHT11(Pin(2))
    uart = UART(0, 9600)
    uart.init(9600, 8, None, 1)
    time.sleep(2)

#networking interface
def wifi_connect():
    global wifi
    wifi = network.WLAN(network.STA_IF)
    wifi.active(True)
    wifi.connect(SSID, PASS)
    while not wifi.isconnected():
        pass



def read_dht():
    global d
    while True:
        try:
            d.measure()
            return "!DHT:" + str(d.temperature()) + str(d.humidity()) + "#"
        except:
            pass

def getntptime():
    while True:
        try:
            ntptime.settime()
            return
        except:
            time.sleep(1)

def send_time():
    getntptime()
    actual_time = time.localtime(time.time() + 7 * 3600)
    return "!TIME:%02d%02d%02d#"%(actual_time[3], actual_time[4], actual_time[5])

def send_date():
    getntptime()
    actual_time = time.localtime(time.time() + 7 * 3600)
    return "!DATE:%02d%02d%02d%d#"%(actual_time[0] % 100, actual_time[1], actual_time[2], day_of_week[actual_time[6]]) 

def main():
    init()
    wifi_connect()
    global uart, msg
    while True:
        try:
            msg =  sys.stdin.readline()
            if "!TIME#" in msg:
                uart.write(send_time())
                time.sleep(1)
                uart.write(send_date())
            if "!DHT#" in msg:
                uart.write(read_dht())
        except:
            init()
            wifi_connect()


main()