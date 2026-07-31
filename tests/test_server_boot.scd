// Test server boot with dummy audio driver
// Run with: sclang tests/test_server_boot.scd

"Testing server boot with dummy audio...".postln;

s = Server.local;

// Configure for dummy audio - no actual hardware needed
s.options.device = nil;  // Use default
s.options.numOutputBusChannels = 2;
s.options.numInputBusChannels = 0;

s.waitForBoot({
    "✓ Server booted successfully!".postln;
    "Server options:".postln;
    s.options.dump;
    
    1.wait;
    s.quit;
    0.exit;
});
