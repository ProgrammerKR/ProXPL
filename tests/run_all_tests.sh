#!/usr/bin/env bash
# ==============================================================================
# ProXPL Automated Test Runner
# ==============================================================================
# Usage:
#   ./tests/run_all_tests.sh [options] [category]
#
# Categories:
#   all         - Run all test suites (default)
#   language    - Run core language tests (tests/language/)
#   integration - Run stdlib & integration tests (tests/integration/)
#   iop         - Run Intent/Context/ASR tests (tests/iop/)
#   misc        - Run miscellaneous feature tests (tests/misc/)
#   repro       - Run regression reproduction tests (tests/repro/)
#   vm          - Run C/C++ VM unit tests (requires build)
#
# Options:
#   --bin PATH  - Path to proxpl binary (default: auto-detect)
#   --verbose   - Show output of passing tests as well
#   --quick     - Stop on first failure
#   --help      - Show this help message
# ==============================================================================

set -euo pipefail

# --- Color definitions ---
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m' # No Color

# --- Paths ---
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

# --- Default options ---
CATEGORY="all"
VERBOSE=false
STOP_ON_FAILURE=false
CUSTOM_BIN=""

# --- Parse arguments ---
while [[ $# -gt 0 ]]; do
    case "$1" in
        --bin)
            CUSTOM_BIN="$2"
            shift 2
            ;;
        --verbose|-v)
            VERBOSE=true
            shift
            ;;
        --quick|-q)
            STOP_ON_FAILURE=true
            shift
            ;;
        --help|-h)
            head -n 22 "$0" | grep "^#" | sed 's/^# \?//'
            exit 0
            ;;
        language|integration|iop|misc|repro|vm|all)
            CATEGORY="$1"
            shift
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use --help for usage instructions."
            exit 1
            ;;
    esac
done

# --- Find ProXPL binary ---
find_binary() {
    if [[ -n "${CUSTOM_BIN}" ]] && [[ -x "${CUSTOM_BIN}" ]]; then
        echo "${CUSTOM_BIN}"
        return 0
    fi

    local candidates=(
        "${ROOT_DIR}/build/bin/Release/proxpl.exe"
        "${ROOT_DIR}/build/bin/Debug/proxpl.exe"
        "${ROOT_DIR}/build/bin/proxpl.exe"
        "${ROOT_DIR}/build/Release/proxpl.exe"
        "${ROOT_DIR}/build/Debug/proxpl.exe"
        "${ROOT_DIR}/build/proxpl.exe"
        "${ROOT_DIR}/build/proxpl"
        "${ROOT_DIR}/build/bin/proxpl"
        "${ROOT_DIR}/proxpl.exe"
        "${ROOT_DIR}/proxpl"
    )

    for bin in "${candidates[@]}"; do
        if [[ -f "${bin}" ]]; then
            echo "${bin}"
            return 0
        fi
    done

    return 1
}

PROXPL_BIN=""
if ! PROXPL_BIN="$(find_binary)"; then
    echo -e "${YELLOW}Warning: ProXPL executable not found in standard build directories.${NC}"
    echo -e "${YELLOW}Please build the project first or specify --bin /path/to/proxpl${NC}"
    echo -e "${YELLOW}Build command: cmake -B build && cmake --build build --config Release${NC}"
fi

# --- Test counters ---
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
SKIPPED_TESTS=0
FAILED_LIST=()

# --- Run a single .prox test file ---
run_prox_test() {
    local test_file="$1"
    local test_name
    test_name="$(basename "${test_file}")"
    local category_name
    category_name="$(basename "$(dirname "${test_file}")")"

    TOTAL_TESTS=$((TOTAL_TESTS + 1))

    if [[ -z "${PROXPL_BIN}" ]]; then
        echo -e "  [${YELLOW}SKIP${NC}] ${category_name}/${test_name} (no binary)"
        SKIPPED_TESTS=$((SKIPPED_TESTS + 1))
        return 0
    fi

    # Known problematic tests with workarounds
    if [[ "${test_name}" == "v1_6_3_features.prox" ]] && [[ "${OSTYPE:-}" == "msys"* || "${OSTYPE:-}" == "win32"* || "${OSTYPE:-}" == "cygwin"* ]]; then
        echo -e "  [${YELLOW}SKIP${NC}] ${category_name}/${test_name} (known Windows issue)"
        SKIPPED_TESTS=$((SKIPPED_TESTS + 1))
        return 0
    fi

    local output=""
    local exit_code=0

    # Run test with 10-second timeout
    set +e
    if command -v timeout &>/dev/null; then
        output="$(timeout 10s "${PROXPL_BIN}" "${test_file}" 2>&1)"
        exit_code=$?
    else
        output="$("${PROXPL_BIN}" "${test_file}" 2>&1)"
        exit_code=$?
    fi
    set -e

    if [[ ${exit_code} -eq 0 ]]; then
        echo -e "  [${GREEN}PASS${NC}] ${category_name}/${test_name}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        if ${VERBOSE}; then
            echo -e "${CYAN}--- Output ---${NC}"
            echo "${output}" | sed 's/^/    /'
            echo -e "${CYAN}--------------${NC}"
        fi
    else
        echo -e "  [${RED}FAIL${NC}] ${category_name}/${test_name} (exit code: ${exit_code})"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        FAILED_LIST+=("${category_name}/${test_name}")
        echo -e "${RED}--- Error Output ---${NC}"
        echo "${output}" | head -n 20 | sed 's/^/    /'
        echo -e "${RED}--------------------${NC}"

        if ${STOP_ON_FAILURE}; then
            echo -e "\n${RED}Stopped on first failure (--quick mode)${NC}"
            exit 1
        fi
    fi
}

