
s.reboot;

Window.closeAll;

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

	~measure = 16;
	~subSteps = 4;

	// ~scopeBus = Bus.control(s, 1);

	~eucosa0Source = Synth(\eucosaSource, [
		\clockBus, ~clockBus,
		// \scopeBus, ~scopeBus,
		\length, ~measure,
		\beats, 7,
		\thresh0, 0.25,
		\thresh1, 0.15
	], target: s, addAction: \addToTail);

	~eucosa1Source = Synth(\eucosaSource, [
		\clockBus, ~clockBus,
		\length, ~measure,
		\beats, 15,
		\thresh0, 0.1,
		\thresh1, 0.05,
	], target: s, addAction: \addToTail);

	~node0 = ~eucosa0Source.nodeID;
	~node1 = ~eucosa1Source.nodeID;

	~eucosaRouter.free;
	~eucosaRouter = OSCdef(\eucosaRouter, { |msg|
		var path, nodeId, replyId, err;
		msg.postln;
		#path, nodeId, replyId, err = msg;
		// err = msg[3];
		[\incoming, nodeId, replyId, err].postln;
		s.makeBundle(nil, {
			case
			{ (nodeId == ~node0) && (replyId == 0) } { Synth(\tr808_kick,       [\amp, 1.0, \pan, -0.3]) }
			{ (nodeId == ~node0) && (replyId == 1) } { Synth(\tr808_snare,      [\amp, 1.0, \pan, 0.3]) }
			{ (nodeId == ~node0) && (replyId == 2) } { }
			{ (nodeId == ~node1) && (replyId == 0) } { Synth(\tr808_closed_hat, [\amp, 1.0, \pan, -0.3]) }
			{ (nodeId == ~node1) && (replyId == 1) } { Synth(\tr808_open_hat,   [\amp, 1.0, \pan, 0.3]) }
			{ (nodeId == ~node1) && (replyId == 2) } { }
			{ true } { };
		});
	}, '/eucosa');

	s.sync;
	~baseClock.value(tempoBpm: 120, subSteps: ~subSteps);

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
