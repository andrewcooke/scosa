#!/bin/bash
# Start JACK with dummy audio driver for SuperCollider
# Run this before starting SuperCollider compositions

# Kill any existing JACK instances (including dbus-launched ones)
killall jackd 2>/dev/null
killall jackdbus 2>/dev/null
sleep 1

# Clean up any stale JACK sockets
rm -f /dev/shm/jack* 2>/dev/null
rm -f /tmp/jack* 2>/dev/null

# Start JACK with dummy driver in background
# Use -T to avoid trying to use realtime scheduling
jackd -T -d dummy -r 48000 -p 1024 &

# Wait for JACK to initialize
sleep 3

# Verify JACK is running
if pgrep -x jackd > /dev/null; then
    echo "✓ JACK dummy driver started successfully."
    echo "You can now run SuperCollider."
    echo "To stop JACK later, run: killall jackd"
else
    echo "✗ Failed to start JACK. Check the error messages above."
    exit 1
fi
