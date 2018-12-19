#!/bin/bash
for i in `seq 8 22`;
do
  ./p5 -t art-full.trace -cache $i 6 2
done
