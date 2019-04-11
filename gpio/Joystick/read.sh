#!/bin/bash

for i in {1..100}; do cat /dev/joystick_read; sleep 1; done;