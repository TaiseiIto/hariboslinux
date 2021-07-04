#!/bin/sh

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
ed .git/config < .edcmd
rm .edcmd

