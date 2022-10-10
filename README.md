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
floppy diskをmemoryに読み込み，getmemmp.binに移行します．
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

### getmemmp.bin
hard wareのmemory mapを0x00000900番地から書き込み，initscrn.binに移行します．

[getmemmp.binのsource](src/getmemmp.s)

#### getmemmp.bin実行時のmemory map
| memory start | memory end | disk start | disk end   | description                        |
| ------------ | ---------- | ---------- | ---------- | ---------------------------------- |
| 0x00000000   | 0x000003ff |            |            | interrupt vector table             |
| 0x00000400   | 0x000004ff |            |            | BIOS data area                     |
| 0x00000800   | 0x00000805 |            |            | BootInformation structure          |
| 0x00000900   | ?          |            |            | memory regions list                |
| ?            | 0x00007bff |            |            | stack                              |
| 0x00007c00   | 0x0009fbff | 0x00000000 | 0x00097fff | loaded disk data                   |
| 0x00007c00   | 0x00007dff | 0x00000000 | 0x000001ff | bootsector.bin                     |
| 0x00007e00   | 0x00008fff | 0x00000200 | 0x000013ff | first FAT                          |
| 0x00009000   | 0x0000a1ff | 0x00001400 | 0x000025ff | second FAT                         |
| 0x0000a200   | 0x0000bdff | 0x00002600 | 0x000041ff | root directory entries             |
| 0x0000be00   | 0x0000c3ff | 0x00004200 | 0x000047ff | loaddisk.bin                       |
| 0x0000c400   | 0x0000c9ff | 0x00004800 | 0x00004dff | getmemmp.bin                       |
| 0x0000ca00   | 0x0000d3ff | 0x00004e00 | 0x000057ff | initscrn.bin                       |
| 0x0000d400   | 0x0000d7ff | 0x00005800 | 0x00005bff | mv2prtmd.bin                       |
| 0x0000d800   | 0x0000dbff | 0x00005c00 | 0x00005fff | dplydisk.bin                       |
| 0x0000dc00   | ?          | 0x00006000 | ?          | kernel.bin                         |

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
| memory start | memory end | disk start | disk end   | description                        |
| ------------ | ---------- | ---------- | ---------- | ---------------------------------- |
| 0x00000000   | 0x000003ff |            |            | interrupt vector table             |
| 0x00000400   | 0x000004ff |            |            | BIOS data area                     |
| 0x00000500   | 0x00000514 |            |            | struct VbeInfoBlock                |
| 0x00000522   | 0x00000600 |            |            | video mode list                    |
| 0x00000600   | 0x00000700 |            |            | best vbe\_mode\_info\_structure    |
| 0x00000700   | 0x00000800 |            |            | current vbe\_mode\_info\_structure |
| 0x00000800   | 0x00000805 |            |            | BootInformation structure          |
| 0x00000900   | ?          |            |            | memory regions list                |
| ?            | 0x00007bff |            |            | stack                              |
| 0x00007c00   | 0x0009fbff | 0x00000000 | 0x00097fff | loaded disk data                   |
| 0x00007c00   | 0x00007dff | 0x00000000 | 0x000001ff | bootsector.bin                     |
| 0x00007e00   | 0x00008fff | 0x00000200 | 0x000013ff | first FAT                          |
| 0x00009000   | 0x0000a1ff | 0x00001400 | 0x000025ff | second FAT                         |
| 0x0000a200   | 0x0000bdff | 0x00002600 | 0x000041ff | root directory entries             |
| 0x0000be00   | 0x0000c3ff | 0x00004200 | 0x000047ff | loaddisk.bin                       |
| 0x0000c400   | 0x0000c9ff | 0x00004800 | 0x00004dff | getmemmp.bin                       |
| 0x0000ca00   | 0x0000d3ff | 0x00004e00 | 0x000057ff | initscrn.bin                       |
| 0x0000d400   | 0x0000d7ff | 0x00005800 | 0x00005bff | mv2prtmd.bin                       |
| 0x0000d800   | 0x0000dbff | 0x00005c00 | 0x00005fff | dplydisk.bin                       |
| 0x0000dc00   | ?          | 0x00006000 | ?          | kernel.bin                         |

### mv2prtmd.bin
real modeからprotected modeに移行し，dplydisk.binに移行します．

[mv2prtmd.binのsource](src/mv2prtmd.s)

