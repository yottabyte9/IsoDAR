import tkinter as tk
from tkinter import messagebox
from PIL import Image, ImageTk
import serial
import serial.tools.list_ports
import threading
import time
import sys
import os
import queue

# ---------- Helpers ----------
def resource_path(relative_path):
    candidates = []
    try:
        candidates.append(os.path.join(sys._MEIPASS, relative_path))
    except Exception:
        pass
    try:
        script_dir = os.path.dirname(os.path.abspath(__file__))
        candidates.append(os.path.join(script_dir, relative_path))
    except Exception:
        pass
    candidates.append(os.path.join(os.getcwd(), relative_path))
    candidates.append(os.path.normpath(r"C:\Users\andre\Desktop\Arduino\pythongui\SciPhyUmichBkg.jpg"))
    candidates.append(relative_path)
    for p in candidates:
        if os.path.exists(p):
            return p
    return candidates[-1]

def list_serial_ports():
    return [p.device for p in serial.tools.list_ports.comports()]

# ---------- Globals ----------
arduino = None
serial_q = queue.Queue()    # incoming lines from Arduino -> GUI
command_q = queue.Queue()   # outgoing commands from GUI -> writer thread
stop_event = threading.Event()
write_lock = threading.Lock()
bg_image = None             
force_shutdown = False

# ---------- GUI setup ----------
root = tk.Tk()
root.title("Arduino Mode Control")
root.geometry("1280x800")

# load background image if available
bg_label = tk.Label(root)
try:
    img_path = resource_path("SciPhyUmichBkg.jpg")
    if os.path.exists(img_path):
        img = Image.open(img_path)
        bg_image = ImageTk.PhotoImage(img)
        bg_label = tk.Label(root, image=bg_image)
        bg_label.place(x=0, y=0, relwidth=1, relheight=1)
    else:
        print(f"Background not found (tried): {img_path}")
except Exception as e:
    print(f"Error loading background image: {e}")

mode_labels = [
    "Force Shutdown", "Leveling Mode", "Follow Mode", "Lift Mode",
    "M1 up", "M1 stop", "M1 down", "M2 up",
    "M2 stop", "M2 down"
]

current_mode = tk.StringVar(value=mode_labels[1])  # show leveling mode by default
m1_speed = tk.StringVar(value="0")
m2_speed = tk.StringVar(value="0")
strain = tk.StringVar(value="0")
tilt = tk.StringVar(value="0")

large_font = ('Arial', 16)
button_w, button_h = 18, 2

data_label = tk.Label(root, text="Waiting for data...", font=large_font, bg='#f0f0f0', width=50, height=2)
data_label.pack(pady=8)
mode_label = tk.Label(root, textvariable=current_mode, font=large_font, bg='#f0f0f0')
mode_label.pack(pady=6)

available_ports = list_serial_ports()
selected_port = tk.StringVar(value=available_ports[0] if available_ports else "")
port_menu = tk.OptionMenu(root, selected_port, *available_ports) if available_ports else tk.Label(root, text="No COM ports")
port_menu.pack(pady=6)

# ---------- Serial threads ----------
def serial_writer():
    """Dedicated writer: drain command_q and write to serial with lock."""
    while not stop_event.is_set():
        try:
            cmd = command_q.get(timeout=0.1)
        except queue.Empty:
            continue
        try:
            if arduino and arduino.is_open:
                with write_lock:
                    arduino.write(f"{cmd}\n".encode('utf-8'))
                    arduino.flush()
            # small delay
                time.sleep(0.01)
        except Exception as e:
            print(f"Writer error: {e}")
        finally:
            try:
                command_q.task_done()
            except Exception:
                pass

def serial_reader():
    """Read lines from serial and put them into serial_q."""
    while not stop_event.is_set():
        try:
            if arduino and arduino.in_waiting > 0:
                line = arduino.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    serial_q.put(line)
        except Exception as e:
            print(f"Reader error: {e}")
        time.sleep(0.01)

def start_serial_threads():
    stop_event.clear()
    t_read = threading.Thread(target=serial_reader, daemon=True)
    t_write = threading.Thread(target=serial_writer, daemon=True)
    t_read.start()
    t_write.start()

# ---------- Command API (non-blocking) ----------
def send_command(mode_int):
    """Queue a command (int or str) for the writer thread."""
    command_q.put(str(int(mode_int)))  # ensure integer string

