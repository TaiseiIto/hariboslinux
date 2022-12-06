# Debug QEMU

## Where does QEMU specify `i386-32bit.xml`?

QEMU specify the XML at `~/qemu/target/i386/cpu.c` line 7145.

```
~/hariboslinux/debug_qemu # make debug-qemu
@ ~/qemu/softmmu/main.c : 46 main
(gdb) break target/i386/cpu.c : 7145
(gdb) continue
(gdb) backtrace
#0  x86_cpu_common_class_init (oc=0x5581fc141440, data=<optimized out>) at ../target/i386/cpu.c:7145
#1  0x00005581fae1324c in type_initialize (ti=0x5581fc108a90) at ../qom/object.c:299
#2  type_initialize (ti=ti@entry=0x5581fc0f4750) at ../qom/object.c:314
#3  0x00005581fae13ce0 in type_initialize (ti=0x5581fc0f4750) at ../qom/object.c:1094
#4  object_class_foreach_tramp (key=<optimized out>, value=0x5581fc0f4750, opaque=0x7ffc4fbc5490)
    at ../qom/object.c:1081
#5  0x00007fd7bac506b8 in g_hash_table_foreach () from /lib/x86_64-linux-gnu/libglib-2.0.so.0
#6  0x00005581fae14205 in object_class_foreach (opaque=0x7ffc4fbc5488, include_abstract=false,
    implements_type=<optimized out>, fn=0x5581fae12250 <object_class_get_list_tramp>) at ../qom/object.c:87
#7  object_class_get_list (implements_type=implements_type@entry=0x5581fb13b30f "machine",
    include_abstract=include_abstract@entry=false) at ../qom/object.c:1160
#8  0x00005581fac00e17 in select_machine (errp=<optimized out>, qdict=0x5581fc1349f0) at ../softmmu/vl.c:1580
#9  qemu_create_machine (qdict=0x5581fc1349f0) at ../softmmu/vl.c:2015
#10 qemu_init (argc=<optimized out>, argv=0x7ffc4fbc5758) at ../softmmu/vl.c:3533
#11 0x00005581faa4475d in main (argc=<optimized out>, argv=<optimized out>) at ../softmmu/main.c:47
(gdb) next
(gdb) print cc->gdb_core_xml_file
$1 = 0x5581fb0a238c "i386-32bit.xml"
```

