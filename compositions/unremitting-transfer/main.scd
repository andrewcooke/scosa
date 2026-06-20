
s.reboot;

Window.closeAll;

s.waitForBoot({

	//    var rootDir = thisProcess.nowExecutingPath.dirname +/+ "../..";
	//    var systemDir = rootDir +/+ "system";

	"loading unremitting-transfer main".postln;

	~routerPath = '/justo/note';

	~bpm = 400;
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
		var mutateSlowly = CoinGate.ar(0.1, slowTrig);
		var mutateQuickly = DC.ar(1);
		var leftHand = Justo.ar(slowTrig, maxSize: ~rhythmLength / ~slowDivider, root: 110, maxDistance: 10, mutate: mutateSlowly, reset: DC.ar(0), numerator: DC.ar(1), denominator: DC.ar(1), seed: ~seed);
		var rightHand = Justo.ar(quickTrig, maxSize: ~rhythmLength, root: 440, maxDistance: 10, mutate: mutateQuickly, reset: resetTrig, numerator: leftHand.numerator, denominator: leftHand.denominator, seed: ~seed);
		SendReply.ar(slowTrig,  ~routerPath, [leftHand.frequency,  0.1, 0.5, leftHand.distance]);
		SendReply.ar(quickTrig, ~routerPath, [rightHand.frequency, 1.0, 1.0, rightHand.distance]);
	}).add;

	SynthDef(\pianoSoundBoard, { |inBus = 0, outBus = 0|
		var input, resonance;
		input = In.ar(inBus, 2);
		resonance = OteySoundBoard.ar(input, c1: 40, c3: 60, mix: 0.4);
		Out.ar(outBus, resonance);
	}).add;

	SynthDef(\pianoString, { |outBus = 0, freq = 440, vel = 0.1, loss = 1.0, t_gate = 1|
		var string, env;
		env = EnvGen.ar(Env.perc(0.0001, 4.0), doneAction: Done.freeSelf);
		string = OteyPianoStrings.ar(freq, vel, loss: loss, t_gate: t_gate, hammer_type: 2);
		Out.ar(outBus, (string * env) ! 2);
	}).add;

	SynthDef(\masterCompressor, { |inBus = 10, outBus = 12, thresh = 0.5, slopeBelow = 1,
		                           slopeAbove = 0.3, clamp = 0.01, relax = 0.1|
		var sig = In.ar(inBus, 2);
		sig = Compander.ar(sig, sig, thresh, slopeBelow, slopeAbove, clamp, relax);
		XOut.ar(outBus, 1.0, sig);
	}).add;

	SynthDef(\masterReverb, { |inBus = 12, outBus = 0, mix = 0.33, room = 0.5, damp = 0.5|
		var sig = In.ar(inBus, 2);
		var rev = FreeVerb.ar(sig, mix, room, damp);
		ReplaceOut.ar(outBus, rev);
	}).add;

	s.sync;

	~quickBus = Bus.audio(s, 1);
	~slowBus = Bus.audio(s, 1);
	~resetBus = Bus.audio(s, 1);
	~pianoBus = Bus.audio(s, 2);
	~compBus = Bus.audio(s, 2);
	~revBus  = Bus.audio(s, 2);

	~sourceGroup = Group.new(s);
	~fxGroup     = Group.after(~sourceGroup); // FX group always runs AFTER sources

	~clockNode = Synth.head(~sourceGroup, \clock, [\quickBus, ~quickBus, \slowBus, ~slowBus]);
	~engineNode = Synth.after(~clockNode, \engine, [\quickBus, ~quickBus, \slowBus, ~slowBus]);

	OSCdef(\router, { |msg|
		var path, node, reply, freq, loss, amp, dist;
		# path, node, reply, freq, loss, amp, dist = msg;
		[freq, loss, amp, dist].postln;
		s.bind {
			Synth.head(~sourceGroup, \pianoString, [
				\outBus, ~pianoBus,
				\freq, freq,
				\loss, loss,
				\vel, amp * (0.3 + (0.7 * 2 / dist))
			]);
		};
	}, ~routerPath);

	~soundBoardNode = Synth.tail(~sourceGroup, \pianoSoundBoard, [\inBus, ~pianoBus, \outBus, ~compBus]);
	~compNode   = Synth(\masterCompressor, [\inBus, ~compBus, \outBus, ~revBus, \thresh, 0.4], ~fxGroup, \addToHead);
	~reverbNode = Synth(\masterReverb, [\inBus, ~revBus, \outBus, 0, \mix, 0.4, \room, 0.7], ~fxGroup, \addToTail);

	"unremitting-transfer main loaded".postln;

});

