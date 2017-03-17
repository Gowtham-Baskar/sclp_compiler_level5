cd testcases;
for f in `ls|grep spim`
do
	echo $f
	diff $f ../testcases14/$f
done