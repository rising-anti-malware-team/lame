import ctypes


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

class ConsoleColor:
    std_out_handle = ctypes.windll.kernel32.GetStdHandle(STD_OUTPUT_HANDLE)
    
    def set_cmd_color(self, color, handle=std_out_handle):
        bool = ctypes.windll.kernel32.SetConsoleTextAttribute(handle, color)
        return bool
        
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



if __name__ == "__main__":
    _c = ConsoleColor()
    _c.set_blue_text()
    print("fsdfsd")
    _c.reset_color()