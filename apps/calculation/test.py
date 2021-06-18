import sys, tty
def command_line():
    tty.setraw(sys.stdin)
    while True:
        char = sys.stdin.read(1)
        if ord(char) == 3: # CTRL-C
            break;
        print(ord(char))
        sys.stdout.write(u"\u001b[1000D") # Move all the way left

command_line();
