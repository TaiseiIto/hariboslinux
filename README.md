# Haribos Linux
このRepositoryでは，Haribos Linuxを作ります．
Haribos Linuxは，Linux環境でBuild可能なHaribote OSの改造版です．
「30日でできるOS自作入門」(ISBN4-8399-1984-4)に基づいて作成します．

## MIT LICENSE
[ここ](LICENSE)に示すとおり，このrepositoryはMIT LICENSEに基づいて配布されます．
そのため，[Taisei Ito](https://github.com/TaiseiIto)はこのrepositoryを利用したことによる問題の責任を負いません．
このrepositoryをbuildすることによって得られるOSは仮想machine上で実行することを強くお勧めします．

## Haribos Linuxの動作条件
「30日でできるOS自作入門」(ISBN4-8399-1984-4)に基づいて作るため，以下の条件を満たすhardwareで実行可能です．

* PC/AT互換機であること
* Intel 486 processorの命令setが実行可能であること
* Legacy BIOSが利用可能であること

また，Buildによって生成されるimage fileはFAT12 floppy disk raw imageです．

## Build
以下のcommandでgccを用いてOSのimage file (haribos.img) がBuildされます．
```
$ make
```
gccがinstallされている必要があります．

## QEMUによる実行
Buildによって生成されたimage fileは，Virtual BoxやVM wareで動作することを確認しています．
また，QEMUでの実行に対応しています．
以下のcommandでharibos.imgをQEMU上で実行します．
```
$ make run
```
qemu-system-i386がinstallされている必要があります．

## Dockerメモ

* イメージのビルド
```
$ docker build -t <image name>:<image tag> <Dockerfile dir>
```

* イメージ一覧
```
$ docker images
```

* コンテナ一覧
```
$ docker ps -a
```

* イメージ取得
```
$ docker pull <image name>
```

* イメージ削除
```
$ docker rmi <image name>
```

* コンテナ作成起動
```
$ docker run --name <container name> -i -t <image name> <command>
```

* コンテナ起動
```
$ docker start <container name>
```

* コンテナ終了
```
$ docker stop <container name>
```

* コンテナ再起動
```
$ docker restart <container name>
```

* コンテナへログイン
```
$ docker exec -i -t <container name>
```

* コンテナ削除
```
$ docker rm <container name>
```

