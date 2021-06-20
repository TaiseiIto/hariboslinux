# Haribos Linux
このRepositoryでは，Haribos Linuxを作ります．
Haribos Linuxは，Linux環境でBuild可能なHaribote OSの改造版です．
「30日でできるOS自作入門」(ISBN4-8399-1984-4)に基づいて作成します．

## MIT LICENSE
[ここ](LICENSE)に示すとおり，このrepositoryはMIT LICENSEに基づいて配布されます．
そのため，[Taisei Ito](https://github.com/TaiseiIto)はこのrepositoryを利用したことによる問題の責任を負いません．
このrepositoryをbuildすることによって得られるHaribos Linuxは仮想machine上で実行することを強くお勧めします．

## Haribos Linuxの動作条件
「30日でできるOS自作入門」(ISBN4-8399-1984-4)に基づいて作るため，以下の条件を満たすhardwareで実行可能です．

* PC/AT互換機であること
* Intel 486 processorの命令setが実行可能であること
* Legacy BIOSが利用可能であること

また，Buildによって生成されるimage fileはFAT12 floppy disk raw imageです．

## Dockerでの実行
[上](#Haribos-Linuxの動作条件)に述べたような機器がない場合でもHaribos Linuxを動かせるように，[Docker Image](https://hub.docker.com/repository/docker/taiseiito/hariboslinux)を用意しました．
システム構成は以下の通りです．

* hostには，dockerと，VNC接続ができるsoftwareがinstallされている必要があります．
* host上でdocker containerが動きます．
* docker container上でQEMUが動きます．
* QEMU上でHaribos Linuxが動きます．
* hostとQEMUの間でVNC通信を行うことで，Halibos Linuxを操作できます．

### Docker imageの取得
まず，以下のcommandでdocker imageを落とします．
```
$ docker pull taiseiito/hariboslinux
```

### Docker containerの起動
次に，以下のcommandでdocker containerを起動します．
```
$ docker run -p 5900:5900 -i -t taiseiito/hariboslinux
```

### QEMUの起動
すると，docker containerのterminalに入るので，以下のcommandでQEMUを起動します．
```
/~/hariboslinux # make run
```

QEMUは5900番ポートからVNC信号を飛ばすので，hostからlocalhostの5900番ポートにVNC接続することで，Halibos Linuxを操作できます．

### QEMUの停止
以下のcommandでQEMUを停止します．
```
/~/hariboslinux # make stop
```

### Docker containerの停止
docker container上で
```
/~/hariboslinux # exit
```
するとdocker containerが停止します．

## 開発者用メモ
docker, VNC softwareに加え，git, makeを用いて開発しています．

### clone repository
まず，このrepositoryをcloneします．
```
$ git clone https://github.com/TaiseiIto/hariboslinux.git
$ cd hariboslinux
```

### Docker image生成
以下のcommandでdocker imageが準備されます．
```
$ make docker-build
```

### Docker container生成および起動
さらに，以下のcommandでdocker containerが起動します．
```
$ make docker-run
```
containerの/bin/shが起動します．
```
/~/hariboslinux # exit
```
でcontainerから抜け，containerを停止します．

また，以下のようなcommandも用意してあります．

### Docker container起動
```
$ make docker-start
```

### Docker container login
```
$ make docker-login
```
containerの/bin/shに入ります．
```
/~/hariboslinux # exit
```
でcontainerから抜け，containerを停止します．

### Docker container停止
```
$ make docker-stop
```

### Docker container削除
```
$ make docker-remove-container
```

### Docker image削除
```
$ make docker-remove-image
```

## OSのimage fileおよびその更新
Docker containerを起動すると，このrepositoryが`~/hariboslinux`にcloneされ，`~/hariboslinux`に移動した状態になります．
ここにはBuild済みのHaribos Linux floppy disk raw image `~/hariboslinux/haribos.img`があります．
ただし，このimage fileは最新でない可能性があるため，最新のimage fileを取得するために以下を実行してください．
```
/~/hariboslinux # make update
```

## QEMUによる実行
次に，container上で以下のcommandによりQEMUを立ち上げ，QEMU上でHaribos Linuxを動かします．
```
/~/hariboslinux # make run
```
このcommandにより実行されるQEMUはVNC通信を行っており，dockerを動かしているhostからlocalhostの5900番ポートにVNC接続することで，Haribos Linuxを操作することができます．

### QEMUの停止
以下のcommandでQEMUを停止します．
```
/~/hariboslinux # make stop
```

