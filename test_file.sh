#!/bin/bash

rm -r $3/*ast
rm -r $3/*spim
rm -r $3/*toks
rm -r $3/*ic
rm -r $3/*dce	
rm -r testcases_1
mkdir testcases_1
ln -s -r $3/*.c testcases_1/
rm -r testcases_2
mkdir testcases_2
ln -s -r $3/*.c testcases_2/

for file in ./$3/*.c; do
	echo ${file}
  ./$1 -icode testcases_1/${file##*/} >& out_1
  ./$2 -icode testcases_2/${file##*/} >& out_2

  diff testcases_1/${file##*/}.ic testcases_2/${file##*/}.ic
  # diff testcases_1/${file##*/}.spim testcases_2/${file##*/}.spim
  diff testcases_1/${file##*/}.dce testcases_2/${file##*/}.dce

done
