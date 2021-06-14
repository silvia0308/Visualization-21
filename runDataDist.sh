#!/bin/bash
>../distribute.txt
echo "Num-Star-FormingStar-WindStar-AGN" > ../distribute.txt
for i in {0..624..2}
  do
     ./dataDistribu $i $1
  done
