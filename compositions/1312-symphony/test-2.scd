
s.boot;

~x.free;

SynthDef(\selectFirstTrig, { |clockFreq = 8, synthFreq = 440|
    var rawTriggers, multiChannelFilter, trigger0, env0, sig0, trigger1, env1, sig1;
    rawTriggers = Impulse.ar(clockFreq);
    //multiChannelFilter = PulseDivider.ar(rawTriggers, div: [2, 3, 4, 5]);
	multiChannelFilter = EuCosa.ar(rawTriggers, 16, 13, 0.1, 0.08);
    trigger0 = multiChannelFilter[0];
    env0 = EnvGen.ar(Env.perc(0.001, 0.1), gate: trigger0);
    sig0 = SinOsc.ar(synthFreq) * env0 * 0.5;
    trigger1 = multiChannelFilter[1];
    env1 = EnvGen.ar(Env.perc(0.001, 0.1), gate: trigger1);
    sig1 = SinOsc.ar(synthFreq * 2) * env1 * 0.25;
	Out.ar(0, (sig0 + sig1) ! 2);
}).add;

~x = Synth(\selectFirstTrig, [\clockFreq, 8, \synthFreq, 440]);

