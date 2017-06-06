./../client get -n 2 -s eq Ala -s any
./../client get -n 1 -s any
./../client get -n 2 -s eq ale -s any
./../client get -n 2 -s any -s any
./../client get -n 3 -s any -s any -s .

sleep 2

./../client read -n eq 2 -i eq 13 -i eq 33
./../client get -n any -i any
./../client get -n lt 10 -i eq 32
./../client get -n eq 1 -i eq 10
./../client get -n eq 3 -i any -i eq 20 -i eq 30
./../client get -n lt 5 -i gt 20 -i eq 28 -i any
./../client get -n any -i eq 31 -i gt 6
./../client get -n eq 2 -i eq13 -i eq 33
./../client get -n any -i eq 20 -i gt 20 -s a
