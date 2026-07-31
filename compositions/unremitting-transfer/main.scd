
// Kill any existing server but don't reboot (to preserve JACK connection)
Server.killAll;

Window.closeAll;

s = Server.local;

// Configure to connect to existing JACK server (don't auto-start)
s.options.numOutputBusChannels = 2;
s.options.numInputBusChannels = 2;
s.options.blockSize = 64;
s.options.sampleRate = 48000;

s.waitForBoot({

	"loading unremitting-transfer main".postln;

	~noteOnPath = '/justo/noteOn';
	~noteOffPath = '/justo/noteOff';

	~bpm = 30;
	~rhythmLength = 4;
	~slowDivider = 16;
	~seed = 0;  // 0 is self-seed
	~maxDist = 10;
	~delay = 5;

	~preQuickBus = Bus.audio(s, 1);
	~quickBus = Bus.audio(s, 1);
	~preSlowBus  = Bus.audio(s, 1);
	~slowBus  = Bus.audio(s, 1);
	~resetBus = Bus.audio(s, 1);

	SynthDef(\clock, { |preQuickBus = 0, quickBus = 0, preSlowBus = 0, slowBus = 0, resetBus = 0|
		var cps = ~bpm / 60;
		var ticks = Impulse.ar(cps);
		// Out.ar(resetBus, PulseDivider.ar(ticks, div: ~slowDivider));
		// Out.ar(slowBus, PulseDivider.ar(ticks, div: ~slowDivider));
		Out.ar(preSlowBus, PulseDivider.ar(ticks, div: ~rhythmLength));
		Out.ar(slowBus, DelayN.ar(PulseDivider.ar(ticks, div: ~rhythmLength), 0.01, ~delay * SampleDur.ir));
		Out.ar(preQuickBus, ticks);
		Out.ar(quickBus, DelayN.ar(ticks, 0.01, ~delay * SampleDur.ir));
		// Out.ar(quickBus, EuCosa.ar(ticks, length: ~rhythmLength, beats: 0.7 * ~rhythmLength, thresh0: 0.99)[0]);
	}).add;

	SynthDef(\engine, { |preQuickBus = 0, quickBus = 0, preSlowBus = 0, slowBus = 0, resetBus = 0|
		var preQuickTrig = In.ar(preQuickBus);
		var quickTrig = In.ar(quickBus);
		var preSlowTrig = In.ar(preSlowBus);
		var slowTrig = In.ar(slowBus);
		var resetTrig = In.ar(resetBus);
		var leftHand = Justo.ar(slowTrig, maxSize: ~rhythmLength, maxRatio: 2, root: 110, maxDistance: 10, mutateAll: 0.1, seed: ~seed);
		var rightHand = Justo.ar(quickTrig, maxSize: ~rhythmLength, maxRatio: 3, root: 660, maxDistance: ~maxDist, mutateBad: 1, mutateAll: 0.1, reset: ~rhythmLength, reverse: 0, numerator: leftHand.numerator, denominator: leftHand.denominator, seed: ~seed);
		SendReply.ar(preSlowTrig,  ~noteOffPath, [0]);
		SendReply.ar(slowTrig,  ~noteOnPath, [leftHand.frequency,  leftHand.distance, leftHand.numerator, leftHand.denominator, 0, 0]);
		SendReply.ar(preQuickTrig,  ~noteOffPath, [1]);
		SendReply.ar(quickTrig, ~noteOnPath, [rightHand.frequency, rightHand.distance, rightHand.numerator, rightHand.denominator, leftHand.distance, 0, 0]); //leftHand.numerator, leftHand.denominator]);
	}).add;

	s.sync;

	MIDIClient.init;
	~index = MIDIClient.destinations.detectIndex({ |ep| ep.device.contains("Neutron") });
	if (~index.notNil) {
		~midiOut = MIDIOut(~index);
		~midiOut.connect(~index);
		~allNotesOff.value(~midiOut, 0);
	} {
		"ERROR: Neutron not found in destinations.".warn;
	};

	~allNotesOff = { |midiOut, channel = 0|
		if (midiOut.notNil) {
			128.do({ |note| midiOut.noteOff(channel, note, 0) });
			midiOut.control(channel, 123, 0);
			midiOut.control(channel, 121, 0);
		};
	};

	~freq2BentMidi = { |freq|
		var midi = freq.cpsmidi;
		var note = midi.round;
		var offset = midi - note;
		var bend = offset.linlin(-1.0, 1.0, 0, 16383).asInteger;
		[note.asInteger, bend.asInteger];
	};

	~sendMidiOn = { |note, bend, amp, midiOut, channel=0|
		if (midiOut.notNil) {
			midiOut.bend(channel, bend);
			midiOut.noteOn(channel, note, amp);
		} {
			"MIDI output not available - would send note: % bend: % amp: %".format(note, bend, amp).postln;
		};
	};

	~sendMidiOff = { |prev, midiOut, channel=0|
		if (midiOut.notNil) {
			if (prev != 0) {midiOut.noteOff(channel, prev);};
		};
	};

	~prevHigh = 0;
	~prevLow = 0;

	OSCFunc({ |msg|
		var path, node, reply, freq, dist, num, den, ldist, lnum, lden, durn, amp, note, bend;
		# path, node, reply, freq, dist, num, den, ldist, lnum, lden = msg;
		# note, bend = ~freq2BentMidi.value(freq);
		[note, bend, dist, ldist].postln;
		if (ldist == 0) {
			~sendMidiOn.value(note, bend, 127, ~midiOut, 0);
			~prevLow = note;
		} {
			~sendMidiOn.value(note, bend, 127, ~midiOut, 0);
			~prevHigh = note;
		};
	}, ~noteOnPath);

	OSCFunc({ |msg|
		var path, node, reply, ldist;
		# path, node, reply, ldist = msg;
		if (ldist == 0) {
			~sendMidiOff.value(~prevLow, ~midiOut, 0);
		} {
			~sendMidiOff.value(~prevHigh, ~midiOut, 0);
		};
	}, ~noteOffPath);

	~clockSynth = Synth(\clock, [\preQuickBus, ~preQuickBus, \quickBus, ~quickBus, \preSlowBus, ~preSlowBus, \slowBus, ~slowBus, \resetBus, ~resetBus]);
	~engineSynth = Synth(\engine, [\preQuickBus, ~preQuickBus, \quickBus, ~quickBus, \preSlowBus, ~preSlowBus, \slowBus, ~slowBus, \resetBus, ~resetBus], addAction: \addAfter);

	"unremitting-transfer main loaded".postln;

});

