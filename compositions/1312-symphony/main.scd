
s.reboot;

Window.closeAll;

s.waitForBoot({

    var rootDir = thisProcess.nowExecutingPath.dirname +/+ "../..";
    var systemDir = rootDir +/+ "system";

	"loading 1312-symphony main".postln;

	// clean up synths if re-running
	~eucosa4Source.free;
	~eucosa12Source.free;

    (systemDir +/+ "voices/808.scd").load;
    (systemDir +/+ "clocks/base.scd").load;
    (systemDir +/+ "clocks/eucosa.scd").load;

    s.sync;  // wait for server to catch up

	~measure = 16;
	~subSteps = 4;
    ~triplets = 3;

	// ~scopeBus = Bus.control(s, 1);

	~fourBus = Bus.audio(s, 1);
	~dividerSynth.free;
	~dividerSynth = {
		Out.ar(~fourBus, PulseDivider.ar(In.ar(~clockBus, 1), ~triplets))
	}.play(target: s, addAction: \addToHead);

	~eucosa4Source = Synth(\eucosaSource, [
		\clockBus, ~fourBus,
		// \scopeBus, ~scopeBus,
		\length, ~measure,
		\beats, 7,
		\thresh0, 0.0001,
		\thresh1, 0.01
	], target: s, addAction: \addToTail);

	~eucosa12Source = Synth(\eucosaSource, [
		\clockBus, ~clockBus,
		\length, ~measure * ~triplets,
		\beats, 35,
		\thresh0, 0.1,
		\thresh1, 0.05,
	], target: s, addAction: \addToTail);

	~node4 = ~eucosa4Source.nodeID;
	~node12 = ~eucosa12Source.nodeID;

	~eucosaRouter.free;
	~eucosaRouter = OSCdef(\eucosaRouter, { |msg|
		var path, nodeId, replyId, err;
		msg.postln;
		#path, nodeId, replyId, err = msg;
		[\incoming, nodeId, replyId, err].postln;
		s.makeBundle(nil, {
			case
			{ (nodeId == ~node4)  && (replyId == 0) } { Synth(\tr808_kick,       [\amp, 1.0, \pan, -0.3]) }
			{ (nodeId == ~node4)  && (replyId == 1) } { Synth(\tr808_snare,      [\amp, 1.0, \pan, 0.3]) }
			{ (nodeId == ~node4)  && (replyId == 2) } { }
			{ (nodeId == ~node12) && (replyId == 0) } { Synth(\tr808_closed_hat, [\amp, 1.0, \pan, -0.3]) }
			{ (nodeId == ~node12) && (replyId == 1) } { Synth(\tr808_open_hat,   [\amp, 1.0, \pan, 0.3]) }
			{ (nodeId == ~node12) && (replyId == 2) } { }
			{ true } { };
		});
	}, '/eucosa');

	s.sync;

	~baseClock.value(tempoBpm: 120, subSteps: ~subSteps * ~triplets);  // sets ~clockBus

	"1312-symphony main loaded".postln;

});

// ~clockScope = Stethoscope.new(s, numChannels: 1, index: ~clockBus.index, rate: \audio);
// ~clockScope.window.name = "Clock Bus Scope";
//
// ~triggerScope = Stethoscope.new(s, numChannels: 1, index: ~scopeBus.index, rate: \audio);
// ~triggerScope.window.name = "Eucosa Trigger Scope";

// s.reboot;
// "/home/andrew/scosa/system/voices/808.scd".load;
// s.makeBundle(nil, {
// 	"Testing 808 Kick...".postln;
// 	Synth(\tr808_kick,  [\amp, 1.0, \pan, -0.3]);
// });
// s.makeBundle(nil, {
// 	"Testing 808 Snare...".postln;
// 	Synth(\tr808_snare, [\amp, 1.0, \pan, 0.3]);
// });
