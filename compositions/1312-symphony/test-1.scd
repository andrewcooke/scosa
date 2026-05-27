
s.waitForBoot({
    var rootDir = thisProcess.nowExecutingPath.dirname +/+ "../..";
    var systemDir = rootDir +/+ "system";
    "--- RUNNING ISOLATED PLUGIN SANITY CHECK ---".postln;
    (systemDir +/+ "clocks/eucosa.scd").load;

    // 1. Allocate a fresh, clean bus
    ~testBus = Bus.audio(s, 1);
    s.sync;

    // 2. Launch ONLY ONE instance with perfectly standard, non-polymetric numbers
    ~testSynth = Synth(\eucosaSource, [
        \clockBus, ~testBus,
        \length, 16,        // Standard 16-step grid
        \beats, 4,         // Standard 4 beats
        \thresh0, 0.25,
        \thresh1, 0.35
    ], target: s, addAction: \addToTail);

    s.sync;
    "--- IF YOU SEE THIS, A SINGLE 16-STEP INSTANCE WORKS! ---".postln;
});
