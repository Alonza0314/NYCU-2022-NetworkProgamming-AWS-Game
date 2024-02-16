# NYCU-2022-NetworkProgamming-AWS-Game

This is a project to realize the cloud server via AWS service.

With using EC2\EFS\VPC service, I create three game server which can communicate with each other through EFS files.

Here are some commands I used:

#Link EC2

ssh -i labsuser.pem ec2-user@

#Transfer files, including code...

scp -i labsuser.pem {file} ec2-user@{ip}:/home/ec2-user

#Downloads packages on EC2

sudo yum update
sudo yum install -y gcc-c++
sudo yum install -y amazon-efs-utils

#Mount EFS

sudo mount -t efs -o tls fs-03679d242d88a3601:/ efs

#Test from TA's image

docker run -it -v D:/大三上/網路程式設計概論/HW3:/home/build yuthomas/np_hw2_x86:latest bin/bash

#EFS path

efs/server{1,2,3}.txt

#Run with TA's test-code

./build.sh
python3 demo.py --server-ip 127.0.0.1
./build/server
python3 demo_part3.py --server-ips <server1 IP> <server2 IP> <server3 IP>
rm -r build
