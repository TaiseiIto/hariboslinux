#!/bin/sh

echo Your GitHub email:
read email
echo Your GitHub name:
read name
git config --global user.email $email
git config --global user.name $name

