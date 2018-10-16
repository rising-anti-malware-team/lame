import ctypes
import platform


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

    def print_default_color(self, text):
        self.set_cmd_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
        print(text)

    def print_red_text(self, text):
        self.set_cmd_color(FOREGROUND_RED | FOREGROUND_INTENSITY)
        print(text)

    def print_green_text(self, text):
        self.set_cmd_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY)
        print(text)

    def print_blue_text(self, text): 
        self.set_cmd_color(FOREGROUND_BLUE | FOREGROUND_INTENSITY)
        print(text)

    def print_red_text_with_blue_bg(self, text):
        self.set_cmd_color(FOREGROUND_RED | FOREGROUND_INTENSITY| BACKGROUND_BLUE | BACKGROUND_INTENSITY)
        print(text)




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

    def set_cmd_color(self, text, color=None, on_color=None, attrs=None):
        fmt_str = '\x1B[;%dm%s\x1B[0m'
        if color is not None:
            text = fmt_str % (color, text)

        if on_color is not None:
            text = fmt_str % (on_color, text)

        if attrs is not None:
            for attr in attrs:
                text = fmt_str % (color, text)
        return text

    def print_default_color(self, text):
        print(self.set_cmd_color(text, 0))

    def print_red_text(self, text):
        print(self.set_cmd_color(text, 31))

    def print_green_text(self, text):
        print(self.set_cmd_color(text, 32))

    def print_blue_text(self, text):
        print(self.set_cmd_color(text, 34))

    def print_red_text_with_blue_bg(self, text):
        print(self.set_cmd_color(text, 34, 44))


class ConsoleColor:

    def __init__(self):
        if platform.system() == "Windows":
            self.color = ConsoleColor_windows()
        else:
            self.color = ConsoleColor_linux()

    def set_cmd_color(self, *args):
        if platform.system() == "Windows":
            self.color.set_cmd_color(args[0], args[1])
        else:
            self.color.set_cmd_color(args[0], args[1], args[2], args[3])
        return

    def print_default_color(self, text):
        self.color.print_default_color(text)
        return

    def print_red_text(self, text):
        self.color.print_red_text(text)
        return

    def print_green_text(self, text):
        self.color.print_green_text(text)
        return

    def print_blue_text(self, text): 
        self.color.print_blue_text(text)
        return

    def print_red_text_with_blue_bg(self, text):
        self.color.print_red_text_with_blue_bg(text)
        return



if __name__ == "__main__":
    _c = ConsoleColor()
    _c.print_blue_text('blue')
    _c.print_default_color('default')
