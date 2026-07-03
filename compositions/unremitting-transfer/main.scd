
s.reboot;

Window.closeAll;

s.waitForBoot({

	"loading unremitting-transfer main".postln;

	~routerPath = '/justo/note';

	~bpm = 360;
	~rhythmLength = 16;
	~slowDivider = 4;
	~seed = 0;  // 0 is self-seed

	SynthDef(\clock, { |quickBus = 0, slowBus = 0, resetBus = 0|
		var cps = ~bpm / 60;
		var ticks = Impulse.ar(cps);
		Out.ar(resetBus, PulseDivider.ar(ticks, div: ~rhythmLength));
		Out.ar(slowBus, PulseDivider.ar(ticks, div: ~slowDivider));
		Out.ar(quickBus, EuCosa.ar(ticks, length: ~rhythmLength, beats: 0.7 * ~rhythmLength, thresh0: 0.99)[0]);
	}).add;

	SynthDef(\engine, { |quickBus = 0, slowBus = 0, resetBus = 0|
		var quickTrig = In.ar(quickBus);
		var slowTrig = In.ar(slowBus);
		var resetTrig = In.ar(resetBus);
		var leftHand = Justo.ar(slowTrig, maxSize: ~rhythmLength / ~slowDivider, root: 110, maxDistance: 10, mutate: 0.1, seed: ~seed);
		var rightHand = Justo.ar(quickTrig, maxSize: ~rhythmLength, root: 440, maxDistance: 10, mutate: 1, reset: resetTrig, reverse: 0.1, numerator: leftHand.numerator, denominator: leftHand.denominator, seed: ~seed);
		SendReply.ar(slowTrig,  ~routerPath, [leftHand.frequency,  0.1, 0.5, leftHand.distance]);
		SendReply.ar(quickTrig, ~routerPath, [rightHand.frequency, 1.0, 1.0, rightHand.distance]);
	}).add;

	s.sync;

	MIDIClient.init;
	~index = MIDIClient.destinations.detectIndex({ |ep| ep.device.contains("Neutron") });
	if (~index.notNil) {
		~midiOut = MIDIOut(~index);
		~midiOut.connect(~index);
	} {
		"ERROR: Neutron not found in destinations.".warn;
	};

	~freq2BentMidi = { |freq|
		var midi = freq.cpsmidi;
		var note = midi.round;
		var offset = midi - note;
		var bend = (8192 + (offset * (8192 / 2))).clip(0, 16383);
		[note.asInteger, bend.asInteger];
	};

	~sendMidi = { |freq, durn, midiOut, channel=0|
		var note, bend;
		# note, bend = ~freq2BentMidi.value(freq);
		Routine({
			midiOut.bend(channel, bend);
			midiOut.noteOn(channel, note, 64);
			durn.wait;
			midiOut.noteOff(channel, note, 0);
		}).play(SystemClock);
	};

	OSCFunc({ |msg|
		var path, node, reply, freq, loss, amp, dist;
		# path, node, reply, freq, loss, amp, dist = msg;
		[freq, loss, amp, dist].postln;
		~sendMidi.value(freq, 0.25, ~midiOut, 0);
	}, ~routerPath);

	~clockSynth = Synth(\clock, [\quickBus, ~quickBus, \slowBus, ~slowBus, \resetBus, ~resetBus]);
	~engineSynth = Synth(\engine, [\quickBus, ~quickBus, \slowBus, ~slowBus, \resetBus, ~resetBus], addAction: \addAfter);

	"unremitting-transfer main loaded".postln;

});

