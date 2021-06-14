#!/bin/bash
mkdir ../type
mkdir ../chord
for i in {0..624..2}
  do
     ./chord $i $1
  done
for i in {0..622..2}
  do
     ./chord2 $i
  done
./chord3
