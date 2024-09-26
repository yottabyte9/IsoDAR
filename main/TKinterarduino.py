import tkinter as tk
import serial
import threading
import time

arduino = serial.Serial('COM7', 9600, timeout=1)  
time.sleep(2)

def set_mode(mode):
    def write_serial():
        try:
            arduino.write(f"{mode}\n".encode())  
            print(f"Mode {mode} sent to Arduino.")
        except Exception as e:
            print(f"Error: {e}")
    
    threading.Thread(target=write_serial).start() 

    current_mode.set(f"Current Mode: {mode_labels[mode]}")

serial_data = []

def read_serial():
    while True:
        try:
            if arduino.in_waiting > 0:
                line = arduino.readline().decode('utf-8').strip()
                if line:
                    serial_data.append(line)  
        except Exception as e:
            print(f"Error reading from serial: {e}")

def update_gui():
    if serial_data:
        line = serial_data.pop(0)  
        data_label.config(text=line)  
    root.after(100, update_gui)  

serial_thread = threading.Thread(target=read_serial, daemon=True)
serial_thread.start()

root = tk.Tk()
root.title("Arduino Mode Control")

mode_labels = [
    "Force Shutdown", "Leveling Mode", "Follow Mode", "Lift Mode", 
    "M1 up", "M1 stop", "M1 down", "M2 up", 
    "M2 stop", "M2 down"
]

current_mode = tk.StringVar()
current_mode.set("Leveling Mode") 

large_font = ('Arial', 20)  
button_width = 20  
button_height = 3  

left_frame = tk.Frame(root)
left_frame.pack(side=tk.LEFT, padx=20, pady=20)

for mode in range(4):
    button = tk.Button(left_frame, text=mode_labels[mode], width=button_width, height=button_height, font=large_font, command=lambda m=mode: set_mode(m))
    button.pack(pady=10)

right_frame = tk.Frame(root)
right_frame.pack(side=tk.RIGHT, padx=20, pady=20)

m1_up_button = tk.Button(right_frame, text=mode_labels[4], width=button_width, height=button_height, font=large_font, command=lambda: set_mode(4))
m1_up_button.grid(row=0, column=0, padx=10, pady=10)

m1_0_button = tk.Button(right_frame, text=mode_labels[5], width=button_width, height=button_height, font=large_font, command=lambda: set_mode(5))
m1_0_button.grid(row=1, column=0, padx=10, pady=10)

m1_down_button = tk.Button(right_frame, text=mode_labels[6], width=button_width, height=button_height, font=large_font, command=lambda: set_mode(6))
m1_down_button.grid(row=2, column=0, padx=10, pady=10)

m2_up_button = tk.Button(right_frame, text=mode_labels[7], width=button_width, height=button_height, font=large_font, command=lambda: set_mode(7))
m2_up_button.grid(row=0, column=1, padx=10, pady=10)

m2_0_button = tk.Button(right_frame, text=mode_labels[8], width=button_width, height=button_height, font=large_font, command=lambda: set_mode(8))
m2_0_button.grid(row=1, column=1, padx=10, pady=10)

m2_down_button = tk.Button(right_frame, text=mode_labels[9], width=button_width, height=button_height, font=large_font, command=lambda: set_mode(9))
m2_down_button.grid(row=2, column=1, padx=10, pady=10)

data_label = tk.Label(root, text="Waiting for data...", width=50, height=2, font=large_font)
data_label.pack(pady=20)

mode_label = tk.Label(root, textvariable=current_mode, font=large_font)
mode_label.pack(pady=20)

root.after(100, update_gui) 
root.mainloop()

arduino.close()
