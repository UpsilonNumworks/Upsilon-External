#!/usr/bin/env bash

set -e

LD=arm-none-eabi-ld
OBJCOPY=arm-none-eabi-objcopy

echo "Building archive..."
echo "  - ld: $LD"
echo "  - objcopy: $OBJCOPY"
echo "  - arguments: $*"
echo "Input files:"

OUTPUT="$1"
shift 1
rm -f "${OUTPUT}"

POS=0

while [[ -n "$1" ]]; do
  # Remove the suffix from $1
  FILENAME="$(echo "$1" | sed -e 's/_flash//')"
  FILENAME="$(echo "$1" | sed -e 's/_archive//')"
  # If filename doesn't exist, interpret it as a application name
  if [[ -n "${FILENAME}" ]]; then
    NAME="${FILENAME}"
    FILENAME="apps/${FILENAME}/app.elf"
  fi
  echo "  - ${FILENAME}"
  NAME="$(basename "$(dirname "${FILENAME}")")"
  sed "s/\+ (0)/+ (${POS})/" apps/external.ld > temp.ld
  "${LD}" "${FILENAME}" -o "${NAME}_linked.elf" -Ttemp.ld
  "${OBJCOPY}" -O binary "${NAME}_linked.elf" "${NAME}"
  tar -rf "${OUTPUT}" "${NAME}"
  POS=$(( POS + (($(stat --printf="%s" "${NAME}") + 1023)/512)*512 ))
  rm -f temp.ld "${NAME}" "${NAME}_linked.elf"
  # Add the icon if it exists to the archive (Icon is the app.icon file in the app directory)
  if [[ -f "apps/${NAME}/app.icon" ]]; then
    ICON="apps/${NAME}/app.icon"
    echo "  - ${ICON}"
    # Copy temporary the icon to the current directory
    cp "${ICON}" "${NAME}.icon"
    # Add the icon to the archive
    tar -rf "${OUTPUT}" "${NAME}.icon"
    # Increase the position of the icon
    POS=$(( POS + (($(stat --printf="%s" "${NAME}.icon") + 1023)/512)*512 ))
    # Remove the temporary icon
    rm -f "${NAME}.icon"
  fi
  shift 1
done
