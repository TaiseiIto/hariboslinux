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

## Docker
### Docker image生成
[上](#Haribos-Linuxの動作条件)に述べたような機器がない場合でもHaribos Linuxを動かせるように，[Dockerfile](Dockerfile)を用意しました．
dockerがinstallされている状態で，以下のcommandで仮想環境のimageが準備されます．
```
$ make docker-build
```

### Docker container生成および起動
さらに，以下のcommandで仮想環境のcontainerが起動します．
```
$ make docker-run
```

また，以下のようなcommandも用意してあります．

### Docker container起動
```
$ make docker-stop
```

### Docker container login
```
$ make docker-login
```

### Docker container停止
```
$ make docker-stop
```

## Build
Docker containerを起動すると，このrepositoryが`~/hariboslinux`にcloneされ，`~/hariboslinux`に移動した状態になります．
container上で以下のcommandによりHaribos Linuxのimage file (haribos.img) がBuildされます．
```
$ make
```

## QEMUによる実行
次に，container上で以下のcommandによりQEMUを立ち上げ，QEMU上でHaribos Linuxを動かします．
```
$ make run
```
このcommandにより実行されるQEMUはVNC通信を行っており，dockerを動かしているホストPCからlocalhostの5900番ポートにVNC接続することで，Haribos Linuxを操作することができます．

