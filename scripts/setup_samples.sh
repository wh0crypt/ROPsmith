#!/usr/bin/env bash

# This script sets up sample binaries for ROP gadget finding tests.

set -euo pipefail

OUTDIR="${1:-test/test_samples}"
mkdir -p "$OUTDIR"

# Check dependencies
for cmd in gcc objcopy head; do
    if ! command -v "$cmd" >/dev/null 2>&1; then
        echo "Error: required command '$cmd' not found. Install it (e.g. build-essential, binutils)." >&2
        exit 1
    fi
done

echo "[*] Generating test samples in $OUTDIR"

if [[ -f "$OUTDIR/sample_valid" ]]; then
    echo "  -> sample_valid already exists, skipping."
else
    # 1) sample_valid: small valid ELF (no-pie for predictable layout)
    cat > /tmp/sample_valid.c <<'C_SRC'
#include <stdio.h>

int foo(void) {
    return 42;
}

int main(void) {
    printf("hello sample_valid\n");
    return foo();
}
C_SRC

    gcc -no-pie -O0 -fno-pie -o "$OUTDIR/sample_valid" /tmp/sample_valid.c
    echo "  -> sample_valid (ELF) created."
fi

if [[ -f "$OUTDIR/sample_corrupt" ]]; then
    echo "  -> sample_corrupt already exists, skipping."
else
    # 2) sample_corrupt: truncated copy of valid ELF (first N bytes only)
    # keep small to simulate corruption
    dd if="$OUTDIR/sample_valid" of="$OUTDIR/sample_corrupt" bs=1 count=64 status=none
    echo "  -> sample_corrupt (truncated ELF) created."
fi

if [[ -f "$OUTDIR/sample_empty" ]]; then
    echo "  -> sample_empty already exists, skipping."
else
    # 3) sample_empty: empty file
    : > "$OUTDIR/sample_empty"
    echo "  -> sample_empty (empty file) created."
fi

if [[ -f "$OUTDIR/sample_gadgets" ]]; then
    echo "  -> sample_gadgets already exists, skipping."
else
    # 4) sample_gadgets: ELF that contains explicit gadget bytes in .text
    # We'll embed raw opcodes for common gadgets:
    #   pop rdi; ret   -> 0x5f 0xc3
    #   pop rsi; ret   -> 0x5e 0xc3  (note: pop rsi is actually 0x5e)
    # We'll create a function with those bytes so they appear in .text.
    cat > /tmp/sample_gadgets.S <<'ASM'
    .section .text
    .global gadget_start
gadget_start:
    .byte 0x5f, 0xc3        /* pop rdi; ret */
    .byte 0x5e, 0xc3        /* pop rsi; ret */
    .byte 0x58, 0xc3        /* pop rax; ret */
    .global main
main:
    /* trivial main that returns 0 */
    xor %edi, %edi
    xor %eax, %eax
    ret
ASM

    gcc -no-pie -O0 -fno-pie -o "$OUTDIR/sample_gadgets" /tmp/sample_gadgets.S
    echo "  -> sample_gadgets (ELF with gadget bytes) created."
fi

if [[ -f "$OUTDIR/sample_no_text" ]]; then
    echo "  -> sample_no_text already exists, skipping."
else
    # 5) sample_no_text: valid ELF but with .text section removed
    # Use objcopy to remove .text. Keep original as sample_valid.
    cp "$OUTDIR/sample_valid" "$OUTDIR/sample_no_text"
    # Remove .text section (objcopy will keep ELF structure but .text gone)
    if objcopy --remove-section .text "$OUTDIR/sample_no_text" 2>/dev/null; then
        echo "  -> sample_no_text (ELF with .text removed) created."
    else
        # Some objcopy versions require --remove-section=.text (with =). Try fallback.
        objcopy --remove-section=.text "$OUTDIR/sample_no_text" || true
        echo "  -> sample_no_text (ELF with .text removed) created (fallback)."
    fi
fi

if [[ -f "$OUTDIR/sample_no_elf" ]]; then
    echo "  -> sample_no_elf already exists, skipping."
else
    # 6) sample_no_elf: plain text file (not an ELF)
    echo "This is definitely not an ELF binary." > "$OUTDIR/sample_no_elf"
    echo "  -> sample_no_elf (plain text) created."
fi

# Set perms
chmod 0755 "$OUTDIR/sample_valid" "$OUTDIR/sample_gadgets" || true
chmod 0644 "$OUTDIR/sample_corrupt" "$OUTDIR/sample_empty" "$OUTDIR/sample_no_text" "$OUTDIR/sample_no_elf" || true

# Clean temporary files
rm -f /tmp/sample_valid.c /tmp/sample_gadgets.S || true

echo "[*] Done. Samples created in $OUTDIR."
