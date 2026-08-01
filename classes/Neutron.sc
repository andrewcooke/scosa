// Neutron - MIDI configuration class for Behringer Neutron synthesizer
// Provides methods to configure split keyboard, note priority, and other settings

Neutron {
	var <midiOut;
	var <channel;
	
	*new { |midiOut, channel=0|
		^super.newCopyArgs(midiOut, channel);
	}
	
	// Find and connect to Neutron MIDI device
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
	
	// Set keyboard split mode
	// splitNote: MIDI note number where split occurs (0-127)
	// When split is enabled, notes below splitNote go to one voice,
	// notes at or above splitNote go to another voice
	setSplitMode { |splitNote=60, enable=true|
		if (midiOut.isNil) {
			"ERROR: MIDI output not initialized".warn;
			^this;
		};
		
		// Neutron uses CC messages for configuration
		// CC 70: Split point (0-127 = MIDI note number)
		// CC 71: Split enable (0=off, 127=on)
		if (enable) {
			midiOut.control(channel, 70, splitNote.clip(0, 127));
			midiOut.control(channel, 71, 127);
			"Neutron: Split mode enabled at note %".format(splitNote).postln;
		} {
			midiOut.control(channel, 71, 0);
			"Neutron: Split mode disabled".postln;
		};
		^this;
	}
	
	// Set note priority mode
	// priority: \lowest, \highest, or \last
	setNotePriority { |priority=\lowest|
		var ccValue;
		
		if (midiOut.isNil) {
			"ERROR: MIDI output not initialized".warn;
			^this;
		};
		
		// Neutron uses CC 72 for note priority
		// 0-42 = Last note priority
		// 43-84 = Lowest note priority  
		// 85-127 = Highest note priority
		ccValue = switch(priority,
			\last,    { 21 },   // Middle of last range
			\lowest,  { 64 },   // Middle of lowest range
			\highest, { 106 },  // Middle of highest range
			{ 
				"Unknown priority mode: %. Using 'lowest'.".format(priority).warn;
				64 
			}
		);
		
		midiOut.control(channel, 72, ccValue);
		"Neutron: Note priority set to %".format(priority).postln;
		^this;
	}
	
	// Send all notes off
	allNotesOff {
		if (midiOut.isNil) {
			"ERROR: MIDI output not initialized".warn;
			^this;
		};
		
		128.do({ |note| midiOut.noteOff(channel, note, 0) });
		midiOut.control(channel, 123, 0);  // All notes off CC
		midiOut.control(channel, 121, 0);  // Reset all controllers
		"Neutron: All notes off".postln;
		^this;
	}
	
	// Reset pitch bend to center
	resetBend {
		if (midiOut.isNil) {
			"ERROR: MIDI output not initialized".warn;
			^this;
		};
		
		midiOut.bend(channel, 8192);  // Center position
		^this;
	}
	
	// Convenience method: send note with pitch bend
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
	
	// Print current configuration
	printInfo {
		"Neutron MIDI Configuration:".postln;
		"  MIDI Out: %".format(midiOut).postln;
		"  Channel: %".format(channel).postln;
	}
}
