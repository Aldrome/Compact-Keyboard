## Compact Piano Keyboard with DSP and MIDI functions

Most piano keyboards that musicians use are full scale, which causes problems for composing or playing
in places that lack the space or accessibility, since it is made for performance. This keyboard project has the
focus of minimizing size to something you can fit on a desk and be able to move more freely with it. It will be
more of a MIDI controller and music creation, not as a performance machine.

# Issues to think about:
	- Choosing a microcontroller with the DSP and MIDI capability (Using Teensy 4.1 + Audio Shield Rev D)
	- Handle the keyboard matrix and decide the key amount
	- PCB designing and how to route the I/Os
	- The type of inputs, considering it is a piano
____________________________________________________________________________________________________________________

# Planning with steps
	1. Prepare microcontroller for use (solder all pin feet in place)
	2. Test out audio I/O using only the headphones
	3. Prototype a simple synth wave maker using a few buttons
	4. If possible, use a synth wave synthesizer to create a piano sound, maybe more. Else, use audio from free source.
	5. Create a prototype keyboard matrix, and a full scale on KiCad. Also add anti-ghosting using diodes.
	6. Test out potentiometer to divide the input keys to 2 sections.
	7. Add a slider + button for DSP, similar to a mixer.
	8. Depending on the time constraint and resources, print a PCB to handle all these features.

# Additional feature that can be added, though not a priority
	1. A display with UI implementation to decide instrument or maybe change DSP settings.
	2. The input keys by default will be a simple button, similar to a typing keyboard.
	   But if possible, use a sensitivity keyboard using a 2 point detector.
	3. No casings for this project.

Notes: This project will be done in 2 parts for every steps that includes prototyping. One being the physical prototype and
       the other part being the schematic design. After finishing schematic, create a footprint for PCB. Printing a PCB will not
       be a priority, as the demo will mainly be a concept using a prototype. Though we will print a PCB if possible.
