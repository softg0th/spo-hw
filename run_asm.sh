#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
OUTPUT_DIR="$ROOT_DIR/out"

mkdir -p "$OUTPUT_DIR"

assemble() {
  mono "$ROOT_DIR/RemoteTasks/Portable.RemoteTasks.Manager.exe" \
    -ul u308454 -up a3092ee6-9bf0-42cc-bfb9-b1ac10446596 -w -id -s Assemble \
    asmListing "$OUTPUT_DIR/out.asm" \
    definitionFile "$ROOT_DIR/arch/myLang.target.pdsl" \
    archName myLang > "$OUTPUT_DIR/asmId.txt"

  mono "$ROOT_DIR/RemoteTasks/Portable.RemoteTasks.Manager.exe" \
    -g "$(cat "$OUTPUT_DIR/asmId.txt")" \
    -r out.ptptb \
    -o "$OUTPUT_DIR/out.ptptb"
}

runbin() {
  mono "$ROOT_DIR/RemoteTasks/Portable.RemoteTasks.Manager.exe" \
    -w -id -s ExecuteBinary \
    -ul u308454 -up a3092ee6-9bf0-42cc-bfb9-b1ac10446596 \
    definitionFile "$ROOT_DIR/arch/myLang.target.pdsl" \
    archName myLang \
    binaryFileToRun "$OUTPUT_DIR/out.ptptb" \
    codeRamBankName code_ram \
    ipRegStorageName ip \
    finishMnemonicName hlt \
    > "$OUTPUT_DIR/runId.txt"

  mono "$ROOT_DIR/RemoteTasks/Portable.RemoteTasks.Manager.exe" \
    -g "$(cat "$OUTPUT_DIR/runId.txt")" \
    -r stdout.txt \
    -o "$OUTPUT_DIR/stdout.txt"

  echo "Stdout:"
  cat "$OUTPUT_DIR/stdout.txt"
}

case "${1:-}" in
  assemble) assemble ;;
  runbin)   runbin ;;
  all)      assemble; runbin ;;
  *) echo "Usage: $0 {assemble|runbin|all}"; exit 1 ;;
esac