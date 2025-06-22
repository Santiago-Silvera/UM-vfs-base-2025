#!/bin/bash

# VFS Test Script
# Tests all VFS functions in a logical sequence

set -e  # Exit on any error

echo "=== VFS Filesystem Test Suite ==="
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test image file
TEST_IMG="test.img"

# Clean up any existing test image
cleanup() {
    echo -e "${YELLOW}Cleaning up...${NC}"
    rm -f "$TEST_IMG"
    if [ -f "host_file.txt" ]; then
        rm -f "host_file.txt"
    fi
    if [ -f "copied_file.txt" ]; then
        rm -f "copied_file.txt"
    fi
}

# Error handler
error_handler() {
    echo -e "${RED}Test failed at line $1${NC}"
    cleanup
    exit 1
}

trap 'error_handler $LINENO' ERR

# Function to print test status
print_status() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✓ $2${NC}"
    else
        echo -e "${RED}✗ $2${NC}"
        exit 1
    fi
}

echo "1. Testing vfs-mkfs..."
./vfs-mkfs "$TEST_IMG" 100 50
print_status $? "Created filesystem image"

echo "2. Testing vfs-info..."
./vfs-info "$TEST_IMG" > /dev/null
print_status $? "Filesystem info displayed"

echo "3. Testing vfs-touch (create files)..."
./vfs-touch "$TEST_IMG" file1.txt file2.txt file3.txt
print_status $? "Created empty files"

echo "4. Testing vfs-touch (duplicate file error)..."
if ./vfs-touch "$TEST_IMG" file1.txt 2>/dev/null; then
    echo -e "${RED}✗ Should have failed when creating duplicate file${NC}"
    exit 1
else
    echo -e "${GREEN}✓ Correctly rejected duplicate file${NC}"
fi

echo "5. Testing vfs-touch (invalid filename)..."
if ./vfs-touch "$TEST_IMG" "bad@name" 2>/dev/null; then
    echo -e "${RED}✗ Should have failed with invalid filename${NC}"
    exit 1
else
    echo -e "${GREEN}✓ Correctly rejected invalid filename${NC}"
fi

echo "6. Testing vfs-ls (basic listing)..."
./vfs-ls "$TEST_IMG" > /dev/null
print_status $? "Directory listing displayed"

echo "7. Testing vfs-ls (verify file count)..."
FILE_COUNT=$(./vfs-ls "$TEST_IMG" | wc -l)
if [ "$FILE_COUNT" -eq 5 ]; then
    echo -e "${GREEN}✓ Correct number of files listed ($FILE_COUNT)${NC}"
else
    echo -e "${RED}✗ Expected 5 files, got $FILE_COUNT${NC}"
    exit 1
fi

echo "8. Testing vfs-ls (verify specific files)..."
./vfs-ls "$TEST_IMG" | grep -q "file1.txt" && \
./vfs-ls "$TEST_IMG" | grep -q "file2.txt" && \
./vfs-ls "$TEST_IMG" | grep -q "file3.txt"
print_status $? "All expected files found in listing"

echo "9. Testing vfs-copy (copy file from host)..."
# Create a test file on host
echo "Hello, VFS" > host_file.txt
./vfs-copy "$TEST_IMG" host_file.txt copied_file.txt
print_status $? "Copied file from host to VFS"

echo "10. Testing vfs-ls (after copy)..."
NEW_FILE_COUNT=$(./vfs-ls "$TEST_IMG" | wc -l)
if [ "$NEW_FILE_COUNT" -eq 6 ]; then
    echo -e "${GREEN}✓ Correct number of files after copy ($NEW_FILE_COUNT)${NC}"
else
    echo -e "${RED}✗ Expected 6 files, got $NEW_FILE_COUNT${NC}"
    exit 1
fi

echo "11. Testing vfs-cat..."
./vfs-cat "$TEST_IMG" copied_file.txt > /dev/null
print_status $? "File content displayed"

echo "12. Testing vfs-trunc..."
./vfs-trunc "$TEST_IMG" file1.txt
print_status $? "File truncated"

echo "13. Testing vfs-rm..."
./vfs-rm "$TEST_IMG" file2.txt
print_status $? "File removed"

echo "14. Testing vfs-ls (after removal)..."
FINAL_FILE_COUNT=$(./vfs-ls "$TEST_IMG" | wc -l)
if [ "$FINAL_FILE_COUNT" -eq 5 ]; then
    echo -e "${GREEN}✓ Correct number of files after removal ($FINAL_FILE_COUNT)${NC}"
else
    echo -e "${RED}✗ Expected 5 files, got $FINAL_FILE_COUNT${NC}"
    exit 1
fi

echo "15. Final vfs-info..."
./vfs-info "$TEST_IMG" > /dev/null
print_status $? "Final filesystem info"

# Clean up test files
rm -f host_file.txt

echo
echo -e "${GREEN}=== All tests passed! ===${NC}"
echo "Filesystem image: $TEST_IMG"
echo "You can inspect it with: ./vfs-info $TEST_IMG"
echo "Or list contents with: ./vfs-ls $TEST_IMG"

# Keep the test image for manual inspection
echo -e "${YELLOW}Test image preserved for manual inspection${NC}"