#### mv2prtmd.bin実行時のmemory map
| memory start | memory end | disk start | disk end   | description                        |
| ------------ | ---------- | ---------- | ---------- | ---------------------------------- |
| 0x00000000   | 0x000003ff |            |            | interrupt vector table             |
| 0x00000400   | 0x000004ff |            |            | BIOS data area                     |
| 0x00000500   | 0x00000600 |            |            | struct VbeInfoBlock                |
| 0x00000600   | 0x00000700 |            |            | best vbe\_mode\_info\_structure    |
| 0x00000700   | 0x00000800 |            |            | current vbe\_mode\_info\_structure |
| 0x00000800   | 0x00000805 |            |            | BootInformation structure          |
| 0x00000900   | ?          |            |            | memory regions list                |
| ?            | 0x00007bff |            |            | stack                              |
| 0x00007c00   | 0x0009fbff | 0x00000000 | 0x00097fff | loaded disk data                   |
| 0x00007c00   | 0x00007dff | 0x00000000 | 0x000001ff | bootsector.bin                     |
| 0x00007e00   | 0x00008fff | 0x00000200 | 0x000013ff | first FAT                          |
| 0x00009000   | 0x0000a1ff | 0x00001400 | 0x000025ff | second FAT                         |
| 0x0000a200   | 0x0000bdff | 0x00002600 | 0x000041ff | root directory entries             |
| 0x0000be00   | 0x0000c3ff | 0x00004200 | 0x000047ff | loaddisk.bin                       |
| 0x0000c400   | 0x0000c9ff | 0x00004800 | 0x00004dff | getmemmp.bin                       |
| 0x0000ca00   | 0x0000d3ff | 0x00004e00 | 0x000057ff | initscrn.bin                       |
| 0x0000d400   | 0x0000d7ff | 0x00005800 | 0x00005bff | mv2prtmd.bin                       |
| 0x0000d800   | 0x0000dbff | 0x00005c00 | 0x00005fff | dplydisk.bin                       |
| 0x0000dc00   | ?          | 0x00006000 | ?          | kernel.bin                         |
| 0x0009fc00   | 0x0009ffff |            |            | ACPI work area                     |
| 0x000a0000   | 0x000bffff |            |            | unused VRAM                        |
| 0x000c0000   | 0x000c7fff |            |            | Video BIOS                         |
| 0x000c8000   | 0x000effff |            |            | BIOS expansions                    |
| 0x000f0000   | 0x000fffff |            |            | Motherboard BIOS                   |

### dplydisk.bin
0x00007c00番地に配置されているdisk imageを，0x00100000番地に展開します．
現在のversionでは，disk imageは0x00097fff番地までしか読み込んでいません．
もしdisk上のこれより先の部分を使用する場合，dplydisk.binにおいて一時的にreal modeに戻って読み込んでください．

[dplydisk.binのsource](src/dplydisk.s)

#### mv2prtmd.bin実行時のmemory map
| memory start | memory end | disk start | disk end   | description                        |
| ------------ | ---------- | ---------- | ---------- | ---------------------------------- |
| 0x00000000   | 0x000003ff |            |            | interrupt vector table             |
| 0x00000400   | 0x000004ff |            |            | BIOS data area                     |
| 0x00000500   | 0x00000600 |            |            | struct VbeInfoBlock                |
| 0x00000600   | 0x00000700 |            |            | best vbe\_mode\_info\_structure    |
| 0x00000700   | 0x00000800 |            |            | current vbe\_mode\_info\_structure |
| 0x00000800   | 0x00000805 |            |            | BootInformation structure          |
| 0x00000900   | ?          |            |            | memory regions list                |
| ?            | 0x00007bff |            |            | stack                              |
| 0x00007c00   | 0x0009fbff | 0x00000000 | 0x00097fff | loaded disk data                   |
| 0x00007c00   | 0x00007dff | 0x00000000 | 0x000001ff | bootsector.bin                     |
| 0x00007e00   | 0x00008fff | 0x00000200 | 0x000013ff | first FAT                          |
| 0x00009000   | 0x0000a1ff | 0x00001400 | 0x000025ff | second FAT                         |
| 0x0000a200   | 0x0000bdff | 0x00002600 | 0x000041ff | root directory entries             |
| 0x0000be00   | 0x0000c3ff | 0x00004200 | 0x000047ff | loaddisk.bin                       |
| 0x0000c400   | 0x0000c9ff | 0x00004800 | 0x00004dff | getmemmp.bin                       |
| 0x0000ca00   | 0x0000d3ff | 0x00004e00 | 0x000057ff | initscrn.bin                       |
| 0x0000d400   | 0x0000d7ff | 0x00005800 | 0x00005bff | mv2prtmd.bin                       |
| 0x0000d800   | 0x0000dbff | 0x00005c00 | 0x00005fff | dplydisk.bin                       |
| 0x0000dc00   | ?          | 0x00006000 | ?          | kernel.bin                         |
| 0x0009fc00   | 0x0009ffff |            |            | ACPI work area                     |
| 0x000a0000   | 0x000bffff |            |            | unused VRAM                        |
| 0x000c0000   | 0x000c7fff |            |            | Video BIOS                         |
| 0x000c8000   | 0x000effff |            |            | BIOS expansions                    |
| 0x000f0000   | 0x000fffff |            |            | Motherboard BIOS                   |

### kernel.bin
OS本体です．

[kernel.binのsource](src/kernel)

