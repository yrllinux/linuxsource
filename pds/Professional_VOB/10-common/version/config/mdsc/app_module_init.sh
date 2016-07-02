#!/bin/sh
count=5
echo -n "PRESS Ctrl+C to stop autorun app modules: "

while [ $count -ge 0 ]
do
    echo -ne "\b$count"
    count=`expr $count - 1`
    sleep 1
done

echo 

#ifocnfig eth0 172.16.5.201

if test -f /app/gk/kdvgk; then
    cd /app/gk
    if ! test -x kdvgk; then chmod +x kdvgk; fi
    ./kdvgk &
else
    echo "Start /app/gk/kdvgk fail!"
fi

if test -f "/app/t.120/dcserver_r"; then
    cd /app/t.120
    if ! test -x dcserver_r; then chmod +x dcserver_r; fi
    ./dcserver_r &
else
    echo "Start /app/t.120/dcserver_r fail!"
fi

if test -f "/app/mp/lib/bcsr.ko"; then
	insmod /app/mp/lib/bcsr.ko
	insmod /app/mp/lib/led_k.ko
else
    echo "insmod the kernel(/app/mp/lib/) fail!"
fi

if test -f "/app/mp/mp8000b"; then
    cd /app/mp
    if ! test -x mp8000b; then chmod +x mp8000b; fi
    ./mp8000b &
else
    echo "Start /app/mp/mp8000b fail!"
fi

if test -f "/app/pxy/pxyserverapp"; then
    cd /app/pxy
    if ! test -x pxyserverapp; then chmod +x pxyserverapp; fi
    ./pxyserverapp &
else
    echo "Start /app/pxy/pxyserverapp fail!"
fi


cd /

