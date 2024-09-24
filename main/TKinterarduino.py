import tkinter as tk
import serial
import threading
import time

# Setup serial communication with Arduino 
arduino = serial.Serial('COM7', 9600, timeout=1)  # Update 'COM7' to your port
time.sleep(2)

# Function to send mode commands to Arduino
def set_mode(mode):
    def write_serial():
        try:
            arduino.write(f"{mode}\n".encode())
            print(f"Mode {mode} sent to Arduino.")
        except Exception as e:
            print(f"Error: {e}")
    
    threading.Thread(target=write_serial).start()  # Run serial writing in a thread

# Thread-safe variable to hold incoming serial data
serial_data = []

# Function to read incoming serial data continuously in a separate thread
def read_serial():
    while True:
        try:
            if arduino.in_waiting > 0:
                line = arduino.readline().decode('utf-8').strip()
                if line:
                    serial_data.append(line)  # Append to thread-safe buffer
        except Exception as e:
            print(f"Error reading from serial: {e}")

# Function to update the GUI with new serial data
def update_gui():
    if serial_data:
        line = serial_data.pop(0)  # Pop the oldest line from the buffer
        data_label.config(text=line)  # Update the label with the latest data
    root.after(100, update_gui)  # Schedule the next update in 100ms

# Start a thread to read from the serial port
serial_thread = threading.Thread(target=read_serial, daemon=True)
serial_thread.start()

# Create the main window
root = tk.Tk()
root.title("Arduino Mode Control")

# Create buttons for modes 0 to 4
buttons = []
for mode in range(5):
    button = tk.Button(root, text=f"Mode {mode}", width=20, height=2, command=lambda m=mode: set_mode(m))
    button.pack(pady=10)
    buttons.append(button)

# Create a label to display the serial data from Arduino
data_label = tk.Label(root, text="Waiting for data...", width=50, height=2)
data_label.pack(pady=20)

# Start the GUI update loop
root.after(100, update_gui)  # Schedule the first update in 100ms
root.mainloop()

# Close the serial connection when the GUI closes
arduino.close()
