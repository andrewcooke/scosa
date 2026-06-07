
s.reboot;

Window.closeAll;

s.waitForBoot({

	//    var rootDir = thisProcess.nowExecutingPath.dirname +/+ "../..";
	//    var systemDir = rootDir +/+ "system";

	"loading unremitting-transfer main".postln;

	~pianoBus = Bus.audio(s, 2);

	SynthDef(\pianoSoundBoard, { |inBus = 0, outBus = 0|
		var input, resonance;
		input = In.ar(inBus, 2);
		resonance = OteySoundBoard.ar(input, c1: 40, c3: 60, mix: 0.4);
		Out.ar(outBus, resonance);
	}).add;

	s.sync;

	SynthDef(\pianoString, { |outBus = 0, freq = 440, vel = 0.1, loss = 1.0, t_gate = 1|
		var string, env;
		env = EnvGen.ar(Env.perc(0.0001, 4.0), doneAction: Done.freeSelf);
		string = OteyPianoStrings.ar(freq, vel, loss: loss, t_gate: t_gate, hammer_type: 2);
		Out.ar(outBus, (string * env) ! 2);
	}).add;

	SynthDef(\engine, {
		var freq, num, denom;
		var bpm = 320;
		var cps = bpm / 60;
		var quick = Impulse.ar(cps);
		var slow = PulseDivider.ar(quick, div: 4);
		var mutateSlowly = CoinGate.ar(0.002, quick);
		var mutateQuickly = DC.ar(1);
		var leftHand = Justo.ar(slow, maxSize: 32,  root: 110, mutate: mutateSlowly, numerator: DC.ar(1), denominator: DC.ar(1));
		var rightHand = Justo.ar(quick, maxSize: 32,  root: 440, mutate: mutateQuickly, numerator: leftHand.numerator, denominator: leftHand.denominator);
		SendReply.ar(slow, '/justo/note', [leftHand.frequency, 0.03, 2.0 / leftHand.distance]);
		SendReply.ar(quick, '/justo/note', [rightHand.frequency, 1.0, 2.0 / rightHand.distance]);
	}).add;

	OSCdef(\relay, { |msg|
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
	}, '/justo/note');

	s.sync;

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

	~compBus = Bus.audio(s, 2);
	~revBus  = Bus.audio(s, 2);

	~sourceGroup = Group.new(s);
	~fxGroup     = Group.after(~sourceGroup); // FX group always runs AFTER sources

	~reverbNode = Synth(\masterReverb, [\inBus, ~revBus, \outBus, 0, \mix, 0.4, \room, 0.7], ~fxGroup, \addToTail);
	~compNode   = Synth(\masterCompressor, [\inBus, ~compBus, \outBus, ~revBus, \thresh, 0.4], ~fxGroup, \addToHead);

	~soundBoardNode = Synth.before(~compNode, \pianoSoundBoard, [\inBus, ~pianoBus, \outBus, ~compBus]);

	~engine = Synth.head(~sourceGroup, \engine);

	"unremitting-transfer main loaded".postln;

});

