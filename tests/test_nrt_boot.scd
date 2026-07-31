// Test Non-Realtime server boot - no audio hardware needed
// Run with: sclang tests/test_nrt_boot.scd

"Testing NRT (Non-Realtime) server boot...".postln;

// Use the NRT server instead of the realtime server
s = Server.default;

// Configure for NRT mode
s.options.numOutputBusChannels = 2;
s.options.numInputBusChannels = 0;
s.options.blockSize = 64;
s.options.sampleRate = 48000;

// Boot in NRT mode by using Server.internal which doesn't need JACK
Server.default = Server.internal;
s = Server.default;

s.waitForBoot({
    "✓ Server booted successfully in NRT mode!".postln;
    "Server is running without audio hardware".postln;
    s.options.dump;
    
    1.wait;
    s.quit;
    0.exit;
});
