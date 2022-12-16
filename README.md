# PSK31-Modulation-Generator
A C++ based QPSK/BPSK wav file generator made from scratch with the standard library! This was made to be used on my [Giraffe Project](https://github.com/joshua-jerred/Giraffe).

You can read about it here: https://joshuajer.red/projects/psk-modulation.html

#### IMPORTANT

This is currently ***untested*** and should not be used on any amateur bands. This is an active project and it has a few bugs. 
It follows the PSK31 standard including varicode and the qpsk convolutional code. Feel free to submit issues.

If you are using this with fldigi please look at the postamble function as fldigi does not follow the standard for message termination and there is a flag that must be set.

The raised cosine filter values are constants and may not have the best values, but it is fully functional in QPSK & BPSK modes at 125 and 250 symbol rates with fairly clean audio.

It can be used as a command line utility in the following ways:
```
./psk
./psk -m qpsk -s 250 -f filename.wav -t "Message to encode"
echo "You can pipe input to it!" | ./psk

-m : mode [bpsk, qpsk] - default is bpsk
-s : symbol_rate [125, 250, 500, 1000] - default is 125
-t : filename [filename.wav] - default is out.wav
```
If no input is piped in, it will prompt for input. All ASCII characters (Control and Printable) are supported.

To terminate input, add a newline (enter) and then do CTRL+D.





![Flow Chart](https://user-images.githubusercontent.com/19292194/203744196-0c35e758-d7cb-4656-9598-3b5b2fe2c8ef.png)
