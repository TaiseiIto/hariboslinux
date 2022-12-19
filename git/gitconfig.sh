#!/bin/bash

# This program gives the push access right.
# Only the developer can execute it.
# Put the ssh private key "/root/hariboslinux/ssh/github" before the execution.
# Put the gpg directory "/root/.gnupg" before the execution.

currentdir=$(pwd)
cd $(dirname $0)
read -p "Your GitHub email:" email
read -p "Your GitHub name:" name
echo -n "Password of ${email}:"
unset password
prompt="Password of ${email}:"
while true ; do
	read -n 1 -p "$prompt" -r -s char
	if [[ $char == $'\0' ]]; then
		break
	fi
	prompt="*"
	password+="$char"
done
echo
git config --global sendemail.confirm auto
git config --global sendemail.smtpserver smtp.office365.com
git config --global sendemail.smtpencryption tls
git config --global sendemail.smtpuser $email
git config --global sendemail.smtpserverport 587
git config --global sendemail.smtppass $password
git config --global user.email $email
git config --global user.name $name
git config --global user.signingkey $(head -n1 /root/.gnupg/signingkey.txt)
git config --global --add commit.gpgsign true
git remote set-url origin git@github.com:TaiseiIto/hariboslinux.git
cat gitconfig >> /root/.gitconfig
cat ../ssh/config >> /root/.ssh/config
chmod 600 ../ssh/github
chmod -R 600 /root/.gnupg
cd $currentdir

