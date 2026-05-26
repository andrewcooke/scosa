
s.waitForBoot({

    var rootDir = thisProcess.nowExecutingPath.dirname +/+ "../..";
    var systemDir = rootDir +/+ "system";

	"loading 1312-symphony main".postln;

	// clean up synths if re-running
	~eucosa0Source.free;
	~eucosa1Source.free;

    (systemDir +/+ "voices/808.scd").load;
    (systemDir +/+ "clocks/base.scd").load;
    (systemDir +/+ "clocks/eucosa.scd").load;

    s.sync;  // wait for server to catch up

	~resolution = 16;
	~baseClock.value(tempoBpm: 120, stepsPerBeat: ~resolution);

	~eucosa0Router.free;
	~eucosa0Router = OSCdef(\eucosa0Router, { |msg|
		var path, id, err;
		#path, id, err = msg;
		s.makeBundle(nil, {
			case
			{ id == 0 } { Synth(\tr808_kick,  [\amp, 0.6, \pan, -0.3]) }
			{ id == 1 } { Synth(\tr808_snare, [\amp, 0.4, \pan, 0.3]) }
			{ id == 2 } { };  // ignore for now
		});
	}, '/eucosa0', s);

	~eucosa1Router.free;
	~eucosa1Router = OSCdef(\eucosa1Router, { |msg|
		var path, id, err;
		#path, id, err = msg;
		s.makeBundle(nil, {
			case
			{ id == 0 } { Synth(\tr808_kick,  [\amp, 0.6, \pan, -0.3]) }
			{ id == 1 } { Synth(\tr808_snare, [\amp, 0.4, \pan, 0.3]) }
			{ id == 2 } { };  // ignore for now
		});
	}, '/eucosa1', s);

	s.sync;

	~eucosa0Source = Synth(\eucosa_source, [
		\clockBus, ~clockBus,
		\length, 16,
		\beats, 4,
		\thresh0, 0.25,
		\thresh1, 0.35,
		\path, '/eucosa0'
	], target: s, addAction: \addToTail);

	~eucosa1Source = Synth(\eucosa_source, [
		\clockBus, ~clockBus,
		\length, 12,
		\beats, 3,
		\thresh0, 0.5,
		\thresh1, 0.15,
		\path, '/eucosa1'
	], target: s, addAction: \addToTail);

	"1312-symphony main loaded".postln;

});
