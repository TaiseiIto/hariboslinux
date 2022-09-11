#!/bin/sh

# This program gives the push access right.
# Only the developer can execute it.
# Put the ssh private key "/root/hariboslinux/ssh/github" before the execution.
# Put the gpg directory "/root/.gnupg" before the execution.

currentdir=$(pwd)
cd $(dirname $0)
echo -n "Your GitHub email:"
read email
echo -n "Your GitHub name:"
read name
echo -n "Password of ${email}:"
read password
git config --global sendemail.confirm auto
git config --global sendemail.smtpserver smtp.office365.com
git config --global sendemail.smtpencryption tls
git config --global sendemail.smtpuser $email
git config --global sendemail.smtpserverport 587
git config --global sendemail.smtppass $password
git config --global user.email $email
git config --global user.name $name
git config --global user.signingkey 5F1ABB3B591CCA48
git config --global --add commit.gpgsign true
git remote set-url origin git@github.com:TaiseiIto/hariboslinux.git
cat gitconfig >> /root/.gitconfig
cat ../ssh/config >> /root/.ssh/config
chmod 600 ../ssh/github
chmod -R 600 /root/.gnupg
cd $currentdir

