#!/usr/bin/env bash
set -euo pipefail
set -x

# --- Load .env (expects: login=..., password=...) ---------------------------
if [[ -f .env ]]; then
  set -a
  . ./.env
  set +a
fi
: "${login:?Missing 'login' in .env}"       # required
: "${password:?Missing 'password' in .env}" # required

REMOTE_TASKS="./RemoteTasks/Portable.RemoteTasks.Manager.exe"
ARCH_DEF="arch/myLang.target.pdsl"
ARCH_NAME="myLang"
OUTPUT_DIR="out"
ASM_FILE="./cmake-build-debug/out.asm"

mkdir -p "$OUTPUT_DIR"

check_file() {
  local f="$1"
  if [[ ! -f "$f" ]]; then
    echo "❌ Error: expected file not found: $f" >&2
    exit 1
  fi
}

# --- Проверки ---------------------------------------------------------------
check_file "$ASM_FILE"
check_file "$REMOTE_TASKS"
check_file "$ARCH_DEF"

# --- Ассемблирование в ptptb ------------------------------------------------
echo ">>> Ассемблирование: $ASM_FILE -> out.ptptb"
mono "$REMOTE_TASKS" \
  -u login -p password -w -s Assemble \
  asmListing "$ASM_FILE" \
  definitionFile "$ARCH_DEF" \
  archName "$ARCH_NAME" \
  > "$OUTPUT_DIR/asmId.txt"
check_file "$OUTPUT_DIR/asmId.txt"

mono "$REMOTE_TASKS" \
  -g "$(cat "$OUTPUT_DIR/asmId.txt")" \
  -r out.ptptb \
  -o "$OUTPUT_DIR/out.ptptb"
check_file "$OUTPUT_DIR/out.ptptb"

ptptb="$OUTPUT_DIR/out.ptptb"

# --- Запуск бинарника в VM (stdout only) ------------------------------------
echo ">>> Запуск бинарника: $ptptb"
mono "$REMOTE_TASKS" \
  -w -s ExecuteBinary \
  definitionFile "$ARCH_DEF" \
  archName "$ARCH_NAME" \
  binaryFileToRun "$ptptb" \
  codeRamBankName code_ram \
  ipRegStorageName ip \
  finishMnemonicName hlt \
  > "$OUTPUT_DIR/runId.txt"
check_file "$OUTPUT_DIR/runId.txt"

mono "$REMOTE_TASKS" \
  -g "$(cat "$OUTPUT_DIR/runId.txt")" \
  -r stdout.txt \
  -o "$OUTPUT_DIR/stdout.txt"
check_file "$OUTPUT_DIR/stdout.txt"

echo ">>> Вывод программы:"
cat "$OUTPUT_DIR/stdout.txt"