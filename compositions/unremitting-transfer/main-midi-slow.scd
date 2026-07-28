
s.reboot;

Window.closeAll;

s.waitForBoot({

	"loading unremitting-transfer main".postln;

	~routerPath = '/justo/note';

	~bpm = 30;
	~rhythmLength = 4;
	~slowDivider = 16;
	~seed = 0;  // 0 is self-seed
	~maxDist = 10;

	~quickBus = Bus.audio(s, 1);
	~slowBus  = Bus.audio(s, 1);
	~resetBus = Bus.audio(s, 1);

	SynthDef(\clock, { |quickBus = 0, slowBus = 0, resetBus = 0|
		var cps = ~bpm / 60;
		var ticks = Impulse.ar(cps);
		// Out.ar(resetBus, PulseDivider.ar(ticks, div: ~slowDivider));
		// Out.ar(slowBus, PulseDivider.ar(ticks, div: ~slowDivider));
		Out.ar(slowBus, PulseDivider.ar(ticks, div: ~rhythmLength));
		Out.ar(quickBus, ticks);
		// Out.ar(quickBus, EuCosa.ar(ticks, length: ~rhythmLength, beats: 0.7 * ~rhythmLength, thresh0: 0.99)[0]);
	}).add;

	SynthDef(\engine, { |quickBus = 0, slowBus = 0, resetBus = 0|
		var quickTrig = In.ar(quickBus);
		var slowTrig = In.ar(slowBus);
		var resetTrig = In.ar(resetBus);
		var leftHand = Justo.ar(slowTrig, maxSize: ~rhythmLength, maxRatio: 2, root: 660, maxDistance: 10, mutateAll: 0.1, seed: ~seed);
		var rightHand = Justo.ar(quickTrig, maxSize: ~rhythmLength, maxRatio: 3, root: 660, maxDistance: ~maxDist, mutateBad: 1, mutateAll: 0.1, reset: ~rhythmLength, reverse: 0, numerator: leftHand.numerator, denominator: leftHand.denominator, seed: ~seed);
		// SendReply.ar(slowTrig,  ~routerPath, [leftHand.frequency,  leftHand.distance, leftHand.numerator, leftHand.denominator, rightHand.numerator, rightHand.denominator]);
		SendReply.ar(quickTrig, ~routerPath, [rightHand.frequency, rightHand.distance, rightHand.numerator, rightHand.denominator, leftHand.distance, 1, 1]); //leftHand.numerator, leftHand.denominator]);
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

	~sendMidi = { |freq, durn, amp, midiOut, channel=0|
		var note, bend;
		# note, bend = ~freq2BentMidi.value(freq);
		Routine({
			midiOut.bend(channel, bend);
			midiOut.noteOn(channel, note, amp);
			durn.wait;
			midiOut.noteOff(channel, note, 0);
		}).play(SystemClock);
	};

	OSCFunc({ |msg|
		var path, node, reply, freq, dist, num, den, ldist, lnum, lden, durn, amp;
		# path, node, reply, freq, dist, num, den, ldist, lnum, lden = msg;
		// [freq, dist, num, den, lnum, lden].postln;
		durn = 0.99 * 60 / ~bpm;
		amp = 127 - (127 * dist / (~maxDist + 1)).asInteger;
		[freq, durn, amp, dist, ldist].postln;
		~sendMidi.value(freq, durn, amp, ~midiOut, 0);
	}, ~routerPath);

	~clockSynth = Synth(\clock, [\quickBus, ~quickBus, \slowBus, ~slowBus, \resetBus, ~resetBus]);
	~engineSynth = Synth(\engine, [\quickBus, ~quickBus, \slowBus, ~slowBus, \resetBus, ~resetBus], addAction: \addAfter);

	"unremitting-transfer main loaded".postln;

});

