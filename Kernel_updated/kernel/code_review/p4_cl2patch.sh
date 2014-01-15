echo path=$1 folder=$2 pre_CL=$3 this_CL=$4
echo sync=$1$2 src=$2_$3 dest=$2_$4
fsync $1$2 $3
mv $2 $2_$3
fsync $1$2 $4
mv $2 $2_$4
diff -aruNp $2_$3 $2_$4 > $2_CL$4.patch