#### kernel.bin実行時のmemory map
| memory start | memory end | disk start | disk end   | description                        |
| ------------ | ---------- | ---------- | ---------- | ---------------------------------- |
| 0x00000000   | 0x000003ff |            |            | interrupt vector table             |
| 0x00000400   | 0x000004ff |            |            | BIOS data area                     |
| 0x00000500   | 0x00000600 |            |            | struct VbeInfoBlock                |
| 0x00000600   | 0x00000700 |            |            | best vbe\_mode\_info\_structure    |
| 0x00000700   | 0x00000800 |            |            | current vbe\_mode\_info\_structure |
| 0x00000800   | 0x00000805 |            |            | BootInformation structure          |
| 0x00000900   | ?          |            |            | memory regions list                |
| 0x00007c00   | 0x0009fbff |            |            | reserved (for real mode execution) |
| 0x0009fc00   | 0x0009ffff |            |            | ACPI work area                     |
| 0x000a0000   | 0x000bffff |            |            | unused VRAM                        |
| 0x000c0000   | 0x000c7fff |            |            | Video BIOS                         |
| 0x000c8000   | 0x000effff |            |            | BIOS expansions                    |
| 0x000f0000   | 0x000fffff |            |            | Motherboard BIOS                   |
| 0x00100000   | 0x00267fff | 0x00000000 | 0x00167fff | loaded disk data                   |
| 0x00100000   | 0x001001ff | 0x00000000 | 0x000001ff | bootsector.bin                     |
| 0x00100200   | 0x001013ff | 0x00000200 | 0x000013ff | first FAT                          |
| 0x00101400   | 0x001025ff | 0x00001400 | 0x000025ff | second FAT                         |
| 0x00102600   | 0x001041ff | 0x00002600 | 0x000041ff | root directory entries             |
| 0x00104200   | 0x001047ff | 0x00004200 | 0x000047ff | loaddisk.bin                       |
| 0x00104800   | 0x00104dff | 0x00004800 | 0x00004dff | getmemmp.bin                       |
| 0x00104e00   | 0x001057ff | 0x00004e00 | 0x000057ff | initscrn.bin                       |
| 0x00105800   | 0x00105bff | 0x00005800 | 0x00005bff | mv2prtmd.bin                       |
| 0x00105c00   | 0x00105fff | 0x00005c00 | 0x00005fff | dplydisk.bin                       |
| 0x00106000   | ?          | 0x00006000 | ?          | kernel.bin                         |
| 0x00268000   | 0x002687ff |            |            | IDT                                |
| 0x00268800   | 0x0026ffff |            |            | reserved (alignment for GDT)       |
| 0x00270000   | 0x0027ffff |            |            | GDT                                |
| 0x00280000   | 0x002fffff |            |            | kernel stack                       |
| 0x00300000   | 0x003fffff |            |            | reserved (for future expantion)    |
| 0x00400000   | ?          |            |            | kernel heap                        |

## 開発者用メモ
docker, VNC softwareに加え，git, makeを用いて開発しています．

### clone repository
このrepositoryをcloneします．
```
$ git clone https://github.com/TaiseiIto/hariboslinux.git
$ cd hariboslinux
```

### Docker image生成
以下のcommandでdocker環境が準備されます．
```
$ make devenv
/~/hariboslinux #
```

以下のコマンドでcontainerから抜け，containerを停止します．

```
/~/hariboslinux # exit
```

以下のコマンドでdocker環境を作り直します．
```
$ make rebuild-devenv
```

以下のコマンドでdocker環境を削除します．
```
$ make clean-devenv
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
/~/hariboslinux # make gitconfig KEY=path/to/ssh/key GPG=path/to/.gnupg
Your GitHub email:email@address
Your GitHub name:TaiseiIto
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

## 実機(Legacy BIOS対応ASUS)での動かし方memo
1. bootable USBの用意
	1. explorerでboot用USBを右クリックし，Formatして空の状態にする．
	1. rufusでboot用USBにharibos.imgを書き込む．
	1. diskmgmt.mscを起動し，bootable USBを右クリックしてDrive Letterを与える．
	1. explorerでbootable USBを右クリックし，ejectしてから取り外す．
1. 実機のBIOS設定変更
	1. 起動前の実機にbootable USBを差す．
	1. 実機を起動し，設定画面からUpdate and Security/Recovery/Advanced startup/Restart now/Troubleshoot/Advanced options/UEFI Firmware Settings/RestartでBIOS設定画面を開く．
	1. Security/Secure Boot menu/Secure Boot ControlをDisableにする．
	1. Boot/Launch CSMをEnableにする．
	1. Boot/Boot Option #1をbootable USBにする．
	1. 再起動
1. bootable USBからの起動
	1. 設定画面からUpdate and Security/Recovery/Advanced startup/Restart now/Use a device/Removable Drive
1. 電源ボタンを押してharibos終了
1. BIOS設定を戻す
	1. BIOS設定画面を開く
	1. Security/Secure Boot menu/Secure Boot ControlをEnableにする．
	1. Boot/Launch CSMをDisableにする．
	1. Boot/Boot Option #1をWindows Boot Managerにする．
	1. 再起動

