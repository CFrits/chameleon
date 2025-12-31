# Bare metal programs.

- **small0**              The smallest possible program which can be loaded onto the Chameleon.
- **uart1a**              Print a character on the Chameleon toolkit debug window.
- **uart1b**              Call a C function from the assembler startup file. Print "Hello World!"
- **uart1c**              Calculate the length of the message to print. Limit to 29 characters.
- **uart1d**              Support uart_transmit_string with more than 29 characters.
- **led1a**               Turn led 1 on.
- **lcd1a**               Display Hello World on the LCD.
- **uart2a**              Add an uart_receive function and a minimal receive panel message implementation.
- **uart2b-reset**        Add a reset option, so no power cycle is needed anymore to load new firmware.
- **led1e-blink-busy**    Rearrange source code. Blink a led with a busy loop.
- **led1e-timer**         Add timer interupts and blink the led with a timer.

