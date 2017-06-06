./../client send -n 1 -i 30
sleep 1;
./../client get -n 1 -i eq 20&
sleep 1;
./../client send -n 1 -i 20
sleep 1;
./../client get -n 1 -i any
