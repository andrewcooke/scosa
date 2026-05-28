
// simple clockBus with bpm etc from google gemini

(

"loading base clock library".postln;

~clockBus = Bus.audio(s, 1);

SynthDef(\master_engine_clock, { |outBus, tempoHz = 8|
    var clockPulse;
    clockPulse = Impulse.ar(tempoHz);
    Out.ar(outBus, clockPulse);
}).add;

// initalisation wrapper
~baseClock = { |tempoBpm = 120, subSteps = 4|
    var hzValue;
    hzValue = (tempoBpm / 60) * subSteps;

    "starting base clock".postln;

    ~clockSource.free;  // clear any previous clock
    ~clockSource = Synth(\master_engine_clock, [
        \outBus, ~clockBus,
        \tempoHz, hzValue
    ], target: s, addAction: \addToHead);  // starts before anything else
};

"base loaded".postln;

)
