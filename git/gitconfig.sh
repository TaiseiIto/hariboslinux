#!/bin/sh

curdir=$(cd $(dirname $0) && pwd)
echo -n "Your GitHub email:"
read email
echo -n "Your GitHub name:"
read name
git config --global user.email $email
git config --global user.name $name
echo /url/ > .edcmd
echo .s/github.com/${name}@github.com/ >> .edcmd
echo w >> .edcmd
echo q >> .edcmd
ed ${curdir}/../.git/config < .edcmd
rm .edcmd

