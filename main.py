from tkinter import *
import ctypes
import sys
import glob
import serial
import time
from tkinter import messagebox

init_main = False

root = Tk()
s = serial.Serial()
s.baudrate = 115200
variable = StringVar(root)
init_flag = False
encoder = None
voltage = None
speed = None
port_menu = OptionMenu(root, variable, "")
init_scan = False
kp_text = None
ki_text = None
kd_text = None
output_limit = None
set_position = None


def enc_reset():
    s.write(b'104\r\n')
    return


def auto_tune_controller():
    if output_limit.get() == '':
        messagebox.showerror("Error", "Output Limit cannot be Empty!")
    else:
        s.write(f'68 {output_limit.get()} \r\n'.encode("utf-8"))
    return


def change_pid_tunings():
    if output_limit.get() == '':
        messagebox.showerror("Error", "Output Limit cannot be Empty!")
    else:
        kp_val = kp_text.get()
        ki_val = ki_text.get()
        kd_val = kd_text.get()
        if kp_val == '':
            kp_val = 0
        if ki_val == '':
            ki_val = 0
        if kd_val == '':
            kd_val = 0
        s.write(
            f"71\t{kp_val} {ki_val} {kd_val} {output_limit.get()} \r\n".encode("utf-8"))


def pos_change():
    if set_position.get() == '':
        messagebox.showerror("Error", "Encoder cannot be Empty!")
    else:
        s.write(f"74 {set_position.get()} \r\n".encode("utf-8"))


def motor_stop():
    s.write(f"79\r\n".encode("utf-8"))


def main_screen():
    global kp_text
    global ki_text
    global kd_text
    global init_main
    global output_limit
    global set_position
    if init_main == False:
        init_main = True
        encoder.pack()
        reset_encoder = Button(root, text="Reset Encoder", command=enc_reset)
        reset_encoder.pack()
        speed.pack()
        voltage.pack()
        auto_tune = Button(root, text="Auto Tune",
                           command=auto_tune_controller)
        auto_tune.pack()
        kp_label = Label(root, text="KP:")
        kp_label.pack()
        kp_text = Entry(root, width=30)
        kp_text.pack()
        ki_label = Label(root, text="KP:")
        ki_label.pack()
        ki_text = Entry(root, width=30)
        ki_text.pack()
        kd_label = Label(root, text="KP:")
        kd_label.pack()
        kd_text = Entry(root, width=30)
        kd_text.pack()
        output_limit_label = Label(root, text="Output Limit: ")
        output_limit_label.pack()
        output_limit = Entry(root, width=30)
        output_limit.pack()
        change_tunings = Button(
            root, text="Change Tunings", command=change_pid_tunings)
        change_tunings.pack()
        enc_val = Label(root, text="Encoder Value: ")
        enc_val.pack()
        set_position = Entry(root, width=30)
        set_position.pack()
        set_pos = Button(root, text="Set Motor Position", command=pos_change)
        set_pos.pack()
        motor_brake = Button(root, text="Stop Motor", command=motor_stop)
        motor_brake.pack()
    data = s.readline()
    data = data.decode("utf-8").replace('\r', '').replace('\n', '')
    data = data.split('\t')
    print(data)
    if data[0] == "68" and len(data) < 2:
        print("Auto Tuning in Progress")
        while True:
            data = s.readline().decode("utf-8").replace('\r', '').replace('\n', '')
            data = data.split("\t")
            # print(data)
            if data[0] == '65':
                break
        # data = data.split('\t')
        kp_text.delete(0, END)
        kp_text.insert(0, data[1])
        ki_text.delete(0, END)
        ki_text.insert(0, data[2])
        kd_text.delete(0, END)
        kd_text.insert(0, data[3])
    # print(data)
    try:
        encoder.config(text=f'Encoder: {data[0]}')
        speed.config(text=f'RPM: {data[1]}')
        voltage.config(text=f'Who knows: {data[2]}')
    except Exception as e:
        print(e)
    root.after(2, main_screen)


def clear_frame():
    global encoder
    global speed
    global voltage
    for widgets in root.winfo_children():
        widgets.destroy()
    encoder = Label(root, text="0")
    speed = Label(root, text="0")
    voltage = Label(root, text="0")


def combo_scan():
    global init_scan
    if init_scan == False:
        ports = serial_ports()
        variable.set(ports[0])  # default value
        menu = port_menu["menu"]
        menu.delete(0, "end")
        for port in ports:
            menu.add_command(label=port,
                             command=lambda value=port: variable.set(value))
        port_menu.pack()
        connect_port()
        init_scan = True
        return
    ports = serial_ports()
    variable.set(ports[0])  # default value
    menu = port_menu["menu"]
    menu.delete(0, "end")
    for port in ports:
        menu.add_command(label=port,
                         command=lambda value=port: variable.set(value))


def connect_controller():
    global init_flag
    s.port = variable.get()
    s.open()
    clear_frame()
    print(s.is_open)
    s.write(b'69\r\n')
    data = s.readline()
    data = int(data.decode('utf-8').replace('\n', '').replace('\r', ''))
    if data == 69:
        head_title = Label(root, text="Motor Controller", font=("Arial", 32))
        head_title.pack(side=TOP)
        main_screen()


def connect_port():
    connect_button = Button(
        root, text="Connect with Controller", command=connect_controller)
    connect_button.pack()


def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


def show_window():
    user32 = ctypes.windll.user32
    screensize = user32.GetSystemMetrics(0), user32.GetSystemMetrics(1)
    if screensize[0] >= 1920 and screensize[1] >= 1080:
        root.geometry("1270x768")
    else:
        root.geometry("800x600")
    frame = Frame(root)
    frame.pack()
    leftframe = Frame(root)
    leftframe.pack(side=LEFT)

    rightframe = Frame(root)
    rightframe.pack(side=RIGHT)

    root.title("Motor Controller")
    head_title = Label(root, text="Motor Controller", font=("Arial", 32))
    head_title.pack(side=TOP)
    scan_ports()
    encoder = Label(root, text="Encoder Value", font=("Arial", 10))
    encoder.pack()
    # get_enc_data()
    root.mainloop()


def get_enc_data():
    encoder = Label(root, text="Encoder Value", font=("Arial", 10))
    # if s.isOpen() and s.in_Waiting():
    #     enc_data = s.readline()
    encoder.pack()
    root.mainloop()


def scan_ports():
    scan = Button(root, text="Scan Ports", font=(
        "Arial", 20), command=combo_scan)
    scan.pack()
    root.mainloop()


if __name__ == "__main__":
    show_window()
    if init_flag:
        root.after(50, main_screen)
        root.mainloop()
