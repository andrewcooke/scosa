
// Kill any existing server but don't reboot (to preserve JACK connection)
Server.killAll;
Window.closeAll;

// Configure to connect to existing JACK server (don't auto-start)
s = Server.local;
s.options.numOutputBusChannels = 2;
s.options.numInputBusChannels = 2;
s.options.blockSize = 64;
s.options.sampleRate = 48000;

s.waitForBoot({

        "loading unremitting-transfer main".postln;

        ~noteOnPath = '/justo/noteOn';
        ~noteOffPath = '/justo/noteOff';

        ~bpm = 20;             // trigger for quick notes
        ~rhythmLength = 4;     // ratio for trigger for slow notes 
        ~gap = 1000;           // downtime so triggers register
	~hold = 0.75;          // not held for full interval

        ~seed = 0;  // 0 is self-seed
	~maxDist = 10;

        ~quickOnBus = Bus.audio(s, 1);
        ~quickOffBus = Bus.audio(s, 1);
        ~slowOnBus  = Bus.audio(s, 1);
        ~slowOffBus  = Bus.audio(s, 1);
        ~resetBus = Bus.audio(s, 1);

        SynthDef(\clock, { |quickOnBus = 0, quickOffBus = 0, slowOnBus = 0, slowOffBus = 0, resetBus = 0|
                var cps = ~bpm / 60;
		var quickPeriod = 1 / cps;
		var slowPeriod = quickPeriod * ~rhythmLength;
                var ticks = Impulse.ar(cps);
		var slowBase = PulseDivider.ar(ticks, div: ~rhythmLength);
                Out.ar(quickOnBus, ticks);
                Out.ar(quickOffBus, DelayN.ar(ticks, quickPeriod, quickPeriod - (~gap * SampleDur.ir)));
                Out.ar(slowOnBus, slowBase);
                Out.ar(slowOffBus, DelayN.ar(slowBase, slowPeriod, slowPeriod - (~gap * SampleDur.ir)));
//                Out.ar(slowOffBus, DelayN.ar(slowBase, slowPeriod, slowPeriod * ~hold));
        }).add;

        SynthDef(\engine, { |quickOnBus = 0, quickOffBus = 0, slowOnBus = 0, slowOffBus = 0, resetBus = 0|
                var quickOnTrig = In.ar(quickOnBus);
                var quickOffTrig = In.ar(quickOffBus);
                var slowOnTrig = In.ar(slowOnBus);
                var slowOffTrig = In.ar(slowOffBus);
                var resetTrig = In.ar(resetBus);
                var leftHand = Justo.ar(slowOnTrig, maxSize: ~rhythmLength, maxRatio: 2, root: 110, maxDistance: 10, mutateAll: 0.1, seed: ~seed);
                var rightHand = Justo.ar(quickOnTrig, maxSize: ~rhythmLength, maxRatio: 3, root: 660, maxDistance: ~maxDist, mutateBad: 1, mutateAll: 0.1, reset: ~rhythmLength, reverse: 0, numerator: leftHand.numerator, denominator: leftHand.denominator, seed: ~seed);
                SendReply.ar(slowOffTrig,  ~noteOffPath, [0]);
                SendReply.ar(slowOnTrig,  ~noteOnPath, [leftHand.frequency,  leftHand.distance, leftHand.numerator, leftHand.denominator, 0, 0]);
                SendReply.ar(quickOffTrig,  ~noteOffPath, [1]);
                SendReply.ar(quickOnTrig, ~noteOnPath, [rightHand.frequency, rightHand.distance, rightHand.numerator, rightHand.denominator, leftHand.distance, 0, 0]); //leftHand.numerator, leftHand.denominator]);
        }).add;

        s.sync;

        // Connect to Neutron and configure
        ~neutron = Neutron.connect(channel: 0);
        if (~neutron.isNil) {
                s.quit;
                1.exit;
        };
        ~neutron.restoreDefaults.
	setParaphonic.   // must be done before key priority
        setSplitMode(57, true).
	setRetrigger(false).
//        setKeyPriority(\last).
        setKeyPriority(\low).
        setAssign(\modWheel).
	setOsc1Bypass.
	setOsc2Bypass.
	setOsc1Range(\eightFoot).
	setOsc2Range(\eightFoot).
	setVcfMode(\low).
        allNotesOff;

        ~freq2BentMidi = { |freq|
                var midi = freq.cpsmidi;
                var note = midi.round;
                var offset = midi - note;
                var bend = offset.linlin(-1.0, 1.0, 0, 16383).asInteger;
                [note.asInteger, bend.asInteger];
        };

        ~prevHigh = 0;
        ~prevLow = 0;

        OSCFunc({ |msg|
                var path, node, reply, freq, dist, num, den, ldist, lnum, lden, durn, amp, note, bend;
                # path, node, reply, freq, dist, num, den, ldist, lnum, lden = msg;
                # note, bend = ~freq2BentMidi.value(freq);
                [note, bend, dist, ldist].postln;
                if (ldist == 0) {
                        ~neutron.noteOn(note, bend, 127);
                        ~prevLow = note;
                } {
                        ~neutron.noteOn(note, bend, 127);
                        ~neutron.control(1, 127);
                        ~prevHigh = note;
                };
        }, ~noteOnPath);

        OSCFunc({ |msg|
                var path, node, reply, ldist;
                # path, node, reply, ldist = msg;
                if (ldist == 0) {
                        if (~prevLow != 0) {~neutron.noteOff(~prevLow);};
                } {
                        if (~prevHigh != 0) {
                                ~neutron.noteOff(~prevHigh);
                                ~neutron.control(1);
                        };
                };
        }, ~noteOffPath);

        ~clockSynth = Synth(\clock, [\quickOnBus, ~quickOnBus, \quickOffBus, ~quickOffBus, \slowOnBus, ~slowOnBus, \slowOffBus, ~slowOffBus, \resetBus, ~resetBus]);
        ~engineSynth = Synth(\engine, [\quickOnBus, ~quickOnBus, \quickOffBus, ~quickOffBus, \slowOnBus, ~slowOnBus, \slowOffBus, ~slowOffBus, \resetBus, ~resetBus], addAction: \addAfter);

        "unremitting-transfer main loaded".postln;

});

