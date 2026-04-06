#!/bin/bash

# rebuilds and runs
if [ "$1" = "remake" ]; then
  
  # Check if build dir exis
  if [ -d "build" ]; then
    rm -rf build # Remove it
  fi

  mkdir build && cd build && cmake -G "Unix Makefiles" .. && make # Create MAIN
else
  # build dir not exists
  if [ ! -d "build" ]; then
    echo "build not exits"
    echo "Run run.sh remake"
    exit 
  else
    cd build   
  fi

fi

# Run the program
./MAIN

