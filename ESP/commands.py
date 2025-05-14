# commands.py
# purpose: control an ESP32 over a socket using keyboard commands
# alisa yurevich, ee14 spring 2025

import socket   #for connection
import sys      #for standard in
import termios #for terminal settings
import msvcrt   #to replace above
import tty      #raw input mode
import time     #time stamps
import select   #check key press w/o blocking

ESP_IP = "192.168.4.1" #default ip of the ep 32 when acting as an AP
PORT = 80 #network port num, default for tcp connections 
SEND_INTERVAL = 0.1 #send interval should not be faster than serial, or i2c transfer rate

# send_command: sends a single-character command to the ESP32 over the socket
def send_command(sock, command): #send command over socket
    try:
        sock.sendall(f"{command}\n".encode())   # send the command with newline as bytes
        print(f"sent: {command}")
    except (socket.error, BrokenPipeError) as e:
        print(f"Connection error: {e}")
        sock.close()
        return None
    return sock

# create_socket,initializes a TCP connection to the ESP32
def create_connection():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP socket
    try:
        s.connect((ESP_IP, PORT))
        print("Connected to ESP32")
        return s
    except socket.error as e:
        print(f"Failed to connect: {e}")
        return None

# is_key_pressed
def is_key_pressed():
    #checks if standard in has input waiting
    return select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], [])

# get_key : reads and returns one uppercase character from keyboard input
def get_key():
    return sys.stdin.read(1).upper()

# set_up_terminal : cbreak mode (unbuffered, char-by-char input) may not work with windows 
def setup_terminal():
    fd = sys.stdin.fileno() # file descriptor for standard in
    old_settings = termios.tcgetattr(fd)
    tty.setcbreak(sys.stdin.fileno())  # no buffering, reads single characters 
    return fd, old_settings

# reset_terminal -> back to prior state + not polling for input
def reset_terminal(fd, old_settings): 
    termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)


#--- MAIN ---#
print("Type to control the ESP32 (Ctrl+C to exit)")
sock = create_connection() # creates connection
fd, old_settings = setup_terminal() # config real time input
try:
    current_key = None
    last_send_time = 0 #time stamp of last command

    while True:
        if is_key_pressed(): #while pressed
            key = get_key()
            if key in ("W", "A", "S", "D", "E", "F", "R", "H",): 
                now = time.time()
                if now - last_send_time >= SEND_INTERVAL: #check interval
                    if sock is None:
                        sock = create_connection() # just an error check
                    if sock: 
                        sock = send_command(sock, key) # send
                        last_send_time = now
        time.sleep(0.01) #delay
except KeyboardInterrupt:
    print("\nExciting!")
finally: 
    reset_terminal(fd, old_settings)
    if sock:
        sock.close()
