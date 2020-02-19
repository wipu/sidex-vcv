# sidex-vcv
VCV Rack plugin for CV-controlling Commodore 64 SID via MIDI

## DISCLAIMER

Before you try this, you need to be aware that

1. This module sends MIDI SysEx messages using the _Casio_ manufacturer ID (because the author doesn't own any Casio devices). If a real Casio instrument receives the messages, the results are undefined and **may harm your instrument or your data**. If you have a Casio instrument connected, you shouldn't use this module. In the future there will be a way to use a different manufacturer ID.
2. This module requires a [patched version of VCV Rack](https://github.com/wipu/Rack/tree/sysex-send), because the official version doesn't support MIDI messages of length > 3 bytes as of this writing. If you feel you need to use this in your official Rack right now, please contact [Andrew Belt](https://github.com/VCVRack/Rack/issues/1433), the author of VCV Rack.

## What

SidEx is a simple CV to MIDI converter module that lets you drive your Commodore 64 SID chip (or an emulation of one) with control voltages from VCV Rack. And, if you use something like [Expert Sleepers ES-8](https://www.expert-sleepers.co.uk/es8.html), the control voltages can even originate from a real Eurorack synthesizer, _making your Commodore 64 essentially a Eurorack module!_

## How

Funnily enough, the standard MIDI speed is just enough to send all SID register values as sysex dumps 50 times per second (a typical refresh rate of a SID engine running in a PAL Commodore 64). So, SidEx simply receives CV values and 50 times per second converts them to SID register values and sends them as a sysex dump.

Naturally you'll need the receiving counterpart running on your Commodore 64: [sidex-0.1.prg](https://github.com/wipu/mockodore/releases/download/sidex-0.1/sidex-0.1.prg)

(The prg assumes the MIDI interface is Datel compatible. It has been tested with C-Lab, but e.g. Kerberos should also work. Other devices will be supported in the future.)

Then just clone this repository and run

    ./run-in-developer-rack.sh

The script will download (if needed) the modified Rack, compile it together with SidEx and run it. Ah, did I mention all this requires Linux?

Then just create an instance of SidEx, connect some signals to relevant inputs (e.g. osc1 gate >=5, some freq, wf=.4 (PULSE=4 divided by 10 to keep it within VCV voltage limits), sustain=10) and select the MIDI input of your Commodore 64.

## Why

Well, why would you **not** want to play SID with your modular?

Even as is, SidEx is a fun way to play the SID. But in the future there will be auxiliary modules that let you control the SID on a higher level, controlling SidEx from higher level concepts.

## Status

Some interesting sounds can already be made. These are the most important known limitations:

* some input ranges are not checked, resulting in odd behaviour beyound limits
* pulse width, ring mod, osc sync input handling is not implemented quite yet
* waveform selection is clumsy: the input value must be the raw nybble divided by 10 (to keep the maximum value 15 within voltage limits, under 10). For example pulse waveform is selected with the value 0.4.
* graphical design of the panel may not please every eye...
* hardcoded Datel MIDI assumption
* hardcoded Manufacturer ID (Casio)
* no save/restore support so you need to repatch after restarting Rack
* this page is all the documentation there is

## Help

Do you know how to implement the look'n'feel of C64 as a simple SVG required for a VCV plugin panel? If you do, your help will be appreciated.

How would you like to control your SID from VCV? Please tell so we can create modules that make it easier to generate CV to SidEx.

How can other modules send SID-related messages locked to 50 Hz frames? This would be useful for stereotypical SID tricks like "wavetable synthesis" and arpeggios - changing the waveform or pitch on every frame.

What else? Feel free to contribute with questions, ideas, patches, critique, ...
