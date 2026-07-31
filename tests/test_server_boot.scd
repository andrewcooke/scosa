// Test server boot with dummy audio driver
// Run with: sclang tests/test_server_boot.scd

"Testing server boot with dummy audio...".postln;

s = Server.local;

// Use scsynth's internal null audio driver - no JACK or hardware needed
Server.program = Server.program.replace("scsynth", "scsynth -H null");
s.options.numOutputBusChannels = 2;
s.options.numInputBusChannels = 0;
s.options.blockSize = 64;
s.options.sampleRate = 48000;

s.waitForBoot({
    "✓ Server booted successfully!".postln;
    "Server options:".postln;
    s.options.dump;
    
    1.wait;
    s.quit;
    0.exit;
});
