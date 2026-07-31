// Test SCosa plugins
// Run with: sclang tests/test_plugins.scd

"Starting SCosa plugin tests...".postln;
"".postln;

// Check if plugin classes are loaded (no server boot needed)
"Checking for EuCosa...".postln;
if(EuCosa.notNil, {
    "✓ EuCosa class found".postln;
    EuCosa.dumpInterface;
}, {
    "✗ EuCosa NOT found".postln;
});

"".postln;
"Checking for TrigGate...".postln;
if(TrigGate.notNil, {
    "✓ TrigGate class found".postln;
    TrigGate.dumpInterface;
}, {
    "✗ TrigGate NOT found".postln;
});

"".postln;
"Checking for Justo...".postln;
if(Justo.notNil, {
    "✓ Justo class found".postln;
    Justo.dumpInterface;
}, {
    "✗ Justo NOT found".postln;
});

"".postln;
"All checks complete".postln;

0.exit;
