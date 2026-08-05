// Neutron - MIDI configuration class for Behringer Neutron synthesizer
// Provides methods to configure split keyboard, note priority, and other settings

Neutron {

        var <midiOut;
        var <channel;

        *new { |midiOut, channel=0|
                ^super.newCopyArgs(midiOut, channel);
        }

        *connect { |channel=0|
                var index;
                MIDIClient.init;
                index = MIDIClient.destinations.detectIndex({ |ep|
                        ep.device.contains("Neutron")
                });
                if (index.notNil) {
                        var midiOut = MIDIOut(index);
                        midiOut.connect(index);
                        "Connected to Neutron on MIDI port %".format(index).postln;
                        ^Neutron(midiOut, channel);
                } {
                        "ERROR: Neutron not found in MIDI destinations.".warn;
                        ^nil;
                };
        }

        setSplitMode { |splitNote=60, enable=true|
                if (midiOut.isNil) {
                        "ERROR: MIDI output not initialized".warn;
                        ^this;
                };

                // Neutron uses CC messages for configuration
                // CC 70: Split point (0-127 = MIDI note number)
                // CC 71: Split enable (0=off, 127=on)
                if (enable) {
                        this.control(70, splitNote.clip(0, 127));
                        this.control(71, 127);
                        "Neutron: Split mode enabled at note %".format(splitNote).postln;
                } {
                        midiOut.control(channel, 71, 0);
                        "Neutron: Split mode disabled".postln;
                };
                ^this;
        }

        allNotesOff {
                if (midiOut.isNil) {
                        "ERROR: MIDI output not initialized".warn;
                        ^this;
                };

                128.do({ |note| this.noteOff(note) });
                this.control(123, 0);  // All notes off CC
                this.control(121, 0);  // Reset all controllers
                "Neutron: All notes off".postln;
                ^this;
        }

        resetBend {
                if (midiOut.isNil) {
                        "ERROR: MIDI output not initialized".warn;
                        ^this;
                };

                midiOut.bend(channel, 8192);  // Center position
                ^this;
        }

        noteOn { |note, bend=8192, velocity=127|
                if (midiOut.isNil) {
                        "ERROR: MIDI output not initialized".warn;
                        ^this;
                };

                midiOut.bend(channel, bend);
                midiOut.noteOn(channel, note, velocity);
                ^this;
        }

        noteOff { |note, velocity=0|
                if (midiOut.isNil) {
                        "ERROR: MIDI output not initialized".warn;
                        ^this;
                };

                midiOut.noteOff(channel, note, velocity);
                ^this;
        }

        control { |cc, value=0|
                if (midiOut.isNil) {
                        "ERROR: MIDI output not initialized".warn;
                        ^this;
                };

                midiOut.control(channel, cc, value);
                ^this;
        }

        setKeyPriority { |priority = \low|
                var map = Dictionary[
                        \low        -> 0,
                        \high       -> 1,
                        \last       -> 2
                ];
		^this.sysexSetFromMap(0x01, map, priority);
        }

        setAssign { |source = \modWheel|
                var map = Dictionary[
                        \osc1       -> 0,
                        \osc2       -> 1,
                        \velocity   -> 2,
                        \modWheel   -> 3,
                        \aftertouch -> 4
                ];
		^this.sysexSetFromMap(0x04, map, source);
        }

	setRetrigger { |on = true|
		^this.sysexSetFromBool(0x05, 0, 1, on);
	}

        setVcfMode { |mode = \band|
                var map = Dictionary[
                        \high -> 0,
                        \band -> 1,
                        \low  -> 2
                ];
		^this.sysexSetFromMap(0x10, map, mode);
        }

	setParaphonic { |on = true|
		^this.sysexSetFromBool(0x0f, 0, 1, on);
	}

	setOsc1Bypass { |bypass = true|
		^this.sysexSetFromBool(0x22, 0, 1, bypass);
	}

	setOsc2Bypass { |bypass = true|
		^this.sysexSetFromBool(0x23, 0, 1, bypass);
	}

        setOsc1Range { |range = \10oct|
                var map = Dictionary[
                        \thirtyTwoFoot -> 0,
                        \sixteenFoot   -> 1,
                        \eightFoot     -> 2,
                        \tenOctaves    -> 3
                ];
		^this.sysexSetFromMap(0x26, map, range);
        }

        setOsc2Range { |range = \10oct|
                var map = Dictionary[
                        \thirtyTwoFoot -> 0,
                        \sixteenFoot   -> 1,
                        \eightFoot     -> 2,
                        \tenOctaves    -> 3
                ];
		^this.sysexSetFromMap(0x27, map, range);
        }

	sysexSetFromBool { |param, off, on, test|
	        var value;
		if (test) {value = on;} {value = off;};
		^this.sysexSet(param, value);
	}

        sysexSetFromMap { |param, map, key|
                var value = map[key];
                if (value.isNil) {
                        ("Error: Unknown value '" ++ key ++ "'").error;
                        ^this;
                };
		^this.sysexSet(param, value);
        }

        sysexSet { |param, value|
                var sysexPayload = Int8Array[
			0xf0,
                        0x00, 0x20, 0x32,  // Behringer Manufacturer Identification
                        0x28,              // Neutron Device Family Identifier
                        0x7f,              // Global Device Index Channel (Boradcast)
                        0x0a,              // Command action byte: Edit Parameter
                        param,              
                        value,  
			0xf7
                ];
                midiOut.sysex(sysexPayload);
                ("Neutron sysex").postln;
                ^this;
        }

	restoreDefaults {
                var sysexPayload = Int8Array[
			0xf0,
                        0x00, 0x20, 0x32,  // Behringer Manufacturer Identification
                        0x28,              // Neutron Device Family Identifier
                        0x7f,              // Global Device Index Channel (Boradcast)
                        0x0b,              // Reset
			0xf7
                ];
                midiOut.sysex(sysexPayload);
                ("Neutron restore").postln;
                ^this;
	}

        printInfo {
                "Neutron MIDI Configuration:".postln;
                "  MIDI Out: %".format(midiOut).postln;
                "  Channel: %".format(channel).postln;
        }
}
