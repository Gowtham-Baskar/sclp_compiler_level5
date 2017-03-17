for f in testcases/*
do
	./sclp $f -icode
done

for f in testcases14/*
do
	./sclp14 $f -icode
done