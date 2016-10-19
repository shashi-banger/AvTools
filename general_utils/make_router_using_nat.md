
On the machine which is to be converted to router do the following

sudo sysctl net.ipv4.ip_forward=1
sudo iptables -t nat -A POSTROUTING -o wlan0 -j MASQUERADE



On the machine which uses the above router to reach out to interbate

ip route del default  
ip route add default vial 192.168.1.31

where 192.168.1.31 is the ip address of the firt machine which would act as router
