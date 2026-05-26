
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

	~eucosa0Source = Synth(\eucosaSource, [
		\clockBus, ~clockBus,
		\length, ~measure,
		\beats, 4,
		\thresh0, 0.25,
		\thresh1, 0.35
	], target: s, addAction: \addToTail);

	~eucosa1Source = Synth(\eucosaSource, [
		\clockBus, ~clockBus,
		\length, ~measure,
		\beats, 15,
		\thresh0, 0.5,
		\thresh1, 0.15,
	], target: s, addAction: \addToTail);

	~node0 = ~eucosa0Source.nodeID;
	~node1 = ~eucosa1Source.nodeID;

	~eucosaRouter.free;
	~eucosaRouter = OSCdef(\eucosaRouter, { |msg|
		var path, nodeId, replyId, err;
		#path, nodeId, replyId = msg;
		err = msg[3];
		[\incoming, nodeId, replyId, err].postln;
		s.makeBundle(nil, {
			case
			{ (nodeId == ~node0) && (replyId == 0) } { Synth(\tr808_kick,  [\amp, 0.6, \pan, -0.3]) }
			{ (nodeId == ~node0) && (replyId == 1) } { Synth(\tr808_snare, [\amp, 0.4, \pan, 0.3]) }
			{ (nodeId == ~node0) && (replyId == 2) } { }
			{ (nodeId == ~node1) && (replyId == 0) } { Synth(\tr808_kick,  [\amp, 0.6, \pan, -0.3]) }
			{ (nodeId == ~node1) && (replyId == 1) } { Synth(\tr808_snare, [\amp, 0.4, \pan, 0.3]) }
			{ (nodeId == ~node1) && (replyId == 2) } { }
			{ true } { };
		});
	}, '/eucosa', s);

	s.sync;
	~baseClock.value(tempoBpm: 120);

	"1312-symphony main loaded".postln;

});
