import time

from rise.devices.joystick import Joystick

joy = Joystick()
joy.open("/dev/input/js0")
joy.start()

while True:
    print(joy.axis)
    print(joy.buttons)
    time.sleep(0.3)
