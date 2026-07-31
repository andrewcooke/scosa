#!/bin/bash
# Start JACK with dummy audio driver for SuperCollider
# Run this before starting SuperCollider compositions

# Kill any existing JACK instances
killall jackd 2>/dev/null

# Start JACK with dummy driver in background
jackd -d dummy -r 48000 -p 1024 &

# Wait for JACK to initialize
sleep 2

echo "JACK dummy driver started. You can now run SuperCollider."
echo "To stop JACK later, run: killall jackd"
