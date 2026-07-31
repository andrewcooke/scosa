// Test server boot with dummy audio driver
// Run with: sclang tests/test_server_boot.scd

"Testing server boot with dummy audio...".postln;

s = Server.local;

// Configure to use JACK dummy driver - no actual audio hardware needed
s.options.device = "dummy";
s.options.numOutputBusChannels = 2;
s.options.numInputBusChannels = 0;
s.options.hardwareBufferSize = 1024;
s.options.sampleRate = 48000;

s.waitForBoot({
    "✓ Server booted successfully!".postln;
    "Server options:".postln;
    s.options.dump;
    
    1.wait;
    s.quit;
    0.exit;
});
