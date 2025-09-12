#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
OUTPUT_DIR="$ROOT_DIR/out"

mkdir -p "$OUTPUT_DIR"

# загружаем логин/пароль из .env
if [[ -f "$ROOT_DIR/.env" ]]; then
  # shellcheck disable=SC2046
  export $(grep -v '^#' "$ROOT_DIR/.env" | xargs)
else
  echo ".env not found in $ROOT_DIR"
  exit 1
fi

assemble() {
  mono "$ROOT_DIR/RemoteTasks/Portable.RemoteTasks.Manager.exe" \
    -ul "$PORTABLE_LOGIN" -up "$PORTABLE_PASSWORD" \
    -w -id -s Assemble \
    asmListing "$OUTPUT_DIR/out.asm" \
    definitionFile "$ROOT_DIR/arch/myLang.target.pdsl" \
    archName myLang > "$OUTPUT_DIR/asmId.txt"

  mono "$ROOT_DIR/RemoteTasks/Portable.RemoteTasks.Manager.exe" \
    -g "$(cat "$OUTPUT_DIR/asmId.txt")" \
    -r out.ptptb \
    -o "$OUTPUT_DIR/out.ptptb"
}

postassemble() {
  local key
  key="$(cat "$OUTPUT_DIR/asmId.txt")"
  mono "$ROOT_DIR/RemoteTasks/Portable.RemoteTasks.Manager.exe" \
    -g "$key" \
    -ul "$PORTABLE_LOGIN" -up "$PORTABLE_PASSWORD"
}

runbin() {
  mkdir -p "$OUTPUT_DIR"

  if [[ ! -s "$OUTPUT_DIR/stdin.txt" ]]; then
    printf "58\n+\n79\n" > "$OUTPUT_DIR/stdin.txt"
  fi

  echo "===== stdin.txt ====="
  cat "$OUTPUT_DIR/stdin.txt" || echo "(stdin.txt missing)"
  echo "====================="

  mono "$ROOT_DIR/RemoteTasks/Portable.RemoteTasks.Manager.exe" \
    -w -id -s ExecuteBinaryWithInput \
    --user-login "$PORTABLE_LOGIN" --user-password "$PORTABLE_PASSWORD" \
    stdinRegStName inp \
    stdoutRegStName outp \
    inputFile "$OUTPUT_DIR/stdin.txt" \
    definitionFile "$ROOT_DIR/arch/myLang.target.pdsl" \
    archName myLang \
    binaryFileToRun "$OUTPUT_DIR/out.ptptb" \
    codeRamBankName code_ram \
    ipRegStorageName ip \
    finishMnemonicName hlt > "$OUTPUT_DIR/runId.txt"

  mono "$ROOT_DIR/RemoteTasks/Portable.RemoteTasks.Manager.exe" \
    -g "$(cat "$OUTPUT_DIR/runId.txt")" \
    -r stdout.txt \
    -o "$OUTPUT_DIR/stdout.txt"

  echo "Stdout:"
  [[ -f "$OUTPUT_DIR/stdout.txt" ]] && cat "$OUTPUT_DIR/stdout.txt" || echo "(stdout.txt not found)"
}

case "${1:-}" in
  assemble) assemble ;;
  postassemble) postassemble ;;
  runbin) runbin ;;
  all) assemble; postassemble; runbin ;;
  *) echo "Usage: $0 {assemble|postassemble|runbin|all}"; exit 1 ;;
esac