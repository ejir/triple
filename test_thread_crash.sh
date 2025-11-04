#!/bin/bash

echo "=== Thread Crash Test ==="
echo "Starting application..."

# Start the app in the background
./app > test_output.log 2>&1 &
APP_PID=$!

# Wait for app to start
sleep 2

# Check if app is running
if ! ps -p $APP_PID > /dev/null; then
    echo "ERROR: App failed to start"
    cat test_output.log
    exit 1
fi

echo "App started with PID: $APP_PID"

# Test 1: Create a thread
echo ""
echo "Test 1: Creating a thread..."
RESPONSE=$(curl -s -X POST http://127.0.0.1:8080/thread -d "board_id=1&subject=Test+Thread&author=Tester&content=Test+Content")
if echo "$RESPONSE" | grep -q "Thread Created"; then
    echo "✓ Thread created successfully"
else
    echo "✗ Failed to create thread"
    kill $APP_PID
    exit 1
fi

# Wait a bit
sleep 1

# Check if app is still running
if ! ps -p $APP_PID > /dev/null; then
    echo "✗ App crashed after creating thread"
    cat test_output.log
    exit 1
fi

# Test 2: Access the thread (this was causing SIGSEGV)
echo ""
echo "Test 2: Accessing the thread (critical test)..."
RESPONSE=$(curl -s http://127.0.0.1:8080/thread?id=1)
if echo "$RESPONSE" | grep -q "Test Thread"; then
    echo "✓ Thread accessed successfully (no crash!)"
else
    echo "✗ Failed to access thread"
    kill $APP_PID
    exit 1
fi

# Wait a bit
sleep 1

# Check if app is still running (most important check)
if ! ps -p $APP_PID > /dev/null; then
    echo "✗ App crashed when accessing thread (SIGSEGV)"
    cat test_output.log
    exit 1
fi

echo "✓ App is still running after accessing thread"

# Test 3: Create multiple threads and access them
echo ""
echo "Test 3: Creating and accessing multiple threads..."
for i in {2..5}; do
    curl -s -X POST http://127.0.0.1:8080/thread -d "board_id=1&subject=Thread$i&author=User$i&content=Content$i" > /dev/null
    sleep 0.5
    curl -s http://127.0.0.1:8080/thread?id=$i > /dev/null
    sleep 0.5
done

# Final check
if ps -p $APP_PID > /dev/null; then
    echo "✓ All tests passed! App is stable."
    kill $APP_PID
    exit 0
else
    echo "✗ App crashed during stress test"
    cat test_output.log
    exit 1
fi
