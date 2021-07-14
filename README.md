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

## bootの流れ

### bootsector.bin
hardwareが起動すると，BIODはfloppy diskの0x00000000番地から0x000001ff番地に書き込まれたbootsector.binを，memoryの0x7c00番地から0x7dff番地に読み込み，これを実行します．
bootsector.binでは，bootの第2段階であるloaddisk.binを含むfloppy diskの0x00002400番地から0x000047ff番地までを，memoryの0xa000番地から0xc400番地に読み込み，loaddisk.binに移行します．
[bootsector.binのsource](src/bootsector.s)

#### bootsector.bin実行時のmemory map

### loaddisk.bin
floppy diskをmemoryに読み込み，initscrn.binに移行します．
これにより，floppy diskの0x00000000番地から0x000783ff番地までが，memoryの0x00007c00番地から0x0007ffff番地に配置されます．
[loaddisk.binのsource](src/loaddisk.s)

#### .bin実行時のmemory map

### initscrn.bin
BIOSのconsole画面を破棄し，VGA画面に移行します．
画面とkeyboardに関する以下の構造体を，0x7bf6番地に配置します．
```
typedef struct
{
	void *vram_addr;
	unsigned short screen_width;
	unsigned short screen_height;
	unsigned char bits_per_pixel;
	unsigned char keyboard_state;
} __attribute__((packed)) BootInformation;
```
そしてmv2prtmdに移行します．

[.binのsource](src/initscrn.s)

#### initscrn.bin実行時のmemory map

### mv2prtmd.bin
real modeからprotected modeに移行し，kernel.binに移行します．
[.binのsource](src/mv2prtmd.s)

#### mv2prtmd.bin実行時のmemory map

### kernel.bin
OS本体です．
[kernel.binのsource](src/kernel)

#### kernel.bin実行時のmemory map

## 開発者用メモ
docker, VNC softwareに加え，git, makeを用いて開発しています．

### clone repository
このrepositoryをcloneします．
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

### Debug
以下のcommandでQEMUをgdb待機状態で起動し，さらにgdbを起動してQEMUに接続します．
このcommandにより，gdbを用いたDebugが可能です．
```
/~/hariboslinux # make debug
```

### git setting
git commitをするために，emailとnameを設定します．
```
/~/hariboslinux # make gitconfig
```

## gdbの使い方
とりあえずQEMUをgdb待機状態で起動してgdbからQEMUに接続する．

```
/~/hariboslinux # make debug
```
loaddisk.binのentry point 0xbe00にbreak pointを置き，そこまで進む処理は.gdbinitに書かれている．

### registerの内容の確認
```
(gdb) info registers
```

### 現在地以降の30命令をdisassemble
target.xmlとi386-32bit.xmlにより，real modeでのdissasembleを行っている．
32bit modeに移行してからは，この設定を変更する必要がある．(変更済み．gdb/.gdbinitでtarget.xmlの読み込みをコメントアウトした．)
```
(gdb) x/30i $eip
```
また，仮想addressではなく，物理addressを指定するということに注意．
例えば，現在のcode segmentが0x0000ce00番地から始まっている場合，
```
(gdb) x/30i 0x0000ce00+$eip
```
のようにして現在地以降の命令を表示する．

### stackの内容を表示
現在のstack flameを2byte単位で8要素表示
```
(gdb) x/8h $esp
```

### break pointの設置
memory番地0xbe00にbreak pointを設置する．
```
(gdb) break *0xbe00
```

### break pointまで実行を進める
```
(gdb) continue
```
break pointに到達すると，gdbはSIGTRAP信号を受信して一時停止します．
実行を再開する場合，
```
(gdb) info breakpoints
```
でbreak pointsの一覧を表示し，引っ掛かったbreak pointを探します．
引っ掛かったbreak pointの番号をnとすると，
```
(gdb) delete n
```
で当該break pointを削除し，
```
(gdb) continue
```
で実行を再開します．
また，別の方法として，
```
(gdb) stepi
```
で1命令だけ実行してbreak pointを抜けてから，
```
(gdb) continue
```
で実行を再開します．
