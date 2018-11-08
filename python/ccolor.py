import ctypes
import platform
import sys


STD_INPUT_HANDLE = -10
STD_OUTPUT_HANDLE= -11
STD_ERROR_HANDLE = -12

FOREGROUND_BLACK = 0x0
FOREGROUND_BLUE = 0x01 # text color contains blue.
FOREGROUND_GREEN= 0x02 # text color contains green.
FOREGROUND_RED = 0x04 # text color contains red.
FOREGROUND_INTENSITY = 0x08 # text color is intensified.

BACKGROUND_BLUE = 0x10 # background color contains blue.
BACKGROUND_GREEN= 0x20 # background color contains green.
BACKGROUND_RED = 0x40 # background color contains red.
BACKGROUND_INTENSITY = 0x80 # background color is intensified.

class ConsoleColor_windows:

    def __init__(self):
        self.std_out_handle = ctypes.windll.kernel32.GetStdHandle(STD_OUTPUT_HANDLE)

    def set_cmd_color(self, color, handle=None):
        if handle is None:
            handle = self.std_out_handle
        return ctypes.windll.kernel32.SetConsoleTextAttribute(handle, color)

    def reset_color(self):
        self.set_cmd_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

    def set_red_text(self):
        self.set_cmd_color(FOREGROUND_RED | FOREGROUND_INTENSITY)

    def set_green_text(self):
        self.set_cmd_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY)

    def set_blue_text(self): 
        self.set_cmd_color(FOREGROUND_BLUE | FOREGROUND_INTENSITY)

    def set_red_text_with_blue_bg(self):
        self.set_cmd_color(FOREGROUND_RED | FOREGROUND_INTENSITY| BACKGROUND_BLUE | BACKGROUND_INTENSITY)




'''
0  All attributes off
1  Bold (or Bright)
4  Underline
5  Blink
7  Reverse
30 Black text
31 Red text
32 Green text
33 Yellow text
34 Blue text
35 Purple text
36 Cyan text
37 White text
40 Black background
41 Red background
42 Green background
43 Yellow background
44 Blue background
45 Purple background
46 Cyan background
47 White background
'''
class ConsoleColor_linux:

    def set_cmd_color(self, color=None, on_color=None):
        if color is not None and on_color is not None:
            sys.stdout.write('\033[1;%d;%dm' % (color, on_color))
        if color == 0:
            sys.stdout.write('\033[0m')
        return

    def reset_color(self):
        self.set_cmd_color(0)

    def set_red_text(self):
        self.set_cmd_color(31, 40)

    def set_green_text(self):
        self.set_cmd_color(32, 40)

    def set_blue_text(self):
        self.set_cmd_color(34, 40)

    def set_red_text_with_blue_bg(self):
        self.set_cmd_color(34, 44)


class ConsoleColor:

    def __init__(self):
        if platform.system() == "Windows":
            self.color = ConsoleColor_windows()
        else:
            self.color = ConsoleColor_linux()

    def set_cmd_color(self, *args):
        self.color.set_cmd_color(args[0], args[1])
        return

    def reset_color(self):
        self.color.reset_color()
        return

    def set_red_text(self):
        self.color.set_red_text()
        return

    def set_green_text(self):
        self.color.set_green_text()
        return

    def set_blue_text(self): 
        self.color.set_blue_text()
        return

    def set_red_text_with_blue_bg(self):
        self.color.set_red_text_with_blue_bg()
        return



if __name__ == "__main__":
    _c = ConsoleColor()
    _c.set_blue_text()
    print('blue')
    _c.set_red_text()
    print('red')
    _c.reset_color()
    print('default')
