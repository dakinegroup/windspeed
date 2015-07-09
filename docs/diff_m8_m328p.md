#Background
Earlier I had used m328p. Unfortunately, the only chip left with me got bricked with wrong fuses (Reset pin was disabled). So I had to move to m8. It took sometime to reuse work done over m328p. 

#Differences
For sake of reference, here are the key implementation gaps detected:
* Timer Registers, USART registers names are different
* Only two interrupts are there, it is not possible to consider every i/o pin for interrupt for change in their levels
* Extended fuse bit is not there, default fuse enables internal RC oscillator. To work on external crystal ceramic resonator, following fuses are used:
    - LFuse: C9
    - HFuse: EF
* In AVR, interrupt vectors are named differently
* Flash is limited to 8k, so program has to be managed carefully. Not to forget still important constraint of RAM which is 1k, half that of m328p.