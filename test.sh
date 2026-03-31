#!/bin/bash

# Configuration
EXE="./pa4"
INPUT_DIR="examples"
EXPECTED_DIR="examples"
OUTPUT_DIR="out"
VALGRIND="valgrind -s --leak-check=full --track-origins=yes --error-exitcode=1"

# Setup
mkdir -p "$OUTPUT_DIR"
PASS=0
FAIL=0

echo "--- Compiling ---"
make clean
make pa4 || { echo "Compilation Failed"; exit 1; }
echo "----------------------------------------------------"

# Function to run a single test case
# Arguments: test_number (e.g., 0, 1, 2)
run_avl_test() {
    local n="$1"
    local infile="$INPUT_DIR/ops$n.b"
    local outfile="$OUTPUT_DIR/tree$n.b"
    local expected="$EXPECTED_DIR/tree$n.b"
    
    echo "===== Testing Case $n ====="

    # 1. Check if input exists
    if [ ! -f "$infile" ]; then
        echo "[SKIP] Input $infile not found."
        return
    fi

    # 2. Run the program and capture stdout
    # Format: ./pa4 -b <input> <output>
    ACTUAL_STDOUT=$($EXE -b "$infile" "$outfile")
    local exit_code=$?

    # 3. Verify Stdout and Exit Code (Expect 1 and SUCCESS for valid inputs)
    if [ "$ACTUAL_STDOUT" == "1" ] && [ $exit_code -eq 0 ]; then
        echo "[PASS] Output '1' and Exit Success"
    else
        echo "[FAIL] Expected stdout '1' and exit 0. Got '$ACTUAL_STDOUT' and $exit_code"
        FAIL=$((FAIL+1))
        return
    fi

    # 4. Diff the binary files
    # cmp is better than diff for binary files
    if cmp -s "$outfile" "$expected"; then
        echo "[PASS] Binary tree matches expected"
        PASS=$((PASS+1))
    else
        echo "[FAIL] Binary tree mismatch (diff $outfile $expected)"
        FAIL=$((FAIL+1))
    fi

    # 5. Valgrind Memory Check
    echo "[MEM ] Running Valgrind..."
    $VALGRIND $EXE -b "$infile" "$outfile" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "[PASS] No memory errors"
    else
        echo "[FAIL] Memory errors or leaks found"
        FAIL=$((FAIL+1))
    fi
    echo ""
}

# --- MAIN TESTING LOOP ---
# Add your test numbers here (e.g., 0 1 2 3)
TEST_CASES="0 1 2 3"

for t in $TEST_CASES; do
    run_avl_test "$t"
done

# --- ERROR HANDLING TEST (Expected -1) ---
echo "===== Testing File Not Found (-1) ====="
OUT_ERR=$($EXE -b "non_existent_file.b" "$OUTPUT_DIR/error.b")
if [ "$OUT_ERR" == "-1" ]; then
    echo "[PASS] Correctly handled missing file with -1"
    PASS=$((PASS+1))
else
    echo "[FAIL] Expected -1 for missing file, got '$OUT_ERR'"
    FAIL=$((FAIL+1))
fi

echo "----------------------------------------------------"
echo "TOTAL CORRECT: $PASS"
echo "TOTAL ERRORS:  $FAIL"

if [ $FAIL -eq 0 ]; then
    echo "ALL TESTS PASSED"
    exit 0
else
    echo "SOME TESTS FAILED"
    exit 1
fi