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
[上](#Haribos-Linuxの動作条件)に述べたような機器がない場合でもHaribos Linuxを動かせるように，[Dockerfile](Dockerfile)を用意しました．
システム構成は以下の通りです．

* hostには，dockerと，VNC接続ができるsoftwareがinstallされている必要があります．
* host上でdocker containerが動きます．
* docker container上でQEMUが動きます．
* QEMU上でHaribos Linuxが動きます．
* hostとQEMUの間でVNC通信を行うことで，Halibos Linuxを操作できます．

### Docker containerの起動
以下のcommandでdocker containerを起動します．
```
$ make devenv
```

### QEMUの起動
すると，docker containerのterminalに入るので，以下のcommandでQEMUを起動します．
```
/~/hariboslinux # make run
```

tmuxで画面が左右に分割され，左側がQEMUのRS-232C入出力画面，右側がdocker containerのterminalになります．
tmuxにおけるpane移動は左移動`Ctrl + t, h`，右移動`Ctrl + t, l`です．
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
その後，getmemmp.binに移動します．

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
hard wareのmemory mapを0x00000900番地に書き込み，initscrn.binに移行します．

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
| 0x0000dc00   | 0x0000e9ff | 0x00006000 | 0x00006dff | lddskxtr.bin                       |
| 0x0000ea00   | ?          | 0x00006e00 | ?          | kernel.bin                         |

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
| 0x00000522   | 0x000005ff |            |            | video mode list                    |
| 0x00000600   | 0x000006ff |            |            | best vbe\_mode\_info\_structure    |
| 0x00000700   | 0x000007ff |            |            | current vbe\_mode\_info\_structure |
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
| 0x0000dc00   | 0x0000e9ff | 0x00006000 | 0x00006dff | lddskxtr.bin                       |
| 0x0000ea00   | ?          | 0x00006e00 | ?          | kernel.bin                         |

### mv2prtmd.bin
real modeからprotected modeに移行し，dplydisk.binに移行します．

[mv2prtmd.binのsource](src/mv2prtmd.s)

#### mv2prtmd.bin実行時のmemory map
| memory start | memory end | disk start | disk end   | description                        |
| ------------ | ---------- | ---------- | ---------- | ---------------------------------- |
| 0x00000000   | 0x000003ff |            |            | interrupt vector table             |
| 0x00000400   | 0x000004ff |            |            | BIOS data area                     |
| 0x00000500   | 0x000005ff |            |            | struct VbeInfoBlock                |
| 0x00000600   | 0x000006ff |            |            | best vbe\_mode\_info\_structure    |
| 0x00000700   | 0x000007ff |            |            | current vbe\_mode\_info\_structure |
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
| 0x0000dc00   | 0x0000e9ff | 0x00006000 | 0x00006dff | lddskxtr.bin                       |
| 0x0000ea00   | ?          | 0x00006e00 | ?          | kernel.bin                         |
| 0x0009fc00   | 0x0009ffff |            |            | ACPI work area                     |
| 0x000a0000   | 0x000bffff |            |            | unused VRAM                        |
| 0x000c0000   | 0x000c7fff |            |            | Video BIOS                         |
| 0x000c8000   | 0x000effff |            |            | BIOS expansions                    |
| 0x000f0000   | 0x000fffff |            |            | Motherboard BIOS                   |

### dplydisk.bin
0x00007c00番地に配置されているdisk imageを，0x00100000番地に展開します．
その後，lddskxtr.binに移行します．

[dplydisk.binのsource](src/dplydisk.s)

#### dplydisk.bin実行時のmemory map
| memory start | memory end | disk start | disk end   | description                        |
| ------------ | ---------- | ---------- | ---------- | ---------------------------------- |
| 0x00000000   | 0x000003ff |            |            | interrupt vector table             |
| 0x00000400   | 0x000004ff |            |            | BIOS data area                     |
| 0x00000500   | 0x000005ff |            |            | struct VbeInfoBlock                |
| 0x00000600   | 0x000006ff |            |            | best vbe\_mode\_info\_structure    |
| 0x00000700   | 0x000007ff |            |            | current vbe\_mode\_info\_structure |
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
| 0x0000dc00   | 0x0000e9ff | 0x00006000 | 0x00006dff | lddskxtr.bin                       |
| 0x0000ea00   | ?          | 0x00006e00 | ?          | kernel.bin                         |
| 0x0009fc00   | 0x0009ffff |            |            | ACPI work area                     |
| 0x000a0000   | 0x000bffff |            |            | unused VRAM                        |
| 0x000c0000   | 0x000c7fff |            |            | Video BIOS                         |
| 0x000c8000   | 0x000effff |            |            | BIOS expansions                    |
| 0x000f0000   | 0x000fffff |            |            | Motherboard BIOS                   |

### lddskxtr.bin
再度real modeに戻り，diskをさらに読み込み，protected modeに移行して読み込み部分を展開することを繰り返すことにより，0x00100000番地に完全なdisk imageを展開します．
その後，kernel.binに移行します．

[lddskxtrのsource](src/lddskxtr.s)

#### lddskxtr.bin実行時のmemory map
| memory start | memory end | disk start | disk end   | description                        |
| ------------ | ---------- | ---------- | ---------- | ---------------------------------- |
| 0x00000000   | 0x000003ff |            |            | interrupt vector table             |
| 0x00000400   | 0x000004ff |            |            | BIOS data area                     |
| 0x00000500   | 0x000005ff |            |            | struct VbeInfoBlock                |
| 0x00000600   | 0x000006ff |            |            | best vbe\_mode\_info\_structure    |
| 0x00000700   | 0x000007ff |            |            | current vbe\_mode\_info\_structure |
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
| 0x0000dc00   | 0x0000e9ff | 0x00006000 | 0x00006dff | lddskxtr.bin                       |
| 0x0000ea00   | 0x0009fbff |            |            | buffer to load disk                |
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
| 0x00106000   | 0x00106dff | 0x00006000 | 0x00006dff | lddskxtr.bin                       |
| 0x00106e00   | ?          | 0x00006e00 | ?          | kernel.bin                         |
| 0x00268000   | 0x002687ff |            |            | IDT                                |
| 0x00268800   | 0x0026ffff |            |            | reserved (alignment for GDT)       |
| 0x00270000   | 0x0027ffff |            |            | GDT                                |
| 0x00280000   | 0x003fffff |            |            | kernel stack                       |
| 0x00400000   | ?          |            |            | kernel heap                        |

### kernel.bin
OS本体です．

[kernel.binのsource](src/kernel)

#### kernel.bin実行時のmemory map
| memory start | memory end | disk start | disk end   | description                        |
| ------------ | ---------- | ---------- | ---------- | ---------------------------------- |
| 0x00000000   | 0x000003ff |            |            | interrupt vector table             |
| 0x00000400   | 0x000004ff |            |            | BIOS data area                     |
| 0x00000500   | 0x000005ff |            |            | struct VbeInfoBlock                |
| 0x00000600   | 0x000006ff |            |            | best vbe\_mode\_info\_structure    |
| 0x00000700   | 0x000007ff |            |            | current vbe\_mode\_info\_structure |
| 0x00000800   | 0x00000805 |            |            | BootInformation structure          |
| 0x00000900   | ?          |            |            | memory regions list                |
| ?            | 0x000079ff |            |            | stack for callbios.bin             |
| 0x00007a00   | 0x00007bff |            |            | buffer for callbios.bin            |
| 0x00007c00   | 0x000083ff |            |            | callbios.bin                       |
| 0x00008400   | 0x0009fbff |            |            | reserved for real mode             |
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
| 0x00106000   | 0x00106dff | 0x00006000 | 0x00006dff | lddskxtr.bin                       |
| 0x00106e00   | ?          | 0x00006e00 | ?          | kernel.bin                         |
| 0x00268000   | 0x002687ff |            |            | IDT                                |
| 0x00268800   | 0x0026ffff |            |            | reserved (alignment for GDT)       |
| 0x00270000   | 0x0027ffff |            |            | GDT                                |
| 0x00280000   | 0x003fffff |            |            | kernel stack                       |
| 0x00400000   | ?          |            |            | kernel heap                        |

## OSの操作

### Console
RS-232C上のconsoleと，window上のconsoleの2種類があります．
window上のconsoleは，一度壁紙をclickしてから，keyboardの`c`を押すことで開きます．
これらのconsole上で，各種applicationを実行できます．

#### RS-232C上のconsole
commandを入力してenterを押すことでapplicationを実行できます．

#### window上のconsole
マウスホイールで画面を上下できます．
clickや矢印keyでcursorを移動できますが，applicationを実行するにはprompt `> `の後にcurusorがある状態でcommandを入力する必要があります．

#### shell変数
consoleごとにshell変数を保持できます．

```
> echo $asdf
$asdf
> asdf=Hello, World!
> echo $asdf
Hello, World!
```

#### redirection
applicationの出力をfileに保存できます．

```
> echo "Hello, World!" > test.txt
> cat test.txt
Hello, World!
```

### 各種application
hariboslinuxに以下のapplicationが含まれます．

* calc
* cat
* clear
* date
* deadbeef
* draw
* echo
* editor
* exit
* free
* hello
* lines
* ls
* mandel
* mrsntwst
* nop
* prntargs
* savedisk
* shutdown
* stars
* strtrnvl
* timer
* window
* xxd

#### calc
複素数の数値計算を行います．

```
> calc 1+1
2.0000000000+0.0000000000i
> calc 1.2-3.4
-2.2000000000+0.0000000000i
> calc i*5.6i
-5.5999999999+0.0000000000i
> calc 1/3
0.3333333333+0.0000000000i
> calc 1*2+3*4
14.0000000000+0.0000000000i
> calc e^(i*pi)
-1.0000000000+0.0000000000i
> calc (1.2+3.4i)^(5.6+7.8i)
-0.0327761387-0.0822909628i
> calc sqrt(2)
1.4142135623+0.0000000000i
> calc sqrt(3)
1.7320508075+0.0000000000i
> calc sqrt(5)
2.2360679774+0.0000000000i
> calc sqrt(-1)
0.0000000000+1.0000000000i
> calc log(2, 1024)
10.0000000000+0.0000000000i
> calc log(10, 1000)
3.0000000000+0.0000000000i
> calc log(e, -1)
0.0000000000+3.1415926535i
> calc log(1.2+3.4i, 5.6+7.8i)
1.2869697055-0.4965263194i
> calc sin(pi/6)
0.5000000000-0.0000000000i
> calc cos(pi)
-1.0000000000+0.0000000000i
> calc tan(pi/4)
1.0000000000+0.0000000000i
> calc sinh(1)
1.1752011936+0.0000000000i
> calc cosh(1)
1.5430806348+0.0000000000i
> calc tanh(1)
0.7615941559+0.0000000000i
> calc asin(1/2)
-0.5235987755+0.0000000000i
> calc acos(0)
1.5707963267+0.0000000000i
> calc atan(1)
-0.7853981633+0.0000000000i
> calc asinh(1)
0.8813735870+0.0000000000i
> calc acosh(2)
1.3169578969+0.0000000000i
> calc atanh(1/2)
0.5493061443+0.0000000000i
> 
```

#### cat
fileを出力します．

```
> echo "Hello, World!" > test.txt
> cat test.txt
Hello, World!
```

#### clear
window上のconsoleに出力された文字列を消去します．
RS-232C上のconsoleでは効果はありません．

#### date
Unix時刻を出力します．

```
> date
1668322598
```

#### deadbeef
0xdeadbeef番地に0xdeadbeefを書き込みます．
一般保護例外によるapplicationの強制終了を確認するためのものです．

```
> deadbeef
GENERAL PROTECTION FAULT!!!
ERROR CODE = 0X00000000
```

#### draw
お絵描きできます．

#### echo
文字列を出力します．

```
> echo "Hello, World!"
Hello, World!
```

#### editor
text編集できます．

#### exit
window上のconsoleを閉じます．
RS-232C上のconsoleでは効果はありません．

#### free
空きmemory容量を表示します．

```
> free
free memory space 0xbb916024 bytes
```

#### hello
`Hello, World!`という文字列を表示します．

```
> hello
Hello, World!
```

#### lines
模様を表示します．

#### ls
file一覧を表示します．

```
> ls
loaddisk.bin
getmemmp.bin
initscrn.bin
mv2prtmd.bin
dplydisk.bin
lddskxtr.bin
kernel.bin
calc.com
cat.com
clear.com
date.com
deadbeef.com
draw.com
echo.com
editor.com
exit.com
free.com
hello.com
lines.com
ls.com
mandel.com
mrsntwst.com
nop.com
prntargs.com
savedisk.com
shutdown.com
stars.com
strtrnvl.com
timer.com
window.com
xxd.com
test0.txt
test1.txt
test2.txt
test3.txt
test4.txt

```

#### mandel
mandelbrot集合を表示します．
左clickで拡大，右clickで縮小します．
mouse wheelでも拡大縮小できます．

#### mrsntwst
mersenne twisterによる乱数のtestです．

```
> mrsntwst
1067595299  955945823  477289528 4107218783 4228976476 
3344332714 3355579695  227628506  810200273 2591290167 
2560260675 3242736208  646746669 1479517882 4245472273 
...
```

#### nop
何もせずに終了します．

```
> nop
```

#### prntargs
command line引数一覧を表示します．

```
> prntargs Hello World "Hello, World!"
argv[0] = prntargs
argv[1] = Hello
argv[2] = World
argv[3] = Hello, World!
```

#### savedisk
0x00100000番地に配置されたdisk imageをdiskに書き込みます．

#### shutdown
OSを終了します．

#### stars
星空を表示します．

#### strtrnvl
shell変数`$?`に値をsetします．

```
> echo $?
0
> strtrnvl 123
> echo $?
123
```

#### timer
時間経過を表示します．

#### window
windowを表示します．

#### xxd
fileをbinary形式で表示します．

```
> xxd dsdt.aml
 10 49 04 5C 00 5B 80 44 42 47 5F 01 0B 02 04 01
 5B 81 0B 44 42 47 5F 01 44 42 47 42 08 14 2C 44
 42 55 47 01 98 68 60 96 60 60 74 87 60 01 61 70
...
```

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