# ---------- GUI actions ----------
def set_mode(mode):
    """Update visuals and queue mode command. Non-blocking."""
    global force_shutdown
    send_command(mode)
    if mode == 0:
        force_shutdown = True
        try:
            bg_label.place_forget()
        except Exception:
            pass
        root.config(bg='red')
        current_mode.set(mode_labels[0])
    else:
        force_shutdown = False
        try:
            if bg_image:
                bg_label.place(x=0, y=0, relwidth=1, relheight=1)
        except Exception:
            pass
        root.config(bg='#f0f0f0')
        current_mode.set(mode_labels[mode] if mode < len(mode_labels) else f"Mode {mode}")

def connect_to_arduino():
    global arduino
    port = selected_port.get()
    if not port:
        messagebox.showerror("Error", "No COM port selected")
        return
    try:
        arduino = serial.Serial(port, 9600, timeout=1)
        time.sleep(2)  # allow Arduino reset
    except Exception as e:
        messagebox.showerror("Connection Error", f"Failed to open {port}: {e}")
        return
    start_serial_threads()
    messagebox.showinfo("Connected", f"Connected to {port}")
    # send leveling mode as the first real command on connect
    send_command(1)
    current_mode.set(mode_labels[1])

# ---------- GUI update loop ----------
def update_gui():
    # consume serial messages and update UI
    updated = False
    while True:
        try:
            line = serial_q.get_nowait()
        except queue.Empty:
            break
        updated = True
        data_label.config(text=line)
        # optional parsing: Arduino snippet prints "Parsed mode: <n>"
        if line.startswith("Parsed mode:"):
            try:
                n = int(line.split("Parsed mode:")[-1].strip())
                # reflect Arduino-acknowledged mode in UI
                if n == 0:
                    current_mode.set(mode_labels[0])
                elif 0 <= n < len(mode_labels):
                    current_mode.set(mode_labels[n])
                else:
                    current_mode.set(f"Mode {n}")
            except Exception:
                pass
        # parse sensor-like lines if present
        if "M1 Speed:" in line:
            try:
                m1_speed.set(line.split("M1 Speed:", 1)[1].strip())
            except Exception:
                pass
        if "M2 Speed:" in line:
            try:
                m2_speed.set(line.split("M2 Speed:", 1)[1].strip())
            except Exception:
                pass
        if "Strain:" in line:
            try:
                strain.set(line.split("Strain:", 1)[1].strip())
            except Exception:
                pass
        if "Tilt:" in line:
            try:
                tilt.set(line.split("Tilt:", 1)[1].strip())
            except Exception:
                pass

    # schedule next update
    root.after(50, update_gui)

# ---------- UI widgets ----------
connect_btn = tk.Button(root, text="Connect", command=connect_to_arduino, width=12, height=1, font=large_font)
connect_btn.pack(pady=6)

btn_frame_left = tk.Frame(root)
btn_frame_left.place(x=20, y=150)
btn_frame_right = tk.Frame(root)
# move manual controls further to the right on a 1280-wide window
# previous x=650; set x=980 (adjust if your monitor/window size differs)
btn_frame_right.place(x=980, y=150)

for mode in range(4):
    b = tk.Button(btn_frame_left, text=mode_labels[mode], width=button_w, height=button_h, font=large_font,
                  command=lambda m=mode: set_mode(m))
    b.pack(pady=6)

manual_modes = list(range(4, 10))
for i, mode in enumerate(manual_modes):
    r = i % 3
    c = i // 3
    b = tk.Button(btn_frame_right, text=mode_labels[mode], width=12, height=2, font=large_font,
                  command=lambda m=mode: set_mode(m))
    b.grid(row=r, column=c, padx=8, pady=8)

# sensor displays
labels_info = [
    ("Tilt:", tilt, 20, 520),
    ("Pressure:", strain, 20, 560),
    ("M1 Speed:", m1_speed, 20, 600),
    ("M2 Speed:", m2_speed, 20, 640),
]
for text, var, x, y in labels_info:
    lbl = tk.Label(root, text=text, font=large_font, bg='#f0f0f0')
    lbl.place(x=x, y=y)
    ent = tk.Entry(root, textvariable=var, font=large_font, width=12, justify='left')
    ent.place(x=x+140, y=y)
    ent.config(state='readonly')

# ---------- Clean shutdown ----------
def on_closing():
    stop_event.set()
    # give threads a moment to exit and flush commands
    time.sleep(0.05)
    try:
        if arduino and arduino.is_open:
            arduino.close()
    except Exception:
        pass
    root.destroy()

root.protocol("WM_DELETE_WINDOW", on_closing)

# start UI loop and initial visual state
current_mode.set(mode_labels[1])
root.after(50, update_gui)
root.mainloop()
