import tkinter as tk
from PIL import Image, ImageTk
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

    if mode == 0:
        root.config(bg='red')  
        bg_label.place_forget() 
        current_mode.set("Force Shutdown")
    elif mode in [4, 5, 6, 7, 8, 9]:
        current_mode.set("Manual Mode")
    else:
        current_mode.set(mode_labels[mode])

serial_data = []

def read_serial():
    while True:
        try:
            if arduino.in_waiting > 0:
                line = arduino.readline().decode('utf-8').strip()  
                if line:
                    serial_data.append(line)
                    print(f"Line received: {line}")  
        except Exception as e:
            print(f"Error reading from serial: {e}")

def update_gui():
    if serial_data:
        line = serial_data.pop(0)  
        data_label.config(text=line) 
        
        if "M1 Speed: " in line:
            m1_speed_value = line[len("M1 Speed: "):]
            m1_speed.set(m1_speed_value)

        elif "M2 Speed: " in line:
            m2_speed_value = line[len("M2 Speed: "):]
            m2_speed.set(m2_speed_value)

        elif "Strain: " in line:
            strain_value = line[len("Strain: "):]
            strain.set(strain_value)
        elif "Tilt: " in line:
            tilt_value = line[len("Tilt: "):]
            tilt.set(tilt_value)
    
    root.after(10, update_gui)  

serial_thread = threading.Thread(target=read_serial, daemon=True)
serial_thread.start()

root = tk.Tk()
root.title("Arduino Mode Control")
root.geometry("1920x1080")

image = Image.open("SciPhyUmichBkg.jpg")
bg_image = ImageTk.PhotoImage(image)
bg_label = tk.Label(root, image=bg_image)
bg_label.place(x=0, y=0, relwidth=1, relheight=1)  

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

data_label = tk.Label(root, text="Waiting for data...", width=50, height=2, font=large_font, bg='#f0f0f0')
data_label.pack(pady=10)
mode_label = tk.Label(root, textvariable=current_mode, font=large_font, bg='#f0f0f0')
mode_label.pack(pady=10)  


left_frame = tk.Frame(root)
left_frame.place(x=20, y=150)  

for mode in range(4):
    button = tk.Button(left_frame, text=mode_labels[mode], width=button_width, height=button_height, font=large_font, command=lambda m=mode: set_mode(m))
    button.pack(pady=10)

right_frame = tk.Frame(root)
right_frame.place(x=1000, y=150)  

m1_up_button = tk.Button(right_frame, text=mode_labels[4], width=10, height=2, font=large_font, command=lambda: set_mode(4))
m1_up_button.grid(row=0, column=0, padx=10, pady=10)

m1_0_button = tk.Button(right_frame, text=mode_labels[5], width=10, height=2, font=large_font, command=lambda: set_mode(5))
m1_0_button.grid(row=1, column=0, padx=10, pady=10)

m1_down_button = tk.Button(right_frame, text=mode_labels[6], width=10, height=2, font=large_font, command=lambda: set_mode(6))
m1_down_button.grid(row=2, column=0, padx=10, pady=10)

m2_up_button = tk.Button(right_frame, text=mode_labels[7], width=10, height=2, font=large_font, command=lambda: set_mode(7))
m2_up_button.grid(row=0, column=1, padx=10, pady=10)

m2_0_button = tk.Button(right_frame, text=mode_labels[8], width=10, height=2, font=large_font, command=lambda: set_mode(8))
m2_0_button.grid(row=1, column=1, padx=10, pady=10)

m2_down_button = tk.Button(right_frame, text=mode_labels[9], width=10, height=2, font=large_font, command=lambda: set_mode(9))
m2_down_button.grid(row=2, column=1, padx=10, pady=10)

m1_speed = tk.StringVar()
m1_speed.set("0")  
m2_speed = tk.StringVar()
m2_speed.set("0") 
strain = tk.StringVar()
strain.set("0") 
tilt = tk.StringVar()
tilt.set("0") 

tilt_label = tk.Label(root, text="Tilt:", font=large_font)
tilt_label.place(x=500, y=500)
tilt_entry = tk.Entry(root, textvariable=tilt, font=large_font, width=15)
tilt_entry.place(x=700, y=500)

strain_label = tk.Label(root, text="Pressure:", font=large_font)
strain_label.place(x=500, y=550)
strain_entry = tk.Entry(root, textvariable=strain, font=large_font, width=15)
strain_entry.place(x=700, y=550)

m1_speed_label = tk.Label(root, text="M1 Speed:", font=large_font)
m1_speed_label.place(x=500, y=600)
m1_speed_entry = tk.Entry(root, textvariable=m1_speed, font=large_font, width=15)
m1_speed_entry.place(x=700, y=600)

m2_speed_label = tk.Label(root, text="M2 Speed:", font=large_font)
m2_speed_label.place(x=500, y=650)
m2_speed_entry = tk.Entry(root, textvariable=m2_speed, font=large_font, width=15)
m2_speed_entry.place(x=700, y=650)

root.after(10, update_gui)
root.mainloop()

arduino.close()
