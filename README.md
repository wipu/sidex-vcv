# sidex-vcv
VCV Rack plugin for CV-controlling Commodore 64 SID via MIDI

## DISCLAIMER

Before you try this, you need to be aware that

1. This module sends MIDI SysEx messages using the _Casio_ manufacturer ID (because the author doesn't own any Casio devices). If a real Casio instrument receives the messages, the results are undefined and **may harm your instrument or your data**. If you have a Casio instrument connected, you shouldn't use this module. In the future there will be a way to use a different manufacturer ID.
2. This module requires a patched version of VCV Rack, because the official version doesn't support MIDI messages of length > 3 bytes as of this writing. If you feel you need to use this in your official Rack right now, please contact Andrew Bell.

## What

SidEx is a simple CV to MIDI converter module that lets you drive your Commodore 64 SID chip (or an emulation of one) with control voltages from VCV Rack. And, if you use something like Expert Sleepers ES-8, the control voltages can even originate from a real Eurorack synthesizer, _making your Commodore 64 essentially an Eurorack module!_

## How

Funnily enough, the standard MIDI speed is just enough to send all SID register values as sysex dumps 50 times per second (a typical refresh rate of a SID engine running in a PAL Commodore 64). So, SidEx simply saves CV values it receives, and 50 times per second it converts them into SID register values and sends them as a sysex dump.

## Why

Well, why would you **not** want to play SID with your modular?

Even as is, SidEx is a fun way to play the SID. But in the future there will be auxiliary modules that let you control the SID on a higher level, controlling SidEx from higher level concepts.
