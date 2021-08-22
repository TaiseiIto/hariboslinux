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

### floppy disk imageをDocker containerからdownload
```
$ make download-image
```

## bootの流れ

### bootsector.bin
hardwareが起動すると，BIODはfloppy diskの0x00000000番地から0x000001ff番地に書き込まれたbootsector.binを，memoryの0x7c00番地から0x7dff番地に読み込み，これを実行します．
bootsector.binでは，bootの第2段階であるloaddisk.binを含むfloppy diskの0x00002400番地から0x000047ff番地までを，memoryの0xa000番地から0xc3ff番地に読み込み，loaddisk.binに移行します．

[bootsector.binのsource](src/bootsector.s)

#### bootsector.bin実行時のmemory map
| memory start | memory end | disk start | disk end   | description            |
| ------------ | ---------- | ---------- | ---------- | ---------------------- |
| 0x00000000   | 0x000003ff |            |            | interrupt vector table |
| 0x00000400   | 0x000004ff |            |            | BIOS data area         |
| 0x00000500   | 0x00007bff |            |            | stack                  |
| 0x00007c00   | 0x00007dff | 0x00000000 | 0x000001ff | bootsector.bin         |

### loaddisk.bin
floppy diskをmemoryに読み込み，initscrn.binに移行します．
これにより，floppy diskの0x00000000番地から0x000783ff番地までが，memoryの0x00007c00番地から0x0007ffff番地に配置されます．

[loaddisk.binのsource](src/loaddisk.s)

#### loaddisk.bin実行時のmemory map
| memory start  | memory end | disk start | disk end   | description            |
| ------------- | ---------- | ---------- | ---------- | ---------------------- |
| 0x00000000    | 0x000003ff |            |            | interrupt vector table |
| 0x00000400    | 0x000004ff |            |            | BIOS data area         |
| 0x00000500    | 0x00007bff |            |            | stack                  |
| 0x00007c00    | 0x00007dff | 0x00000000 | 0x000001ff | bootsector.bin         |
| 0x0000a000    | 0x0000c3ff | 0x00002400 | 0x000047ff | loaded disk data       |
| 0x0000a200    | 0x0000bdff | 0x00002600 | 0x000041ff | root directory entries |
| 0x0000be00    | 0x0000c3ff | 0x00004200 | 0x000047ff | loaddisk.bin           |

### initscrn.bin
BIOSのconsole画面を破棄し，VGA画面に移行します．
画面とkeyboardに関する以下の構造体を，0x0500番地に配置します．
```
typedef struct
{
	unsigned short memory_size; // MiB
	unsigned short screen_width;
	unsigned short screen_height;
	unsigned char bits_per_pixel;
	unsigned char keyboard_state;
	unsigned char last_loaded_cylinder;
	unsigned char last_loaded_head;
	unsigned char last_loaded_sector;
} __attribute__((packed)) BootInformation;
```
そしてmv2prtmdに移行します．

[initscrn.binのsource](src/initscrn.s)

#### initscrn.bin実行時のmemory map
| memory start | memory end | disk start | disk end   | description                       |
| ------------ | ---------- | ---------- | ---------- | --------------------------------- |
| 0x00000000   | 0x000003ff |            |            | interrupt vector table            |
| 0x00000400   | 0x000004ff |            |            | BIOS data area                    |
| 0x00000500   | 0x00000514 |            |            | struct VbeInfoBlock               |
| 0x00000522   | 0x00000600 |            |            | video mode list                   |
| 0x00000600   | 0x00000700 |            |            | struct vbe\_mode\_info\_structure |
| 0x00000700   | 0x00000705 |            |            | BootInformation structure         |
| 0x00000706   | 0x00007bff |            |            | stack                             |
| 0x00007c00   | 0x0009fbff | 0x00000000 | 0x00097fff | loaded disk data                  |
| 0x00007c00   | 0x00007dff | 0x00000000 | 0x000001ff | bootsector.bin                    |
| 0x00007e00   | 0x00008fff | 0x00000200 | 0x000013ff | first FAT                         |
| 0x00009000   | 0x0000a1ff | 0x00001400 | 0x000025ff | second FAT                        |
| 0x0000a200   | 0x0000bdff | 0x00002600 | 0x000041ff | root directory entries            |
| 0x0000be00   | 0x0000c3ff | 0x00004200 | 0x000047ff | loaddisk.bin                      |
| 0x0000c400   | 0x0000cdff | 0x00004800 | 0x000051ff | initscrn.bin                      |
| 0x0000ce00   | 0x0000d1ff | 0x00005200 | 0x000055ff | mv2prtmd.bin                      |
| 0x0000d200   | ?          | 0x00005600 | ?          | kernel.bin                        |

### mv2prtmd.bin
real modeからprotected modeに移行し，kernel.binに移行します．

[mv2prtmd.binのsource](src/mv2prtmd.s)

