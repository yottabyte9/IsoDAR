import threading
import tkinter as tk
import serial
import time

arduino = serial.Serial('COM7', 9600, timeout=2)
time.sleep(2)

def set_mode(mode):
    def write_serial():
        try:
            arduino.write(f"{mode}\n".encode())
            print(f"Mode {mode} sent to Arduino.")
        except Exception as e:
            print(f"Error: {e}")
    
    # Run the serial write in a new thread
    threading.Thread(target=write_serial).start()

# Create the main window
root = tk.Tk()
root.title("Arduino Mode Control")

buttons = []
for mode in range(5):
    button = tk.Button(root, text=f"Mode {mode}", width=20, height=2, command=lambda m=mode: set_mode(m))
    button.pack(pady=10)
    buttons.append(button)

# Start the Tkinter main loop
root.mainloop()

arduino.close()
