# Chameleon
Port of micropython to the chameleon.

# Toolkit
The chameleon still needs to be programmed with the original toolkit of the development environment.
It's still available from the website [Chameleon](https://chameleon.synth.net/english/index.shtml)
The debugging interface is however very limited and is not very useful and not even possible when the
application is flashed onto the chameleon.
There a toolkit program is included which is made to run in linux. 
There is no makefile for the toolkit program but it can simple be compiled with the following command:

    gcc toolkit.c -o toolkit -lpthread -Wall

Here are some examples:

    $./toolkit
    >>> 
    Repl friendly mode
    ChamPython v0.2 based on MicroPython v1.27.0; Chameleon coldfire
    >>> import led
    >>> led.on(0)
    >>> 
    >>> import lcd
    >>> lcd.print(0, 0, "Micropython v0.2")
    >>> lcd.print(1, 0, "   Chameleon    ")
    >>> 
