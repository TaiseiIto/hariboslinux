#!/bin/sh

# This program gives the push access right.
# Only the developer can execute it.
# Put the private key "/~/hariboslinux/ssh/github" before the execution.

currentdir=$(pwd)
cd $(dirname $0)
echo -n "Your GitHub email:"
read email
echo -n "Your GitHub name:"
read name
git config --global user.email $email
git config --global user.name $name
git remote set-url origin git@github.com:TaiseiIto/hariboslinux.git
cat ../ssh/config >> ~/.ssh/config
chmod 600 ../ssh/github
cd $currentdir

