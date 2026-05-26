
// 808 emulation from google gemini

(

"loading 808 voice library".postln;

SynthDef(\tr808_kick, { |out = 0, freq = 55, amp = 0.5, pan = 0, velocity = 1|
    var env, freqEnv, sig;
    env = EnvGen.ar(Env.perc(0.001, 1.5), doneAction: Done.freeSelf);
    freqEnv = EnvGen.ar(Env.perc(0.001, 0.04)) * 180 + freq;
    sig = SinOsc.ar(freqEnv) * velocity.linexp(0, 1, 0.5, 1.5);
    sig = (sig * 1.1).clip2(1.0);
    Out.ar(out, Balance2.ar(sig, sig, pan, env * amp));
}).add;

SynthDef(\tr808_snare, { |out = 0, freq = 180, amp = 0.5, pan = 0, velocity = 1|
    var toneEnv, noiseEnv, tone, noise, sig;
    toneEnv = EnvGen.ar(Env.perc(0.001, 0.1), doneAction: Done.none);
    noiseEnv = EnvGen.ar(Env.perc(0.001, 0.25), doneAction: Done.freeSelf);
    
    tone = SinOsc.ar(freq) * 0.6 + SinOsc.ar(freq * 1.83) * 0.4;
    tone = tone * toneEnv;
    
    noise = WhiteNoise.ar() * noiseEnv;
    noise = BBandPass.ar(noise, 1500, 0.5);
    
    sig = (tone + (noise * 1.2)) * velocity;
    Out.ar(out, Balance2.ar(sig, sig, pan, amp));
}).add;

SynthDef(\tr808_closed_hat, { |out = 0, amp = 0.5, pan = 0, velocity = 1|
    var env, pulseGroup, sig;
    env = EnvGen.ar(Env.perc(0.001, 0.05), doneAction: Done.freeSelf);
    
    // 6 detuned square waves
    pulseGroup = [300, 430, 520, 680, 890, 1020].collect { |f| Pulse.ar(f, 0.5) }.sum;

    sig = HPF.ar(pulseGroup, 7000);
    sig = BBandPass.ar(sig, 10000, 0.5) * velocity;
    
    Out.ar(out, Balance2.ar(sig, sig, pan, env * amp));
}).add;

SynthDef(\tr808_open_hat, { |out = 0, amp = 0.5, pan = 0, velocity = 1|
    var env, pulseGroup, sig;
    env = EnvGen.ar(Env.perc(0.001, 0.4), doneAction: Done.freeSelf);
    
    pulseGroup = [300, 430, 520, 680, 890, 1020].collect { |f| Pulse.ar(f, 0.5) }.sum;
    
    sig = HPF.ar(pulseGroup, 6500);
    sig = BBandPass.ar(sig, 9500, 0.4) * velocity;
    
    Out.ar(out, Balance2.ar(sig, sig, pan, env * amp));
}).add;

SynthDef(\tr808_cowbell, { |out = 0, freq = 540, amp = 0.5, pan = 0, velocity = 1|
    var env, pulse1, pulse2, sig;
    env = EnvGen.ar(Env.perc(0.001, 0.35), doneAction: Done.freeSelf);
    
    pulse1 = Pulse.ar(freq, 0.5);
    pulse2 = Pulse.ar(freq * 1.481, 0.5);
    
    sig = BBandPass.ar(pulse1 + pulse2, freq * 1.555, 0.4) * velocity;
    Out.ar(out, Balance2.ar(sig, sig, pan, env * amp));
}).add;

SynthDef(\tr808_clap, { |out = 0, amp = 0.5, pan = 0, velocity = 1|
    var envBurst, envTail, noise, sig;
    
    // 3 super fast spikes followed by a smooth tail
    envBurst = EnvGen.ar(Env.new([0,1,0,1,0,1,0], [0.002, 0.01, 0.002, 0.01, 0.002, 0.012]));
    envTail = EnvGen.ar(Env.perc(0.02, 0.25), doneAction: Done.freeSelf);
    
    noise = WhiteNoise.ar();
    sig = (noise * envBurst * 0.7) + (noise * envTail * 0.4);
    sig = BBandPass.ar(sig, 1100, 0.6) * velocity;
    
    Out.ar(out, Balance2.ar(sig, sig, pan, sig * amp));
}).add;

SynthDef(\tr808_tom, { |out = 0, freq = 90, amp = 0.5, pan = 0, velocity = 1, decay = 0.4|
    var env, freqEnv, sig;
    env = EnvGen.ar(Env.perc(0.002, decay), doneAction: Done.freeSelf);
    freqEnv = EnvGen.ar(Env.perc(0.001, 0.06)) * 40 + freq;
    
    sig = SinOsc.ar(freqEnv) + (HPF.ar(WhiteNoise.ar(), 1000) * 0.02 * env);
    sig = sig * velocity;
    
    Out.ar(out, Balance2.ar(sig, sig, pan, env * amp));
}).add;

"808 loaded".postln;

)
