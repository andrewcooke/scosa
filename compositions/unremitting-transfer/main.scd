
s.reboot;

Window.closeAll;

s.waitForBoot({

	//    var rootDir = thisProcess.nowExecutingPath.dirname +/+ "../..";
	//    var systemDir = rootDir +/+ "system";

	"loading unremitting-transfer main".postln;

	~routerPath = '/justo/note';

	~bpm = 320;
	~measure = 64;
	~ratio = 4;
	~seed = 1;

	s.randSeed = ~seed;

	SynthDef(\clock, { |quickBus = 0, slowBus = 0|
		var cps = ~bpm / 60;
		var ticks = Impulse.ar(cps);
		Out.ar(slowBus, PulseDivider.ar(ticks, div: ~ratio));
		Out.ar(quickBus, EuCosa.ar(ticks, length: ~measure, beats: 0.77 * ~measure, thresh0: 0.9)[0]);
	}).add;

	SynthDef(\engine, { |quickBus = 0, slowBus = 0|
		var quickTrig = In.ar(quickBus);
		var slowTrig = In.ar(slowBus);
		var mutateSlowly = CoinGate.ar(0.002, slowTrig);
		var mutateQuickly = DC.ar(1);
		var leftHand = Justo.ar(slowTrig, maxSize: ~measure / ~ratio, root: 110, mutate: mutateSlowly, numerator: DC.ar(1), denominator: DC.ar(1));
		var rightHand = Justo.ar(quickTrig, maxSize: ~measure, root: 440, mutate: mutateQuickly, numerator: leftHand.numerator, denominator: leftHand.denominator);
		SendReply.ar(slowTrig,  ~routerPath, [leftHand.frequency,  0.1, 0.2 + (0.3 * 2.0 / leftHand.distance)]);
		SendReply.ar(quickTrig, ~routerPath, [rightHand.frequency, 1.0, 0.1 + (0.9 * 2.0 / rightHand.distance)]);
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
	~pianoBus = Bus.audio(s, 2);
	~compBus = Bus.audio(s, 2);
	~revBus  = Bus.audio(s, 2);

	~sourceGroup = Group.new(s);
	~fxGroup     = Group.after(~sourceGroup); // FX group always runs AFTER sources

	~clockNode = Synth.head(~sourceGroup, \clock, [\quickBus, ~quickBus, \slowBus, ~slowBus]);
	~engineNode = Synth.after(~clockNode, \engine, [\quickBus, ~quickBus, \slowBus, ~slowBus]);

	OSCdef(\router, { |msg|
		var path, node, reply, freq, loss, amp;
		# path, node, reply, freq, loss, amp = msg;
		[freq, loss, amp].postln;
		s.bind {
			Synth.head(~sourceGroup, \pianoString, [
				\outBus, ~pianoBus,
				\freq, freq,
				\loss, loss,
				\vel, amp
			]);
		};
	}, ~routerPath);

	~soundBoardNode = Synth.tail(~sourceGroup, \pianoSoundBoard, [\inBus, ~pianoBus, \outBus, ~compBus]);
	~compNode   = Synth(\masterCompressor, [\inBus, ~compBus, \outBus, ~revBus, \thresh, 0.4], ~fxGroup, \addToHead);
	~reverbNode = Synth(\masterReverb, [\inBus, ~revBus, \outBus, 0, \mix, 0.4, \room, 0.7], ~fxGroup, \addToTail);

	"unremitting-transfer main loaded".postln;

});

