#!/bin/bash -eu
DIRECTORY_TO_OBSERVE="ES6-Hello-world"      # might want to change this
function block_for_change {
  inotifywait --recursive \
    --event modify,move,create,delete \
    $DIRECTORY_TO_OBSERVE
}
BUILD_SCRIPT=build.sh          # might want to change this too
function build {
  bash cd ES6-Hello-world
  bash make
}
build
while block_for_change; do
  build
done