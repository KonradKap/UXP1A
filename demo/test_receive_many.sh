./../client get -n 2 -s eq ala -s any
./../client get -n 1 -s any
./../client get -n 2 -s eq ale -s any
./../client get -n 2 -s any -s any
./../client get -n 2 -s any -s any 

sleep 2

./../client read -n eq 3 -i lt 30 -i gt 20 -i any
./../client get -n 3 -i any -i le 42 -i ge 42
./../client get -n 1 -i any
./../client get -n 2 -i eq 31 -i any
./../client get -n 1 -i gt 100
./../client get -n 1 -i lt 100
./../client get -n 3 -i any -i eq 20 -i gt 20
./../client get -n 3 -i any -i eq 22 -s eq ?
