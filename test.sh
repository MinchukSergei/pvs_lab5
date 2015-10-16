sudo rmmod hello
make
sudo insmod hello.ko
sudo chmod a+r+w /sys/kernel/hello/delay