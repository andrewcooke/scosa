
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
		resonance = OteySoundBoard.ar(input, c1: 20, c3: 20, mix: 0.5);
		Out.ar(outBus, resonance);
	}).add;

	s.sync;

	~soundBoardNode = Synth.tail(s, \pianoSoundBoard, [\inBus, ~pianoBus, \outBus, 0]);

	SynthDef(\pianoString, { |outBus = 0, freq = 440, vel = 0.1, t_gate = 1|
		var string, env;
		env = EnvGen.ar(Env.perc(0.0001, 4.0), doneAction: Done.freeSelf);
		string = OteyPianoStrings.ar(freq, vel, t_gate: t_gate);
		Out.ar(outBus, (string * env) ! 2);
	}).add;

	SynthDef(\engine, {
		var bpm = 480;
		var cps = bpm / 60;
		var trig = Impulse.ar(cps);
		//var mutate = DC.ar(0.0);
		var mutate = CoinGate.ar(0.2, trig);
		var freq = Justo.ar(trig, maxSize: 12, mutate: mutate, root: 660);
		SendReply.ar(trig, '/justo/note', [trig, freq]);
	}).add;

	OSCdef(\relay, { |msg|
		var freq = msg[4];
		freq.postln;
		if (freq.notNil and: { freq > 0 }) {
			s.bind {
				Synth.head(s, \pianoString, [
					\outBus, ~pianoBus,
					\freq, freq
				]);
			};
		};
	}, '/justo/note');

	s.sync;

	~engine = Synth(\engine);

	"unremitting-transfer main loaded".postln;

});