#### mv2prtmd.bin実行時のmemory map
| memory start | memory end | disk start | disk end   | description                       |
| ------------ | ---------- | ---------- | ---------- | ----------------------------------|
| 0x00000000   | 0x000003ff |            |            | interrupt vector table            |
| 0x00000400   | 0x000004ff |            |            | BIOS data area                    |
| 0x00000500   | 0x00000514 |            |            | struct VbeInfoBlock               |
| 0x00000522   | 0x00000600 |            |            | video mode list                   |
| 0x00000600   | 0x00000700 |            |            | struct vbe\_mode\_info\_structure |
| 0x00000700   | 0x00000705 |            |            | BootInformation structure         |
| 0x00000706   | 0x00007bff |            |            | stack                             |
| 0x00007c00   | 0x0009fbff | 0x00000000 | 0x00097fff | loaded disk data                  |
| 0x00007c00   | 0x00007dff | 0x00000000 | 0x000001ff | bootsector.bin                    |
| 0x00007e00   | 0x00008fff | 0x00000200 | 0x000013ff | first FAT                         |
| 0x00009000   | 0x0000a1ff | 0x00001400 | 0x000025ff | second FAT                        |
| 0x0000a200   | 0x0000bdff | 0x00002600 | 0x000041ff | root directory entries            |
| 0x0000be00   | 0x0000c3ff | 0x00004200 | 0x000047ff | loaddisk.bin                      |
| 0x0000c400   | 0x0000cdff | 0x00004800 | 0x000051ff | initscrn.bin                      |
| 0x0000ce00   | 0x0000d1ff | 0x00005200 | 0x000055ff | mv2prtmd.bin                      |
| 0x0000d200   | ?          | 0x00005600 | ?          | kernel.bin                        |
| 0x0009fc00   | 0x0009ffff |            |            | ACPI work area                    |
| 0x000a0000   | 0x000bffff |            |            | unused VRAM                       |
| 0x000c0000   | 0x000c7fff |            |            | Video BIOS                        |
| 0x000c8000   | 0x000effff |            |            | BIOS expansions                   |
| 0x000f0000   | 0x000fffff |            |            | Motherboard BIOS                  |

### kernel.bin
OS本体です．

[kernel.binのsource](src/kernel)

#### kernel.bin実行時のmemory map
| memory start | memory end | disk start | disk end   | description                       |
| ------------ | ---------- | ---------- | ---------- | ----------------------------------|
| 0x00000000   | 0x000003ff |            |            | interrupt vector table            |
| 0x00000400   | 0x000004ff |            |            | BIOS data area                    |
| 0x00000500   | 0x00000514 |            |            | struct VbeInfoBlock               |
| 0x00000522   | 0x00000600 |            |            | video mode list                   |
| 0x00000600   | 0x00000700 |            |            | struct vbe\_mode\_info\_structure |
| 0x00000700   | 0x00000705 |            |            | BootInformation structure         |
| 0x00000706   | 0x000073ff |            |            | reserved                          |
| 0x00007400   | 0x00007bff |            |            | IDT                               |
| 0x00007c00   | 0x0009fbff | 0x00000000 | 0x00097fff | loaded disk data                  |
| 0x00007c00   | 0x00007dff | 0x00000000 | 0x000001ff | bootsector.bin                    |
| 0x00007e00   | 0x00008fff | 0x00000200 | 0x000013ff | first FAT                         |
| 0x00009000   | 0x0000a1ff | 0x00001400 | 0x000025ff | second FAT                        |
| 0x0000a200   | 0x0000bdff | 0x00002600 | 0x000041ff | root directory entries            |
| 0x0000be00   | 0x0000c3ff | 0x00004200 | 0x000047ff | loaddisk.bin                      |
| 0x0000c400   | 0x0000cdff | 0x00004800 | 0x000051ff | initscrn.bin                      |
| 0x0000ce00   | 0x0000d1ff | 0x00005200 | 0x000055ff | mv2prtmd.bin                      |
| 0x0000d200   | ?          | 0x00005600 | ?          | kernel.bin                        |
| 0x0009fc00   | 0x0009ffff |            |            | ACPI work area                    |
| 0x000a0000   | 0x000bffff |            |            | unused VRAM                       |
| 0x000c0000   | 0x000c7fff |            |            | Video BIOS                        |
| 0x000c8000   | 0x000effff |            |            | BIOS expansions                   |
| 0x000f0000   | 0x000fffff |            |            | Motherboard BIOS                  |
| 0x00100000   | 0x001fffff |            |            | kernel stack                      |
| 0x00200000   | 0x0020ffff |            |            | GDT                               |
| 0x00210000   | 0x00efffff |            |            | reserved                          |
| 0x00f00000   | 0x00ffffff |            |            | ISA memory hole                   |
| 0x01000000   | ?          |            |            | reserved                          |

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
kernel.binのentry point 0xce00にbreak pointを置き，そこまで進む処理は.gdbinitに書かれている．

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
(gdb) x/8h 0x00100000+$esp
```
0x00100000はkernel.bin実行中のstack領域の開始物理address

### gdbのexamine commandまとめ
文字列を表示
```
(gdb) x/s address
```
命令を表示
```
(gdb) x/i address
```
1byte表示
```
(gdb) x/b address
```
2byte表示
```
(gdb) x/h address
```
4byte表示
```
(gdb) x/w address
```
8byte表示
```
(gdb) x/g address
```

### gdb print command
物理addressに格納されている4 bytesを16進数で表示
```
(gdb) p/x *(unsigned int *)address
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
