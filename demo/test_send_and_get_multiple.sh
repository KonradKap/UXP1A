./../client send -n 2 -i 30 -i 30
./../client send -n 2 -i 20 -i 20
./../client send -n 2 -s asd -s dsa
sleep 1;
./../client get -n 2 -i eq 20 -i any
./../client get -n 2 -i eq 20 -i any&
sleep 1;
./../client send -n 2 -i 20 -i 30
sleep 1;
./../client get -n 2 -i eq 30 -i eq 30

./../client read -n 1 -s any -s any
./../client get -n 1 -s eq asd -s any

