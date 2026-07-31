// Test server boot with dummy audio driver
// Run with: SC_JACK=0 sclang tests/test_server_boot.scd

"Testing server boot with dummy audio...".postln;

// Kill any existing server
Server.killAll;

s = Server.local;

// Try to use default ALSA device (not JACK)
s.options.device = nil;
s.options.numOutputBusChannels = 2;
s.options.numInputBusChannels = 2;
s.options.blockSize = 64;
s.options.sampleRate = 48000;

s.waitForBoot({
    "✓ Server booted successfully!".postln;
    "Server running with ALSA".postln;
    s.options.dump;
    
    1.wait;
    s.quit;
    0.exit;
});
