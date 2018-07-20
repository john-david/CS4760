

#!/bin/bash





m=`ipcs -m | grep -w 777 | cut -d' ' -f2`
for i in $m
do
        echo removing shm id $i
        ipcrm -m $i
done

s=`ipcs -s | grep -w 777 | cut -d' ' -f2`
for i in $s
do
        echo removing sem id $i
        ipcrm -s $i
done

q=`ipcs -q | grep -w 777 | cut -d' ' -f2`
for i in $q
do
        echo removing queue id $i
        ipcrm -q $i
done
