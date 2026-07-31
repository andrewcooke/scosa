// Test SCosa plugins
// Run with: sclang tests/test_plugins.scd

"Starting SCosa plugin tests...".postln;

s = Server.local;

s.waitForBoot({
    "Server booted successfully".postln;
    
    // Check if plugins are loaded
    "".postln;
    "Checking for EuCosa...".postln;
    if(EuCosa.notNil, {
        "✓ EuCosa found".postln;
        EuCosa.dumpInterface;
    }, {
        "✗ EuCosa NOT found".postln;
    });
    
    "".postln;
    "Checking for TrigGate...".postln;
    if(TrigGate.notNil, {
        "✓ TrigGate found".postln;
        TrigGate.dumpInterface;
    }, {
        "✗ TrigGate NOT found".postln;
    });
    
    "".postln;
    "Checking for Justo...".postln;
    if(Justo.notNil, {
        "✓ Justo found".postln;
        Justo.dumpInterface;
    }, {
        "✗ Justo NOT found".postln;
    });
    
    "".postln;
    "All checks complete".postln;
    
    1.wait;
    s.quit;
    0.exit;
});
