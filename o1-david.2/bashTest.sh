
#!/bin/bash
# A sample Bash script

# myarr=($(ipcs -mp | awk '$2=="o1-david" { print $4 }'))

 
# myarr2=($(ipcs -mp | awk '$2=="o1-david" { print $3 }'))

# myarr=($(ipcs -mp | awk '$2=="o1-david" { print $4 }'))

# lsof | egrep "myarr|COMMAND"

myarr=($(lsof | awk '$3=="o1-david" { print $2 }'))

for i in "${myarr[@]}"
do
     echo found PID: $i
done


for i in "${myarr[@]}"
do
     echo removing old process id $i
     echo kill -9 $i
done


