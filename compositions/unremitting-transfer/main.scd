
s.reboot;

Window.closeAll;

s.waitForBoot({

	//    var rootDir = thisProcess.nowExecutingPath.dirname +/+ "../..";
	//    var systemDir = rootDir +/+ "system";

	"loading unremitting-transfer main".postln;

	~pianoBus = Bus.audio(s, 2);

	SynthDef(\pianoSoundBoard, { |inBus = 0, out = 0|
		var input, resonance;
		input = In.ar(inBus, 1);
		resonance = OteySoundBoard.ar(input);
		Out.ar(out, resonance);
	}).add;

	s.sync;

	~soundBoardNode = Synth.tail(s, \pianoSoundBoard, [\inBus, ~pianoBus, \out, 0]);

	SynthDef(\pianoString, { |outBus = 0, freq = 440, vel = 0.5, t_gate = 1|
		var stringSig, env;
		env = EnvGen.ar(Env.perc(0.0001, 4.0), doneAction: Done.freeSelf);
		stringSig = OteyPianoStrings.ar(freq, vel, t_gate: t_gate);
		// Out.ar(outBus, Pan2.ar(stringSig * env * 0.15, LinLin.kr(freq, 36.midicps, 90.midicps, -0.6, 0.6)));
		Out.ar(outBus, stringSig * env);
	}).add;

	SynthDef(\engine, {
		var bpm = 120;
		var cps = bpm / 60;
		var trig = Impulse.ar(cps);
		var freq = Justo.ar(trig);
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