# --- Run a test suite directory ---
run_suite() {
    local suite_dir="$1"
    local suite_title="$2"

    if [[ ! -d "${suite_dir}" ]]; then
        return 0
    fi

    local count
    count="$(find "${suite_dir}" -maxdepth 1 -name "*.prox" 2>/dev/null | wc -l)"
    if [[ ${count} -eq 0 ]]; then
        return 0
    fi

    echo -e "\n${BOLD}${BLUE}=== ${suite_title} (${count} tests) ===${NC}"

    for test_file in "${suite_dir}"/*.prox; do
        if [[ -f "${test_file}" ]]; then
            run_prox_test "${test_file}"
        fi
    done
}

# --- Banner ---
echo -e "${BOLD}======================================================${NC}"
echo -e "${BOLD}            ProXPL Automated Test Suite               ${NC}"
echo -e "${BOLD}======================================================${NC}"
if [[ -n "${PROXPL_BIN}" ]]; then
    echo -e "Binary:   ${CYAN}${PROXPL_BIN}${NC}"
fi
echo -e "Category: ${CYAN}${CATEGORY}${NC}"
echo -e "Time:     $(date '+%Y-%m-%d %H:%M:%S')"

# --- Execute selected test suites ---
case "${CATEGORY}" in
    language)
        run_suite "${ROOT_DIR}/tests/language" "Language Core Tests"
        ;;
    integration)
        run_suite "${ROOT_DIR}/tests/integration" "Integration & Stdlib Tests"
        ;;
    iop)
        run_suite "${ROOT_DIR}/tests/iop" "Intent / Context / ASR Tests"
        ;;
    misc)
        run_suite "${ROOT_DIR}/tests/misc" "Miscellaneous Feature Tests"
        ;;
    repro)
        run_suite "${ROOT_DIR}/tests/repro" "Regression Reproduction Tests"
        ;;
    vm)
        echo -e "\n${BOLD}${BLUE}=== C/C++ VM Unit Tests ===${NC}"
        if [[ -d "${ROOT_DIR}/build" ]] && command -v ctest &>/dev/null; then
            ctest --test-dir "${ROOT_DIR}/build" -R "Bytecode|Opcode" --output-on-failure
        else
            echo -e "  [${YELLOW}SKIP${NC}] VM unit tests require building with -DBUILD_TESTS=ON and ctest"
        fi
        ;;
    all)
        run_suite "${ROOT_DIR}/tests/language" "1. Language Core Tests"
        run_suite "${ROOT_DIR}/tests/integration" "2. Integration & Stdlib Tests"
        run_suite "${ROOT_DIR}/tests/iop" "3. Intent / Context / ASR Tests"
        run_suite "${ROOT_DIR}/tests/misc" "4. Miscellaneous Tests"
        run_suite "${ROOT_DIR}/tests/repro" "5. Regression Tests"

        # Root level tests if any
        for test_file in "${ROOT_DIR}/tests"/*.prox; do
            if [[ -f "${test_file}" ]]; then
                echo -e "\n${BOLD}${BLUE}=== 6. Root Level Tests ===${NC}"
                break
            fi
        done
        for test_file in "${ROOT_DIR}/tests"/*.prox; do
            if [[ -f "${test_file}" ]]; then
                run_prox_test "${test_file}"
            fi
        done
        ;;
esac

# --- Summary ---
echo -e "\n${BOLD}======================================================${NC}"
echo -e "${BOLD}                    Test Summary                      ${NC}"
echo -e "${BOLD}======================================================${NC}"
echo -e "Total:   ${TOTAL_TESTS}"
echo -e "Passed:  ${GREEN}${PASSED_TESTS}${NC}"
echo -e "Failed:  ${RED}${FAILED_TESTS}${NC}"
echo -e "Skipped: ${YELLOW}${SKIPPED_TESTS}${NC}"

if [[ ${FAILED_TESTS} -gt 0 ]]; then
    echo -e "\n${RED}${BOLD}Failed Tests (${FAILED_TESTS}):${NC}"
    for failed in "${FAILED_LIST[@]}"; do
        echo -e "  - ${RED}${failed}${NC}"
    done
    echo -e "\n${RED}FAILED (exit status 1)${NC}"
    exit 1
elif [[ ${PASSED_TESTS} -eq 0 && ${TOTAL_TESTS} -gt 0 ]]; then
    echo -e "\n${YELLOW}NO TESTS PASSED (Check if binary exists and is executable)${NC}"
    exit 0
else
    echo -e "\n${GREEN}${BOLD}ALL TESTS PASSED!${NC}"
    exit 0
fi
