# db_bench V0

```
xxy@LAPTOP-LOMHUK6P:~/bench/XOY-leveldb-exp2/build$ ./db_bench
LevelDB:    version 1.23
Date:       Sun Jan  5 14:40:05 2025
CPU:        24 * AMD Ryzen AI 9 HX 370 w/ Radeon 890M
CPUCache:   1024 KB
Keys:       16 bytes each
Values:     5000 bytes each (2500 bytes after compression)
Entries:    1000000
RawSize:    4783.6 MB (estimated)
FileSize:   2399.4 MB (estimated)
WARNING: Snappy compression is not enabled
------------------------------------------------
fillseq      :      18.288 micros/op;  261.6 MB/s     
fillsync     :    1467.256 micros/op;    3.3 MB/s (1000 ops)
fillrandom   :     180.092 micros/op;   26.6 MB/s     
overwrite    :     223.800 micros/op;   21.4 MB/s     
readrandom   :      26.436 micros/op; (864322 of 1000000 found)
readrandom   :      17.695 micros/op; (864083 of 1000000 found)
readseq      :       0.586 micros/op; 8166.4 MB/s    
readreverse  :       0.841 micros/op; 5687.0 MB/s    
compact      : 15983532.000 micros/op;
readrandom   :      10.778 micros/op; (864105 of 1000000 found)
readseq      :       0.471 micros/op; 10166.7 MB/s   
readreverse  :       0.751 micros/op; 6370.9 MB/s    
fill100K     :     516.302 micros/op;  184.7 MB/s (1000 ops)
crc32c       :       0.560 micros/op; 6981.2 MB/s (4K per op)
snappycomp   :     876.000 micros/op; (snappy failure)
snappyuncomp :     812.000 micros/op; (snappy failure)
zstdcomp     :     884.000 micros/op; (zstd failure)
zstduncomp   :     849.000 micros/op; (zstd failure)
```



# db_bench V3

### origin

```
xxy@LAPTOP-LOMHUK6P:~/bench/XOY-leveldb-exp2/build$ ./db_bench
LevelDB:    version 1.23
Date:       Sat Jan  4 21:42:01 2025
CPU:        24 * AMD Ryzen AI 9 HX 370 w/ Radeon 890M
CPUCache:   1024 KB
Keys:       16 bytes each
Values:     5000 bytes each (2500 bytes after compression)
Entries:    1000000
RawSize:    4783.6 MB (estimated)
FileSize:   2399.4 MB (estimated)
WARNING: Snappy compression is not enabled
------------------------------------------------
fillseq      :       9.424 micros/op;  507.6 MB/s     
fillsync     :    1322.246 micros/op;    3.6 MB/s (1000 ops)
fillrandom   :      16.385 micros/op;  292.0 MB/s     
overwrite    :      43.718 micros/op;  109.4 MB/s     
readrandom   :       5.891 micros/op; (864322 of 1000000 found)
readrandom   :       5.257 micros/op; (864083 of 1000000 found)
readunorderseq :       1.249 micros/op; 3828.8 MB/s  
readseq      :       3.241 micros/op; 1475.9 MB/s    
readreverse  :       3.325 micros/op; 1438.7 MB/s    
compact      :  151395.000 micros/op;
readrandom   :      30.305 micros/op; (864482 of 1000000 found)
readunorderseq :       1.287 micros/op; 3717.1 MB/s  
readseq      :       3.357 micros/op; 1424.9 MB/s    
readreverse  :       3.332 micros/op; 1435.9 MB/s    
fill100K     :      49.125 micros/op; 1941.6 MB/s (1000 ops)
crc32c       :       0.576 micros/op; 6784.2 MB/s (4K per op)
snappycomp   :     894.000 micros/op; (snappy failure)
snappyuncomp :     837.000 micros/op; (snappy failure)
zstdcomp     :     849.000 micros/op; (zstd failure)
zstduncomp   :     861.000 micros/op; (zstd failure)
```

Options.valuelog_crc=true + ReadOptions.verify_checksums=true

```
xxy@LAPTOP-LOMHUK6P:~/bench/XOY-leveldb-exp2/build$ ./db_bench
LevelDB:    version 1.23
Date:       Sat Jan  4 21:54:09 2025
CPU:        24 * AMD Ryzen AI 9 HX 370 w/ Radeon 890M
CPUCache:   1024 KB
Keys:       16 bytes each
Values:     5000 bytes each (2500 bytes after compression)
Entries:    1000000
RawSize:    4783.6 MB (estimated)
FileSize:   2399.4 MB (estimated)
WARNING: Snappy compression is not enabled
------------------------------------------------
fillseq      :      11.596 micros/op;  412.5 MB/s     
fillsync     :    1295.241 micros/op;    3.7 MB/s (1000 ops)
fillrandom   :      15.880 micros/op;  301.2 MB/s     
overwrite    :      47.611 micros/op;  100.5 MB/s     
readrandom   :       6.641 micros/op; (864322 of 1000000 found)
readrandom   :       4.998 micros/op; (864083 of 1000000 found)
readunorderseq :       1.256 micros/op; 3810.1 MB/s  
readseq      :       3.114 micros/op; 1536.0 MB/s    
readreverse  :       3.202 micros/op; 1493.9 MB/s    
compact      :  164088.000 micros/op;
readrandom   :      32.763 micros/op; (864482 of 1000000 found)
readunorderseq :       1.270 micros/op; 3767.4 MB/s  
readseq      :       3.389 micros/op; 1411.5 MB/s    
readreverse  :       4.951 micros/op;  966.2 MB/s    
fill100K     :      62.256 micros/op; 1532.1 MB/s (1000 ops)
crc32c       :       0.561 micros/op; 6967.7 MB/s (4K per op)
snappycomp   :    1226.000 micros/op; (snappy failure)
snappyuncomp :    1341.000 micros/op; (snappy failure)
zstdcomp     :    1312.000 micros/op; (zstd failure)
zstduncomp   :    1069.000 micros/op; (zstd failure)
```

options.max_unorder_iter_memory_usage=64<<24;

```
xxy@LAPTOP-LOMHUK6P:~/bench/XOY-leveldb-exp2/build$ ./db_bench
LevelDB:    version 1.23
Date:       Sat Jan  4 22:01:02 2025
CPU:        24 * AMD Ryzen AI 9 HX 370 w/ Radeon 890M
CPUCache:   1024 KB
Keys:       16 bytes each
Values:     5000 bytes each (2500 bytes after compression)
Entries:    1000000
RawSize:    4783.6 MB (estimated)
FileSize:   2399.4 MB (estimated)
WARNING: Snappy compression is not enabled
------------------------------------------------
fillseq      :      10.441 micros/op;  458.1 MB/s     
fillsync     :    1407.271 micros/op;    3.4 MB/s (1000 ops)
fillrandom   :      13.759 micros/op;  347.7 MB/s     
overwrite    :      44.691 micros/op;  107.0 MB/s     
readrandom   :       5.752 micros/op; (864322 of 1000000 found)
readrandom   :       4.213 micros/op; (864083 of 1000000 found)
readunorderseq :       1.252 micros/op; 3822.2 MB/s  
readseq      :       3.117 micros/op; 1534.8 MB/s    
readreverse  :       3.200 micros/op; 1494.8 MB/s    
compact      :  151401.000 micros/op;
readrandom   :      29.261 micros/op; (864482 of 1000000 found)
readunorderseq :       1.351 micros/op; 3540.8 MB/s  
readseq      :       3.096 micros/op; 1545.2 MB/s    
readreverse  :       3.146 micros/op; 1520.6 MB/s    
fill100K     :      46.554 micros/op; 2048.9 MB/s (1000 ops)
crc32c       :       0.553 micros/op; 7070.0 MB/s (4K per op)
snappycomp   :    1136.000 micros/op; (snappy failure)
snappyuncomp :     839.000 micros/op; (snappy failure)
zstdcomp     :     817.000 micros/op; (zstd failure)
zstduncomp   :     811.000 micros/op; (zstd failure)
```



options.max_unorder_iter_memory_usage=64<<22;

```
xxy@LAPTOP-LOMHUK6P:~/bench/XOY-leveldb-exp2/build$ ./db_bench
LevelDB:    version 1.23
Date:       Sat Jan  4 22:09:37 2025
CPU:        24 * AMD Ryzen AI 9 HX 370 w/ Radeon 890M
CPUCache:   1024 KB
Keys:       16 bytes each
Values:     5000 bytes each (2500 bytes after compression)
Entries:    1000000
RawSize:    4783.6 MB (estimated)
FileSize:   2399.4 MB (estimated)
WARNING: Snappy compression is not enabled
------------------------------------------------
fillseq      :       9.149 micros/op;  522.9 MB/s     
fillsync     :    1260.337 micros/op;    3.8 MB/s (1000 ops)
fillrandom   :      16.319 micros/op;  293.1 MB/s     
overwrite    :      43.827 micros/op;  109.1 MB/s     
readrandom   :       5.710 micros/op; (864322 of 1000000 found)
readrandom   :       4.262 micros/op; (864083 of 1000000 found)
readunorderseq :       1.242 micros/op; 3850.3 MB/s  
readseq      :       4.794 micros/op;  997.8 MB/s    
readreverse  :       3.254 micros/op; 1470.1 MB/s    
compact      :  146958.000 micros/op;
readrandom   :      30.457 micros/op; (864482 of 1000000 found)
readunorderseq :       1.238 micros/op; 3864.2 MB/s  
readseq      :       3.074 micros/op; 1556.0 MB/s    
readreverse  :       3.119 micros/op; 1533.8 MB/s    
fill100K     :      45.835 micros/op; 2081.0 MB/s (1000 ops)
crc32c       :       0.545 micros/op; 7166.9 MB/s (4K per op)
snappycomp   :     858.000 micros/op; (snappy failure)
snappyuncomp :     821.000 micros/op; (snappy failure)
zstdcomp     :     840.000 micros/op; (zstd failure)
zstduncomp   :     845.000 micros/op; (zstd failure)
```

options.max_unorder_iter_memory_usage=64<<18;

```
xxy@LAPTOP-LOMHUK6P:~/bench/XOY-leveldb-exp2/build$ ./db_bench
LevelDB:    version 1.23
Date:       Sat Jan  4 22:13:46 2025
CPU:        24 * AMD Ryzen AI 9 HX 370 w/ Radeon 890M
CPUCache:   1024 KB
Keys:       16 bytes each
Values:     5000 bytes each (2500 bytes after compression)
Entries:    1000000
RawSize:    4783.6 MB (estimated)
FileSize:   2399.4 MB (estimated)
WARNING: Snappy compression is not enabled
------------------------------------------------
fillseq      :       9.094 micros/op;  526.0 MB/s     
fillsync     :    1267.120 micros/op;    3.8 MB/s (1000 ops)
fillrandom   :      16.316 micros/op;  293.2 MB/s     
overwrite    :      43.970 micros/op;  108.8 MB/s     
readrandom   :       5.859 micros/op; (864322 of 1000000 found)
readrandom   :       4.303 micros/op; (864083 of 1000000 found)
readunorderseq :       1.255 micros/op; 3810.6 MB/s  
readseq      :       3.203 micros/op; 1493.6 MB/s    
readreverse  :       4.932 micros/op;  969.9 MB/s    
compact      :  149083.000 micros/op;
readrandom   :      29.995 micros/op; (864482 of 1000000 found)
readunorderseq :       1.208 micros/op; 3958.6 MB/s  
readseq      :       3.100 micros/op; 1542.9 MB/s    
readreverse  :       3.137 micros/op; 1524.7 MB/s    
fill100K     :      48.072 micros/op; 1984.2 MB/s (1000 ops)
crc32c       :       0.571 micros/op; 6842.8 MB/s (4K per op)
snappycomp   :     928.000 micros/op; (snappy failure)
snappyuncomp :     828.000 micros/op; (snappy failure)
zstdcomp     :    1233.000 micros/op; (zstd failure)
zstduncomp   :    1360.000 micros/op; (zstd failure)
```



options.mem_value_log_number=512;

```
xxy@LAPTOP-LOMHUK6P:~/bench/XOY-leveldb-exp2/build$ ./db_bench
LevelDB:    version 1.23
Date:       Sat Jan  4 22:18:38 2025
CPU:        24 * AMD Ryzen AI 9 HX 370 w/ Radeon 890M
CPUCache:   1024 KB
Keys:       16 bytes each
Values:     5000 bytes each (2500 bytes after compression)
Entries:    1000000
RawSize:    4783.6 MB (estimated)
FileSize:   2399.4 MB (estimated)
WARNING: Snappy compression is not enabled
------------------------------------------------
fillseq      :       9.431 micros/op;  507.2 MB/s     
fillsync     :    1407.629 micros/op;    3.4 MB/s (1000 ops)
fillrandom   :      15.357 micros/op;  311.5 MB/s     
overwrite    :      43.646 micros/op;  109.6 MB/s     
readrandom   :       4.007 micros/op; (864322 of 1000000 found)
readrandom   :       2.150 micros/op; (864083 of 1000000 found)
readunorderseq :       1.298 micros/op; 3684.5 MB/s  
readseq      :       2.678 micros/op; 1786.2 MB/s    
readreverse  :       1.019 micros/op; 4693.5 MB/s    
compact      :  121448.000 micros/op;
readrandom   :      22.242 micros/op; (864482 of 1000000 found)
readunorderseq :       1.296 micros/op; 3691.2 MB/s  
readseq      :       3.521 micros/op; 1358.5 MB/s    
readreverse  :       1.003 micros/op; 4770.5 MB/s    
fill100K     :      47.101 micros/op; 2025.1 MB/s (1000 ops)
crc32c       :       0.567 micros/op; 6889.1 MB/s (4K per op)
snappycomp   :     874.000 micros/op; (snappy failure)
snappyuncomp :     831.000 micros/op; (snappy failure)
zstdcomp     :     840.000 micros/op; (zstd failure)
zstduncomp   :     806.000 micros/op; (zstd failure)

xxy@LAPTOP-LOMHUK6P:~/bench/XOY-leveldb-exp2/build$ ./db_bench
LevelDB:    version 1.23
Date:       Sat Jan  4 22:22:50 2025
CPU:        24 * AMD Ryzen AI 9 HX 370 w/ Radeon 890M
CPUCache:   1024 KB
Keys:       16 bytes each
Values:     5000 bytes each (2500 bytes after compression)
Entries:    1000000
RawSize:    4783.6 MB (estimated)
FileSize:   2399.4 MB (estimated)
WARNING: Snappy compression is not enabled
------------------------------------------------
fillseq      :       9.114 micros/op;  524.8 MB/s     
fillsync     :    1350.784 micros/op;    3.5 MB/s (1000 ops)
fillrandom   :      16.186 micros/op;  295.5 MB/s     
overwrite    :      45.896 micros/op;  104.2 MB/s     
readrandom   :       3.767 micros/op; (864322 of 1000000 found)
readrandom   :       3.586 micros/op; (864083 of 1000000 found)
readunorderseq :       1.335 micros/op; 3583.2 MB/s  
readseq      :       0.969 micros/op; 4935.5 MB/s    
readreverse  :       1.035 micros/op; 4621.9 MB/s    
compact      :  124413.000 micros/op;
readrandom   :      22.616 micros/op; (864482 of 1000000 found)
readunorderseq :       1.379 micros/op; 3468.0 MB/s  
readseq      :       4.246 micros/op; 1126.5 MB/s    
readreverse  :       1.041 micros/op; 4594.6 MB/s    
fill100K     :      45.569 micros/op; 2093.1 MB/s (1000 ops)
crc32c       :       0.564 micros/op; 6929.8 MB/s (4K per op)
snappycomp   :     860.000 micros/op; (snappy failure)
snappyuncomp :     831.000 micros/op; (snappy failure)
zstdcomp     :     809.000 micros/op; (zstd failure)
zstduncomp   :     809.000 micros/op; (zstd failure)

xxy@LAPTOP-LOMHUK6P:~/bench/XOY-leveldb-exp2/build$ ./db_bench
LevelDB:    version 1.23
Date:       Sat Jan  4 22:25:35 2025
CPU:        24 * AMD Ryzen AI 9 HX 370 w/ Radeon 890M
CPUCache:   1024 KB
Keys:       16 bytes each
Values:     5000 bytes each (2500 bytes after compression)
Entries:    1000000
RawSize:    4783.6 MB (estimated)
FileSize:   2399.4 MB (estimated)
WARNING: Snappy compression is not enabled
------------------------------------------------
fillseq      :      10.441 micros/op;  458.2 MB/s     
fillsync     :    1352.363 micros/op;    3.5 MB/s (1000 ops)
fillrandom   :      14.264 micros/op;  335.4 MB/s     
overwrite    :      47.653 micros/op;  100.4 MB/s     
readrandom   :       3.749 micros/op; (864322 of 1000000 found)
readrandom   :       2.173 micros/op; (864083 of 1000000 found)
readunorderseq :       1.313 micros/op; 3644.4 MB/s  
readseq      :       0.994 micros/op; 4812.8 MB/s    
readreverse  :       1.056 micros/op; 4531.1 MB/s    
compact      :  146978.000 micros/op;
readrandom   :      26.075 micros/op; (864482 of 1000000 found)
readunorderseq :       1.373 micros/op; 3484.7 MB/s  
readseq      :       1.621 micros/op; 2950.7 MB/s    
readreverse  :       1.050 micros/op; 4557.6 MB/s    
fill100K     :      52.272 micros/op; 1824.7 MB/s (1000 ops)
crc32c       :       0.571 micros/op; 6839.2 MB/s (4K per op)
snappycomp   :     910.000 micros/op; (snappy failure)
snappyuncomp :     968.000 micros/op; (snappy failure)
zstdcomp     :    1295.000 micros/op; (zstd failure)
zstduncomp   :     982.000 micros/op; (zstd failure)
```



options.mem_value_log_number=32

```
xxy@LAPTOP-LOMHUK6P:~/bench/XOY-leveldb-exp2/build$ ./db_bench
LevelDB:    version 1.23
Date:       Sat Jan  4 23:12:43 2025
CPU:        24 * AMD Ryzen AI 9 HX 370 w/ Radeon 890M
CPUCache:   1024 KB
Keys:       16 bytes each
Values:     5000 bytes each (2500 bytes after compression)
Entries:    1000000
RawSize:    4783.6 MB (estimated)
FileSize:   2399.4 MB (estimated)
WARNING: Snappy compression is not enabled
------------------------------------------------
fillseq      :       9.187 micros/op;  520.7 MB/s     
fillsync     :    1394.107 micros/op;    3.4 MB/s (1000 ops)
fillrandom   :      14.321 micros/op;  334.0 MB/s     
overwrite    :      43.685 micros/op;  109.5 MB/s     
readrandom   :      11.576 micros/op; (864322 of 1000000 found)
readrandom   :       8.912 micros/op; (864083 of 1000000 found)
readunorderseq :       1.293 micros/op; 3699.4 MB/s  
readseq      :       7.223 micros/op;  662.3 MB/s    
readreverse  :       7.414 micros/op;  645.3 MB/s    
compact      :  159015.000 micros/op;
readrandom   :      29.700 micros/op; (864482 of 1000000 found)
readunorderseq :       1.204 micros/op; 3974.4 MB/s  
readseq      :       7.955 micros/op;  601.3 MB/s    
readreverse  :       7.085 micros/op;  675.2 MB/s    
fill100K     :      49.873 micros/op; 1912.5 MB/s (1000 ops)
crc32c       :       0.560 micros/op; 6972.8 MB/s (4K per op)
snappycomp   :     860.000 micros/op; (snappy failure)
snappyuncomp :    1290.000 micros/op; (snappy failure)
zstdcomp     :    1075.000 micros/op; (zstd failure)
zstduncomp   :     871.000 micros/op; (zstd failure)
```

options.mem_value_log_number=128

```
xxy@LAPTOP-LOMHUK6P:~/bench/XOY-leveldb-exp2/build$ ./db_bench
LevelDB:    version 1.23
Date:       Sat Jan  4 23:17:53 2025
CPU:        24 * AMD Ryzen AI 9 HX 370 w/ Radeon 890M
CPUCache:   1024 KB
Keys:       16 bytes each
Values:     5000 bytes each (2500 bytes after compression)
Entries:    1000000
RawSize:    4783.6 MB (estimated)
FileSize:   2399.4 MB (estimated)
WARNING: Snappy compression is not enabled
------------------------------------------------
fillseq      :       9.080 micros/op;  526.9 MB/s     
fillsync     :    1450.453 micros/op;    3.3 MB/s (1000 ops)
fillrandom   :      15.214 micros/op;  314.4 MB/s     
overwrite    :      43.780 micros/op;  109.3 MB/s     
readrandom   :       7.183 micros/op; (864322 of 1000000 found)
readrandom   :       5.007 micros/op; (864083 of 1000000 found)
readunorderseq :       1.322 micros/op; 3618.4 MB/s  
readseq      :       3.460 micros/op; 1382.4 MB/s    
readreverse  :       3.477 micros/op; 1375.7 MB/s    
compact      :  164147.000 micros/op;
readrandom   :      29.408 micros/op; (864482 of 1000000 found)
readunorderseq :       1.243 micros/op; 3848.2 MB/s  
readseq      :       0.959 micros/op; 4986.3 MB/s    
readreverse  :       1.015 micros/op; 4714.0 MB/s    
fill100K     :      47.614 micros/op; 2003.2 MB/s (1000 ops)
crc32c       :       0.568 micros/op; 6872.3 MB/s (4K per op)
snappycomp   :    1566.000 micros/op; (snappy failure)
snappyuncomp :     855.000 micros/op; (snappy failure)
zstdcomp     :     848.000 micros/op; (zstd failure)
zstduncomp   :     851.000 micros/op; (zstd failure)
```





# YCSB-Leveldb:V0

## 10fields*500field-len

workloada-1000000-10*500

```
  LD       ycsb
2025-01-05 15:21:57 0 sec: 0 operations;
2025-01-05 15:22:07 10 sec: 387047 operations; [INSERT: Count=387047 Max=19218.43 Min=4.14 Avg=20.17 90=9.13 99=1058.82 99.9=1184.77 99.99=1378.30]
2025-01-05 15:22:18 21 sec: 531270 operations; [INSERT: Count=531270 Max=541589.50 Min=4.14 Avg=33.85 90=9.46 99=1091.58 99.9=1216.51 99.99=1587.20]
2025-01-05 15:22:28 31 sec: 758899 operations; [INSERT: Count=758899 Max=541589.50 Min=4.05 Avg=35.18 90=9.11 99=1094.65 99.9=1217.54 99.99=1601.54]
2025-01-05 15:22:38 41 sec: 876919 operations; [INSERT: Count=876919 Max=541589.50 Min=4.05 Avg=41.07 90=9.10 99=1108.99 99.9=1232.89 99.99=1723.39]
2025-01-05 15:22:48 51 sec: 1000000 operations; [INSERT: Count=1000000 Max=541589.50 Min=4.05 Avg=45.72 90=9.22 99=1116.16 99.9=1240.06 99.99=2420.74]
Load runtime(sec): 51.4745
Load operations(ops): 1000000
Load throughput(ops/sec): 19427.1
2025-01-05 15:22:48 0 sec: 0 operations;
2025-01-05 15:22:58 10 sec: 398304 operations; [READ: Count=198696 Max=969.22 Min=0.57 Avg=8.87 90=15.71 99=33.76 99.9=76.99 99.99=139.39] [UPDATE: Count=199608 Max=12533.76 Min=4.98 Avg=39.61 90=27.41 99=1102.85 99.9=1236.99 99.99=1721.34]
2025-01-05 15:23:08 20 sec: 808664 operations; [READ: Count=404154 Max=10747.90 Min=0.57 Avg=9.20 90=16.61 99=35.30 99.9=79.36 99.99=157.69] [UPDATE: Count=404510 Max=22724.61 Min=4.98 Avg=38.76 90=28.70 99=1099.78 99.9=1232.89 99.99=1820.67]
2025-01-05 15:23:11 22 sec: 1000000 operations; [READ: Count=499823 Max=10747.90 Min=0.54 Avg=8.73 90=15.61 99=33.34 99.9=76.29 99.99=156.29] [UPDATE: Count=500177 Max=22724.61 Min=4.88 Avg=34.47 90=26.73 99=1090.56 99.9=1223.68 99.99=1739.78]
Run runtime(sec): 22.4951
Run operations(ops): 1000000
Run throughput(ops/sec): 44454.2
All steps completed successfully!
```

workloadb-1000000-10*500

```
  LD       ycsb
2025-01-05 15:24:32 0 sec: 0 operations;
2025-01-05 15:24:42 10 sec: 396743 operations; [INSERT: Count=396743 Max=17350.65 Min=4.31 Avg=19.59 90=9.34 99=1055.74 99.9=1165.31 99.99=1427.45]
2025-01-05 15:24:53 21 sec: 602651 operations; [INSERT: Count=602651 Max=530317.31 Min=4.31 Avg=29.29 90=9.12 99=1077.25 99.9=1192.96 99.99=1670.14]
2025-01-05 15:25:03 31 sec: 808638 operations; [INSERT: Count=808638 Max=530317.31 Min=4.31 Avg=32.71 90=9.21 99=1088.51 99.9=1205.25 99.99=1756.16]
2025-01-05 15:25:13 41 sec: 983428 operations; [INSERT: Count=983428 Max=530317.31 Min=4.31 Avg=36.02 90=9.35 99=1098.75 99.9=1216.51 99.99=1893.38]
2025-01-05 15:25:14 41 sec: 1000000 operations; [INSERT: Count=1000000 Max=530317.31 Min=4.08 Avg=35.54 90=9.33 99=1097.73 99.9=1215.49 99.99=1852.41]
Load runtime(sec): 41.294
Load operations(ops): 1000000
Load throughput(ops/sec): 24216.6
2025-01-05 15:25:14 0 sec: 0 operations;
2025-01-05 15:25:25 11 sec: 646218 operations; [READ: Count=613744 Max=547356.67 Min=0.52 Avg=8.96 90=12.94 99=35.45 99.9=77.44 99.99=143.87] [UPDATE: Count=32474 Max=23003.13 Min=5.17 Avg=156.86 90=1071.10 99=1207.30 99.9=1357.82 99.99=14540.80]
2025-01-05 15:25:27 13 sec: 1000000 operations; [READ: Count=949848 Max=547356.67 Min=0.52 Avg=7.82 90=11.11 99=30.88 99.9=68.22 99.99=130.62] [UPDATE: Count=50152 Max=23003.13 Min=5.17 Avg=109.12 90=51.90 99=1185.79 99.9=1314.82 99.99=8921.09]
Run runtime(sec): 13.6057
Run operations(ops): 1000000
Run throughput(ops/sec): 73498.6
All steps completed successfully!
```

workloadc-1000000-10*500

```
  LD       ycsb
2025-01-05 15:26:29 0 sec: 0 operations;
2025-01-05 15:26:39 10 sec: 398110 operations; [INSERT: Count=398110 Max=17973.25 Min=4.26 Avg=19.46 90=8.92 99=1053.69 99.9=1165.31 99.99=1503.23]
2025-01-05 15:26:49 20 sec: 606596 operations; [INSERT: Count=606596 Max=21774.33 Min=4.26 Avg=27.33 90=8.75 99=1076.22 99.9=1189.89 99.99=1587.20]
2025-01-05 15:27:00 31 sec: 720688 operations; [INSERT: Count=720688 Max=527695.87 Min=4.26 Avg=37.40 90=8.89 99=1096.70 99.9=1213.44 99.99=1763.33]
2025-01-05 15:27:10 41 sec: 838629 operations; [INSERT: Count=838629 Max=527695.87 Min=4.09 Avg=43.23 90=9.07 99=1110.02 99.9=1226.75 99.99=1904.64]
2025-01-05 15:27:18 49 sec: 1000000 operations; [INSERT: Count=1000000 Max=527695.87 Min=4.09 Avg=43.70 90=8.93 99=1110.02 99.9=1226.75 99.99=1901.57]
Load runtime(sec): 49.4375
Load operations(ops): 1000000
Load throughput(ops/sec): 20227.5
2025-01-05 15:27:18 0 sec: 0 operations;
2025-01-05 15:27:27 8 sec: 1000000 operations; [READ: Count=1000000 Max=536346.62 Min=1.19 Avg=8.15 90=10.68 99=19.42 99.9=35.36 99.99=104.83]
Run runtime(sec): 8.81349
Run operations(ops): 1000000
Run throughput(ops/sec): 113462
All steps completed successfully!
```

workloadd-1000000-10*500

```
  LD       ycsb
2025-01-05 15:28:06 0 sec: 0 operations;
2025-01-05 15:28:16 10 sec: 382540 operations; [INSERT: Count=382540 Max=49807.36 Min=3.78 Avg=20.43 90=8.78 99=1056.77 99.9=1172.48 99.99=1451.01]
2025-01-05 15:28:27 21 sec: 541726 operations; [INSERT: Count=541726 Max=535298.05 Min=3.78 Avg=33.09 90=8.65 99=1087.49 99.9=1204.22 99.99=1589.25]
2025-01-05 15:28:37 31 sec: 728142 operations; [INSERT: Count=728142 Max=535298.05 Min=3.78 Avg=36.87 90=8.65 99=1097.73 99.9=1217.54 99.99=1637.38]
2025-01-05 15:28:47 41 sec: 888919 operations; [INSERT: Count=888919 Max=535298.05 Min=3.78 Avg=40.42 90=8.72 99=1103.87 99.9=1224.70 99.99=1694.72]
2025-01-05 15:28:53 46 sec: 1000000 operations; [INSERT: Count=1000000 Max=535298.05 Min=3.78 Avg=40.78 90=8.68 99=1104.89 99.9=1223.68 99.99=1694.72]
Load runtime(sec): 46.5766
Load operations(ops): 1000000
Load throughput(ops/sec): 21470
2025-01-05 15:28:53 0 sec: 0 operations;
2025-01-05 15:29:04 11 sec: 741818 operations; [INSERT: Count=37056 Max=521928.70 Min=4.58 Avg=197.28 90=1067.01 99=1185.79 99.9=1368.06 99.99=18202.62] [READ: Count=704762 Max=17612.80 Min=0.50 Avg=4.37 90=9.11 99=27.49 99.9=61.47 99.99=115.07]
2025-01-05 15:29:05 12 sec: 1000000 operations; [INSERT: Count=49902 Max=521928.70 Min=4.58 Avg=150.53 90=1055.74 99=1170.43 99.9=1337.34 99.99=14139.39] [READ: Count=950098 Max=17612.80 Min=0.47 Avg=4.19 90=8.82 99=24.82 99.9=56.32 99.99=108.48]
Run runtime(sec): 12.361
Run operations(ops): 1000000
Run throughput(ops/sec): 80899.7
All steps completed successfully!
```



workloade-recordcount=100000-operationcount=10000-10*500

```
  LD       ycsb
2025-01-05 15:29:47 0 sec: 0 operations;
2025-01-05 15:29:48 1 sec: 100000 operations; [INSERT: Count=100000 Max=2177.02 Min=4.22 Avg=6.74 90=7.83 99=17.98 99.9=37.38 99.99=208.00]
Load runtime(sec): 1.22978
Load operations(ops): 100000
Load throughput(ops/sec): 81315.4
2025-01-05 15:29:48 0 sec: 0 operations;
2025-01-05 15:29:54 5 sec: 10000 operations; [INSERT: Count=477 Max=125.82 Min=5.77 Avg=46.92 90=75.26 99=100.42 99.9=125.82 99.99=125.82] [SCAN: Count=9523 Max=19431.42 Min=3.77 Avg=515.87 90=907.26 99=1196.03 99.9=1736.70 99.99=7577.60]
Run runtime(sec): 5.47417
Run operations(ops): 10000
Run throughput(ops/sec): 1826.76
All steps completed successfully!
```



workloadf-1000000-10*500

```
  LD       ycsb
2025-01-05 15:30:43 0 sec: 0 operations;
2025-01-05 15:30:53 10 sec: 349919 operations; [INSERT: Count=349919 Max=43253.76 Min=4.18 Avg=23.13 90=7.70 99=1067.01 99.9=1179.65 99.99=1614.85]
2025-01-05 15:31:04 21 sec: 580094 operations; [INSERT: Count=580094 Max=523763.71 Min=4.18 Avg=30.73 90=7.98 99=1081.34 99.9=1203.20 99.99=1715.20]
2025-01-05 15:31:14 31 sec: 695620 operations; [INSERT: Count=695620 Max=523763.71 Min=4.18 Avg=39.02 90=8.21 99=1102.85 99.9=1223.68 99.99=1909.76]
2025-01-05 15:31:24 41 sec: 814040 operations; [INSERT: Count=814040 Max=523763.71 Min=4.18 Avg=44.75 90=8.51 99=1111.04 99.9=1233.92 99.99=3088.38]
2025-01-05 15:31:35 52 sec: 990902 operations; [INSERT: Count=990902 Max=531103.74 Min=4.18 Avg=46.83 90=8.62 99=1112.06 99.9=1232.89 99.99=3088.38]
2025-01-05 15:31:35 52 sec: 1000000 operations; [INSERT: Count=1000000 Max=531103.74 Min=4.18 Avg=46.46 90=8.59 99=1112.06 99.9=1232.89 99.99=3088.38]
Load runtime(sec): 52.2434
Load operations(ops): 1000000
Load throughput(ops/sec): 19141.2
2025-01-05 15:31:35 0 sec: 0 operations;
2025-01-05 15:31:45 10 sec: 372767 operations; [READ: Count=248572 Max=1964.03 Min=0.54 Avg=10.24 90=18.25 99=40.86 99.9=92.99 99.99=162.69] [UPDATE: Count=124195 Max=24395.78 Min=4.90 Avg=56.63 90=23.97 99=1123.33 99.9=1243.13 99.99=2099.20]
2025-01-05 15:31:55 20 sec: 948926 operations; [READ: Count=632678 Max=3565.57 Min=0.49 Avg=9.49 90=16.96 99=35.87 99.9=82.75 99.99=161.92] [UPDATE: Count=316248 Max=24395.78 Min=4.74 Avg=42.05 90=17.70 99=1096.70 99.9=1215.49 99.99=2240.51]
2025-01-05 15:32:06 31 sec: 1394875 operations; [READ: Count=929797 Max=529268.73 Min=0.49 Avg=10.47 90=16.73 99=36.03 99.9=83.33 99.99=165.38] [UPDATE: Count=465078 Max=24395.78 Min=4.74 Avg=43.79 90=17.89 99=1100.80 99.9=1224.70 99.99=2902.01]
2025-01-05 15:32:07 32 sec: 1499897 operations; [READ: Count=1000000 Max=529268.73 Min=0.49 Avg=10.37 90=16.62 99=35.33 99.9=81.92 99.99=164.35] [UPDATE: Count=499897 Max=24395.78 Min=4.74 Avg=41.51 90=17.57 99=1096.70 99.9=1219.58 99.99=2377.73]
Run runtime(sec): 32.2646
Run operations(ops): 1000000
Run throughput(ops/sec): 30993.7
All steps completed successfully!
```





## 10fields*100field-len

workloada-1000000-10*100

```
  LD       ycsb
2025-01-05 15:52:53 0 sec: 0 operations;
2025-01-05 15:52:59 6 sec: 1000000 operations; [INSERT: Count=1000000 Max=11919.36 Min=1.35 Avg=4.83 90=3.84 99=22.51 99.9=1082.37 99.99=1189.89]
Load runtime(sec): 6.34656
Load operations(ops): 1000000
Load throughput(ops/sec): 157566
2025-01-05 15:52:59 0 sec: 0 operations;
2025-01-05 15:53:06 6 sec: 1000000 operations; [READ: Count=499746 Max=6340.61 Min=0.46 Avg=4.85 90=8.81 99=14.58 99.9=32.29 99.99=71.36] [UPDATE: Count=500254 Max=10297.34 Min=2.01 Avg=7.59 90=11.54 99=25.21 99.9=50.62 99.99=142.85]
Run runtime(sec): 6.75589
Run operations(ops): 1000000
Run throughput(ops/sec): 148019
All steps completed successfully!
```

workloadb-1000000-10*100

```
  LD       ycsb
2025-01-05 15:54:45 0 sec: 0 operations;
2025-01-05 15:54:52 7 sec: 1000000 operations; [INSERT: Count=1000000 Max=509870.08 Min=1.33 Avg=5.19 90=3.69 99=19.77 99.9=1075.20 99.99=1203.20]
Load runtime(sec): 7.06858
Load operations(ops): 1000000
Load throughput(ops/sec): 141471
2025-01-05 15:54:52 0 sec: 0 operations;
2025-01-05 15:54:57 4 sec: 1000000 operations; [READ: Count=950016 Max=8560.64 Min=0.45 Avg=3.86 90=6.78 99=12.86 99.9=28.38 99.99=70.59] [UPDATE: Count=49984 Max=11845.63 Min=2.13 Avg=13.65 90=30.09 99=44.45 99.9=131.71 99.99=3540.99]
Run runtime(sec): 4.9661
Run operations(ops): 1000000
Run throughput(ops/sec): 201365
All steps completed successfully!
```

workloadc-1000000-10*100

```
  LD       ycsb
2025-01-05 15:56:33 0 sec: 0 operations;
2025-01-05 15:56:39 6 sec: 1000000 operations; [INSERT: Count=1000000 Max=11763.71 Min=1.35 Avg=4.62 90=3.59 99=19.31 99.9=1071.10 99.99=1172.48]
Load runtime(sec): 6.05356
Load operations(ops): 1000000
Load throughput(ops/sec): 165192
2025-01-05 15:56:39 0 sec: 0 operations;
2025-01-05 15:56:43 4 sec: 1000000 operations; [READ: Count=1000000 Max=7897.09 Min=0.77 Avg=3.69 90=5.85 99=10.73 99.9=24.24 99.99=64.06]
Run runtime(sec): 4.09051
Run operations(ops): 1000000
Run throughput(ops/sec): 244469
All steps completed successfully!
```

workloadd-1000000-10*100

```
  LD       ycsb
2025-01-05 15:57:33 0 sec: 0 operations;
2025-01-05 15:57:39 5 sec: 1000000 operations; [INSERT: Count=1000000 Max=11927.55 Min=1.34 Avg=4.43 90=3.50 99=8.25 99.9=1073.15 99.99=1186.82]
Load runtime(sec): 5.87062
Load operations(ops): 1000000
Load throughput(ops/sec): 170340
2025-01-05 15:57:39 0 sec: 0 operations;
2025-01-05 15:57:42 2 sec: 1000000 operations; [INSERT: Count=49658 Max=9773.06 Min=1.54 Avg=7.00 90=21.65 99=36.00 99.9=67.84 99.99=252.29] [READ: Count=950342 Max=5664.77 Min=0.42 Avg=2.20 90=5.67 99=11.56 99.9=23.39 99.99=55.36]
Run runtime(sec): 2.92777
Run operations(ops): 1000000
Run throughput(ops/sec): 341556
All steps completed successfully!
```

workloade-recordcount=100000-operationcount=10000-10*100

```
  LD       ycsb
2025-01-05 15:58:47 0 sec: 0 operations;
2025-01-05 15:58:47 0 sec: 100000 operations; [INSERT: Count=100000 Max=1615.87 Min=1.34 Avg=2.85 90=3.74 99=9.41 99.9=24.88 99.99=93.38]
Load runtime(sec): 0.449435
Load operations(ops): 100000
Load throughput(ops/sec): 222502
2025-01-05 15:58:47 0 sec: 0 operations;
2025-01-05 15:58:49 2 sec: 10000 operations; [INSERT: Count=508 Max=107.33 Min=3.62 Avg=48.62 90=69.57 99=92.61 99.9=107.14 99.99=107.33] [SCAN: Count=9492 Max=660.48 Min=5.70 Avg=180.07 90=308.22 99=355.84 99.9=405.25 99.99=644.61]
Run runtime(sec): 2.10515
Run operations(ops): 10000
Run throughput(ops/sec): 4750.26
All steps completed successfully!
```

workloadf-1000000-10*100

```
  LD       ycsb
2025-01-05 15:59:12 0 sec: 0 operations;
2025-01-05 15:59:18 6 sec: 1000000 operations; [INSERT: Count=1000000 Max=16072.70 Min=1.35 Avg=4.85 90=3.72 99=19.47 99.9=1071.10 99.99=1186.82]
Load runtime(sec): 6.3055
Load operations(ops): 1000000
Load throughput(ops/sec): 158592
2025-01-05 15:59:18 0 sec: 0 operations;
2025-01-05 15:59:26 8 sec: 1500583 operations; [READ: Count=1000000 Max=6959.10 Min=0.43 Avg=4.36 90=7.85 99=12.53 99.9=26.51 99.99=74.88] [UPDATE: Count=500583 Max=724041.73 Min=2.00 Avg=6.76 90=5.44 99=14.01 99.9=44.41 99.99=124.16]
Run runtime(sec): 8.30236
Run operations(ops): 1000000
Run throughput(ops/sec): 120448
All steps completed successfully!
```



## 20fields*1000field-len

workloada-1000000-20*1000

```
  LD       ycsb
2025-01-05 15:59:59 0 sec: 0 operations;
2025-01-05 16:00:09 10 sec: 114605 operations; [INSERT: Count=114605 Max=63504.38 Min=11.38 Avg=65.40 90=40.45 99=1150.97 99.9=1295.36 99.99=7274.49]
2025-01-05 16:00:19 20 sec: 148212 operations; [INSERT: Count=148212 Max=63504.38 Min=11.38 Avg=112.49 90=60.41 99=1188.86 99.9=1353.73 99.99=8151.04]
2025-01-05 16:00:30 30 sec: 208343 operations; [INSERT: Count=208343 Max=403701.76 Min=11.38 Avg=123.30 90=68.16 99=1191.93 99.9=1373.18 99.99=8495.10]
2025-01-05 16:00:40 40 sec: 243351 operations; [INSERT: Count=243351 Max=403701.76 Min=11.38 Avg=143.21 90=1070.08 99=1202.17 99.9=1394.69 99.99=10141.69]
2025-01-05 16:00:50 50 sec: 279328 operations; [INSERT: Count=279328 Max=403701.76 Min=11.38 Avg=157.48 90=1078.27 99=1208.32 99.9=1416.19 99.99=10141.69]
2025-01-05 16:01:01 61 sec: 336350 operations; [INSERT: Count=336350 Max=499384.32 Min=11.38 Avg=159.25 90=1077.25 99=1208.32 99.9=1410.05 99.99=10559.49]
2025-01-05 16:01:11 71 sec: 369130 operations; [INSERT: Count=369130 Max=499384.32 Min=11.38 Avg=170.03 90=1084.41 99=1212.41 99.9=1426.43 99.99=11124.74]
2025-01-05 16:01:21 81 sec: 427164 operations; [INSERT: Count=427164 Max=499384.32 Min=11.38 Avg=167.17 90=1082.37 99=1211.39 99.9=1432.58 99.99=11247.61]
2025-01-05 16:01:32 92 sec: 460728 operations; [INSERT: Count=460728 Max=499384.32 Min=11.38 Avg=176.91 90=1086.46 99=1214.46 99.9=1438.72 99.99=11689.98]
2025-01-05 16:01:42 102 sec: 495035 operations; [INSERT: Count=495035 Max=499384.32 Min=11.38 Avg=183.19 90=1088.51 99=1217.54 99.9=1457.15 99.99=11247.61]
2025-01-05 16:01:52 112 sec: 529834 operations; [INSERT: Count=529834 Max=499384.32 Min=11.38 Avg=188.46 90=1090.56 99=1219.58 99.9=1462.27 99.99=11247.61]
2025-01-05 16:02:03 123 sec: 563005 operations; [INSERT: Count=563005 Max=499384.32 Min=11.38 Avg=195.32 90=1091.58 99=1221.63 99.9=1475.58 99.99=11247.61]
2025-01-05 16:02:13 133 sec: 594816 operations; [INSERT: Count=594816 Max=499384.32 Min=11.38 Avg=200.29 90=1093.63 99=1223.68 99.9=1482.75 99.99=11247.61]
2025-01-05 16:02:23 143 sec: 627620 operations; [INSERT: Count=627620 Max=499384.32 Min=11.22 Avg=204.46 90=1094.65 99=1225.73 99.9=1485.82 99.99=11247.61]
2025-01-05 16:02:33 154 sec: 662796 operations; [INSERT: Count=662796 Max=499384.32 Min=11.22 Avg=208.02 90=1094.65 99=1227.78 99.9=1491.97 99.99=11649.02]
2025-01-05 16:02:43 164 sec: 686409 operations; [INSERT: Count=686409 Max=499384.32 Min=11.22 Avg=214.57 90=1096.70 99=1229.82 99.9=1501.18 99.99=11689.98]
2025-01-05 16:02:53 174 sec: 705880 operations; [INSERT: Count=705880 Max=499384.32 Min=11.22 Avg=222.05 90=1098.75 99=1231.87 99.9=1515.52 99.99=11919.36]
2025-01-05 16:03:04 184 sec: 741306 operations; [INSERT: Count=741306 Max=866123.78 Min=11.22 Avg=224.91 90=1098.75 99=1232.89 99.9=1519.62 99.99=12427.26]
2025-01-05 16:03:15 195 sec: 775745 operations; [INSERT: Count=775745 Max=866123.78 Min=11.22 Avg=227.68 90=1099.78 99=1233.92 99.9=1523.71 99.99=12320.77]
2025-01-05 16:03:25 205 sec: 808423 operations; [INSERT: Count=808423 Max=866123.78 Min=11.22 Avg=229.83 90=1099.78 99=1233.92 99.9=1525.76 99.99=12058.62]
2025-01-05 16:03:36 216 sec: 830244 operations; [INSERT: Count=830244 Max=866123.78 Min=11.22 Avg=235.90 90=1101.82 99=1235.97 99.9=1531.90 99.99=12451.84]
2025-01-05 16:03:46 226 sec: 851331 operations; [INSERT: Count=851331 Max=866123.78 Min=11.22 Avg=241.15 90=1102.85 99=1238.02 99.9=1546.24 99.99=12320.77]
2025-01-05 16:03:56 236 sec: 886430 operations; [INSERT: Count=886430 Max=866123.78 Min=11.22 Avg=241.91 90=1102.85 99=1239.04 99.9=1546.24 99.99=12320.77]
2025-01-05 16:04:06 247 sec: 920445 operations; [INSERT: Count=920445 Max=866123.78 Min=11.22 Avg=243.84 90=1102.85 99=1240.06 99.9=1546.24 99.99=12427.26]
2025-01-05 16:04:16 257 sec: 954142 operations; [INSERT: Count=954142 Max=866123.78 Min=11.22 Avg=244.83 90=1103.87 99=1240.06 99.9=1554.43 99.99=12214.27]
2025-01-05 16:04:26 267 sec: 962618 operations; [INSERT: Count=962618 Max=866123.78 Min=11.22 Avg=252.76 90=1104.89 99=1243.13 99.9=1570.82 99.99=12451.84]
2025-01-05 16:04:37 278 sec: 994253 operations; [INSERT: Count=994253 Max=866123.78 Min=11.22 Avg=254.90 90=1105.92 99=1243.13 99.9=1572.86 99.99=12451.84]
2025-01-05 16:04:44 284 sec: 1000000 operations; [INSERT: Count=1000000 Max=866123.78 Min=11.22 Avg=259.63 90=1106.94 99=1245.18 99.9=1587.20 99.99=12451.84]
Load runtime(sec): 284.448
Load operations(ops): 1000000
Load throughput(ops/sec): 3515.58
2025-01-05 16:04:44 0 sec: 0 operations;
2025-01-05 16:04:54 10 sec: 56589 operations; [READ: Count=28226 Max=4689.92 Min=1.91 Avg=27.17 90=50.17 99=137.34 99.9=322.56 99.99=897.02] [UPDATE: Count=28363 Max=21430.27 Min=13.97 Avg=320.03 90=1176.58 99=1386.49 99.9=1864.70 99.99=11935.74]
2025-01-05 16:05:04 20 sec: 104740 operations; [READ: Count=52246 Max=15114.24 Min=1.78 Avg=29.02 90=52.09 99=178.69 99.9=537.09 99.99=1468.41] [UPDATE: Count=52494 Max=21430.27 Min=13.97 Avg=347.30 90=1188.86 99=1413.12 99.9=1780.73 99.99=11214.85]
2025-01-05 16:05:15 30 sec: 136540 operations; [READ: Count=68253 Max=15114.24 Min=1.78 Avg=30.35 90=54.91 99=188.41 99.9=635.39 99.99=1994.75] [UPDATE: Count=68287 Max=400818.17 Min=13.97 Avg=410.04 90=1201.15 99=1430.53 99.9=1991.68 99.99=11296.77]
2025-01-05 16:05:25 40 sec: 198379 operations; [READ: Count=99182 Max=15114.24 Min=1.78 Avg=31.00 90=52.83 99=225.66 99.9=783.87 99.99=3411.97] [UPDATE: Count=99197 Max=400818.17 Min=13.97 Avg=371.83 90=1186.82 99=1413.12 99.9=2115.58 99.99=11214.85]
2025-01-05 16:05:35 50 sec: 231885 operations; [READ: Count=115882 Max=15114.24 Min=1.78 Avg=32.19 90=54.27 99=266.75 99.9=842.24 99.99=3491.84] [UPDATE: Count=116003 Max=400818.17 Min=13.97 Avg=397.82 90=1189.89 99=1418.24 99.9=2418.69 99.99=11214.85]
2025-01-05 16:05:46 61 sec: 279192 operations; [READ: Count=139557 Max=15114.24 Min=1.78 Avg=33.84 90=54.78 99=347.13 99.9=912.89 99.99=4157.44] [UPDATE: Count=139635 Max=488636.41 Min=13.97 Avg=400.62 90=1186.82 99=1431.55 99.9=2729.98 99.99=12779.52]
2025-01-05 16:05:56 71 sec: 341791 operations; [READ: Count=170886 Max=15114.24 Min=1.52 Avg=34.98 90=54.24 99=403.97 99.9=992.77 99.99=4190.21] [UPDATE: Count=170905 Max=488636.41 Min=13.66 Avg=377.81 90=1177.60 99=1428.48 99.9=2897.92 99.99=12779.52]
2025-01-05 16:06:06 81 sec: 404249 operations; [READ: Count=202126 Max=15114.24 Min=1.52 Avg=36.82 90=54.69 99=447.74 99.9=1055.74 99.99=4251.65] [UPDATE: Count=202123 Max=488636.41 Min=13.66 Avg=361.07 90=1170.43 99=1431.55 99.9=3065.86 99.99=13066.24]
2025-01-05 16:06:16 92 sec: 467049 operations; [READ: Count=233431 Max=15114.24 Min=1.41 Avg=38.63 90=55.07 99=476.93 99.9=1197.06 99.99=4509.69] [UPDATE: Count=233618 Max=488636.41 Min=13.66 Avg=351.86 90=1164.29 99=1435.65 99.9=3368.96 99.99=13778.94]
2025-01-05 16:06:26 102 sec: 530904 operations; [READ: Count=265474 Max=15114.24 Min=1.41 Avg=39.54 90=55.04 99=492.03 99.9=1282.05 99.99=4595.71] [UPDATE: Count=265430 Max=488636.41 Min=13.66 Avg=341.35 90=1160.19 99=1436.67 99.9=3409.92 99.99=13262.85]
2025-01-05 16:06:36 112 sec: 586189 operations; [READ: Count=292973 Max=15114.24 Min=1.41 Avg=40.93 90=55.71 99=511.23 99.9=1343.49 99.99=4673.53] [UPDATE: Count=293216 Max=488636.41 Min=13.66 Avg=337.63 90=1157.12 99=1448.96 99.9=3495.93 99.99=14221.31]
2025-01-05 16:06:47 123 sec: 648261 operations; [READ: Count=323957 Max=15114.24 Min=1.41 Avg=41.76 90=55.71 99=525.82 99.9=1380.35 99.99=4677.63] [UPDATE: Count=324304 Max=497549.31 Min=13.66 Avg=333.78 90=1154.05 99=1453.06 99.9=3962.88 99.99=16121.85]
2025-01-05 16:06:57 133 sec: 709529 operations; [READ: Count=354558 Max=16711.68 Min=1.41 Avg=43.21 90=56.26 99=539.65 99.9=1633.28 99.99=4739.07] [UPDATE: Count=354971 Max=497549.31 Min=13.60 Avg=327.74 90=1150.97 99=1457.15 99.9=4104.19 99.99=14458.88]
2025-01-05 16:07:07 143 sec: 769996 operations; [READ: Count=384836 Max=16711.68 Min=1.41 Avg=44.37 90=56.86 99=550.40 99.9=1816.58 99.99=4825.09] [UPDATE: Count=385160 Max=497549.31 Min=13.60 Avg=323.15 90=1148.93 99=1461.25 99.9=4126.72 99.99=14221.31]
2025-01-05 16:07:18 153 sec: 832042 operations; [READ: Count=415800 Max=16711.68 Min=1.41 Avg=45.07 90=57.09 99=557.05 99.9=1923.07 99.99=4845.57] [UPDATE: Count=416242 Max=497549.31 Min=13.60 Avg=319.71 90=1146.88 99=1467.39 99.9=4147.20 99.99=14737.41]
2025-01-05 16:07:28 163 sec: 890584 operations; [READ: Count=445049 Max=16711.68 Min=1.41 Avg=45.96 90=57.47 99=566.27 99.9=2123.78 99.99=4857.85] [UPDATE: Count=445535 Max=497549.31 Min=13.60 Avg=317.03 90=1144.83 99=1471.49 99.9=4147.20 99.99=14737.41]
2025-01-05 16:07:38 173 sec: 950788 operations; [READ: Count=475213 Max=18104.32 Min=1.41 Avg=46.79 90=57.53 99=572.93 99.9=2359.30 99.99=4886.53] [UPDATE: Count=475575 Max=497549.31 Min=13.60 Avg=314.04 90=1144.83 99=1474.56 99.9=4214.78 99.99=14737.41]
2025-01-05 16:07:47 183 sec: 1000000 operations; [READ: Count=499872 Max=18104.32 Min=1.41 Avg=48.82 90=57.34 99=598.53 99.9=2715.65 99.99=4931.58] [UPDATE: Count=500128 Max=762314.75 Min=13.60 Avg=311.47 90=1143.81 99=1496.06 99.9=4214.78 99.99=15319.04]
Run runtime(sec): 183.076
Run operations(ops): 1000000
Run throughput(ops/sec): 5462.22
All steps completed successfully!
```

workloadb-1000000-20*1000

```
  LD       ycsb
2025-01-05 16:08:54 0 sec: 0 operations;
2025-01-05 16:09:04 10 sec: 106648 operations; [INSERT: Count=106648 Max=44466.18 Min=13.10 Avg=70.16 90=47.13 99=1180.67 99.9=1308.67 99.99=7757.82]
2025-01-05 16:09:15 20 sec: 147135 operations; [INSERT: Count=147135 Max=489947.14 Min=13.10 Avg=117.91 90=64.83 99=1219.58 99.9=1395.71 99.99=9633.79]
2025-01-05 16:09:25 30 sec: 204469 operations; [INSERT: Count=204469 Max=489947.14 Min=12.62 Avg=127.01 90=79.42 99=1220.61 99.9=1412.10 99.99=9633.79]
2025-01-05 16:09:35 40 sec: 238367 operations; [INSERT: Count=238367 Max=489947.14 Min=12.62 Avg=147.39 90=1056.77 99=1221.63 99.9=1421.31 99.99=9969.66]
2025-01-05 16:09:46 51 sec: 270455 operations; [INSERT: Count=270455 Max=491257.85 Min=12.42 Avg=167.37 90=1082.37 99=1227.78 99.9=1442.82 99.99=11476.99]
2025-01-05 16:09:56 61 sec: 331046 operations; [INSERT: Count=331046 Max=491257.85 Min=12.42 Avg=162.60 90=1079.30 99=1224.70 99.9=1434.62 99.99=11476.99]
2025-01-05 16:10:06 71 sec: 364925 operations; [INSERT: Count=364925 Max=491257.85 Min=12.42 Avg=172.70 90=1086.46 99=1226.75 99.9=1460.22 99.99=12001.28]
2025-01-05 16:10:17 82 sec: 399130 operations; [INSERT: Count=399130 Max=491257.85 Min=12.42 Avg=183.01 90=1090.56 99=1228.80 99.9=1466.37 99.99=12115.97]
2025-01-05 16:10:27 92 sec: 433525 operations; [INSERT: Count=433525 Max=491257.85 Min=12.42 Avg=189.65 90=1093.63 99=1230.85 99.9=1471.49 99.99=12468.22]
2025-01-05 16:10:37 102 sec: 467400 operations; [INSERT: Count=467400 Max=491257.85 Min=12.25 Avg=195.54 90=1094.65 99=1231.87 99.9=1482.75 99.99=12115.97]
2025-01-05 16:10:48 113 sec: 501676 operations; [INSERT: Count=501676 Max=491257.85 Min=12.13 Avg=202.18 90=1096.70 99=1232.89 99.9=1486.85 99.99=12468.22]
2025-01-05 16:10:58 123 sec: 536346 operations; [INSERT: Count=536346 Max=491257.85 Min=12.13 Avg=206.21 90=1097.73 99=1233.92 99.9=1501.18 99.99=12115.97]
2025-01-05 16:11:08 133 sec: 569996 operations; [INSERT: Count=569996 Max=491257.85 Min=12.13 Avg=210.17 90=1098.75 99=1235.97 99.9=1506.30 99.99=12558.33]
2025-01-05 16:11:19 144 sec: 604511 operations; [INSERT: Count=604511 Max=491257.85 Min=12.13 Avg=214.83 90=1099.78 99=1238.02 99.9=1521.66 99.99=12697.60]
2025-01-05 16:11:29 154 sec: 638544 operations; [INSERT: Count=638544 Max=491257.85 Min=12.13 Avg=217.71 90=1101.82 99=1240.06 99.9=1526.78 99.99=12558.33]
2025-01-05 16:11:39 164 sec: 647056 operations; [INSERT: Count=647056 Max=491257.85 Min=12.13 Avg=229.87 90=1104.89 99=1244.16 99.9=1551.36 99.99=12607.49]
2025-01-05 16:11:50 175 sec: 681775 operations; [INSERT: Count=681775 Max=491257.85 Min=12.13 Avg=232.85 90=1104.89 99=1244.16 99.9=1554.43 99.99=12713.98]
2025-01-05 16:12:00 185 sec: 715849 operations; [INSERT: Count=715849 Max=491257.85 Min=12.13 Avg=234.58 90=1105.92 99=1244.16 99.9=1556.48 99.99=12607.49]
2025-01-05 16:12:10 195 sec: 750472 operations; [INSERT: Count=750472 Max=491257.85 Min=12.13 Avg=235.97 90=1105.92 99=1245.18 99.9=1566.72 99.99=12558.33]
2025-01-05 16:12:21 206 sec: 759020 operations; [INSERT: Count=759020 Max=491257.85 Min=12.13 Avg=247.31 90=1107.97 99=1248.26 99.9=1587.20 99.99=12648.45]
2025-01-05 16:12:31 216 sec: 790841 operations; [INSERT: Count=790841 Max=491257.85 Min=12.13 Avg=249.04 90=1107.97 99=1247.23 99.9=1580.03 99.99=12607.49]
2025-01-05 16:12:41 226 sec: 798116 operations; [INSERT: Count=798116 Max=491257.85 Min=12.13 Avg=257.13 90=1110.02 99=1249.28 99.9=1588.22 99.99=12648.45]
2025-01-05 16:12:51 237 sec: 827562 operations; [INSERT: Count=827562 Max=5096079.36 Min=11.32 Avg=262.13 90=1108.99 99=1248.26 99.9=1586.17 99.99=12713.98]
2025-01-05 16:13:01 247 sec: 836158 operations; [INSERT: Count=836158 Max=5096079.36 Min=11.32 Avg=271.08 90=1111.04 99=1250.30 99.9=1592.32 99.99=12713.98]
2025-01-05 16:13:11 257 sec: 865916 operations; [INSERT: Count=865916 Max=5096079.36 Min=11.32 Avg=272.53 90=1110.02 99=1249.28 99.9=1587.20 99.99=12697.60]
2025-01-05 16:13:22 267 sec: 874505 operations; [INSERT: Count=874505 Max=5096079.36 Min=11.32 Avg=281.99 90=1112.06 99=1251.33 99.9=1592.32 99.99=12771.33]
2025-01-05 16:13:32 277 sec: 877268 operations; [INSERT: Count=877268 Max=5096079.36 Min=11.32 Avg=284.66 90=1113.09 99=1252.35 99.9=1595.39 99.99=12713.98]
2025-01-05 16:13:42 287 sec: 897056 operations; [INSERT: Count=897056 Max=15099494.40 Min=11.32 Avg=296.54 90=1112.06 99=1250.30 99.9=1588.22 99.99=13123.58]
2025-01-05 16:13:53 298 sec: 911257 operations; [INSERT: Count=911257 Max=15099494.40 Min=11.32 Avg=303.00 90=1113.09 99=1252.35 99.9=1592.32 99.99=13262.85]
2025-01-05 16:14:03 308 sec: 916844 operations; [INSERT: Count=916844 Max=15099494.40 Min=11.32 Avg=308.03 90=1114.11 99=1252.35 99.9=1597.44 99.99=13148.16]
2025-01-05 16:14:13 318 sec: 947805 operations; [INSERT: Count=947805 Max=15099494.40 Min=11.32 Avg=311.48 90=1113.09 99=1252.35 99.9=1588.22 99.99=13500.42]
2025-01-05 16:14:24 329 sec: 956399 operations; [INSERT: Count=956399 Max=15099494.40 Min=11.32 Avg=319.79 90=1115.13 99=1253.38 99.9=1591.30 99.99=13606.91]
2025-01-05 16:14:34 339 sec: 956420 operations; [INSERT: Count=956420 Max=15099494.40 Min=11.32 Avg=319.81 90=1115.13 99=1253.38 99.9=1591.30 99.99=13606.91]
2025-01-05 16:14:44 349 sec: 964593 operations; [INSERT: Count=964593 Max=19461570.56 Min=11.32 Avg=337.62 90=1114.11 99=1253.38 99.9=1588.22 99.99=13664.25]
2025-01-05 16:14:55 360 sec: 990052 operations; [INSERT: Count=990052 Max=19461570.56 Min=11.32 Avg=339.30 90=1115.13 99=1253.38 99.9=1588.22 99.99=14041.09]
2025-01-05 16:15:05 370 sec: 995996 operations; [INSERT: Count=995996 Max=19461570.56 Min=11.32 Avg=344.02 90=1116.16 99=1254.40 99.9=1590.27 99.99=14041.09]
2025-01-05 16:15:15 380 sec: 995996 operations; [INSERT: Count=995996 Max=19461570.56 Min=11.32 Avg=344.02 90=1116.16 99=1254.40 99.9=1590.27 99.99=14041.09]
2025-01-05 16:15:20 385 sec: 1000000 operations; [INSERT: Count=1000000 Max=19461570.56 Min=11.32 Avg=360.74 90=1115.13 99=1254.40 99.9=1590.27 99.99=14180.35]
Load runtime(sec): 385.924
Load operations(ops): 1000000
Load throughput(ops/sec): 2591.19
2025-01-05 16:15:20 0 sec: 0 operations;
2025-01-05 16:15:31 10 sec: 402504 operations; [READ: Count=382629 Max=405536.77 Min=1.29 Avg=16.08 90=24.11 99=52.26 99.9=106.05 99.99=184.45] [UPDATE: Count=19875 Max=468451.33 Min=14.45 Avg=206.09 90=1093.63 99=1233.92 99.9=1353.73 99.99=6799.36]
2025-01-05 16:15:41 20 sec: 533850 operations; [READ: Count=507489 Max=405536.77 Min=1.29 Avg=16.89 90=27.28 99=59.10 99.9=115.90 99.99=194.69] [UPDATE: Count=26361 Max=468451.33 Min=14.45 Avg=438.75 90=1145.86 99=1294.34 99.9=1553.41 99.99=11165.69]
2025-01-05 16:15:51 30 sec: 661416 operations; [READ: Count=628665 Max=405536.77 Min=1.29 Avg=17.68 90=29.89 99=68.09 99.9=128.90 99.99=220.29] [UPDATE: Count=32751 Max=468451.33 Min=14.45 Avg=577.41 90=1181.69 99=1310.72 99.9=1589.25 99.99=12017.66]
2025-01-05 16:16:02 41 sec: 1000000 operations; [READ: Count=950279 Max=405536.77 Min=1.29 Avg=16.04 90=26.59 99=64.58 99.9=121.73 99.99=213.50] [UPDATE: Count=49721 Max=5091885.05 Min=14.45 Avg=505.53 90=1141.76 99=1290.24 99.9=1508.35 99.99=11165.69]
2025-01-05 16:16:02 41 sec: 1000000 operations; [READ: Count=950279 Max=405536.77 Min=1.29 Avg=16.04 90=26.59 99=64.58 99.9=121.73 99.99=213.50] [UPDATE: Count=49721 Max=5091885.05 Min=14.45 Avg=505.53 90=1141.76 99=1290.24 99.9=1508.35 99.99=11165.69]
Run runtime(sec): 41.98
Run operations(ops): 1000000
Run throughput(ops/sec): 23820.9
All steps completed successfully!
```

workloadc-1000000-20*1000

```
  LD       ycsb
2025-01-05 16:16:35 0 sec: 0 operations;
2025-01-05 16:16:45 10 sec: 101975 operations; [INSERT: Count=101975 Max=27623.42 Min=12.84 Avg=74.31 90=60.61 99=1182.72 99.9=1333.25 99.99=8036.35]
2025-01-05 16:16:56 20 sec: 141277 operations; [INSERT: Count=141277 Max=470810.62 Min=12.84 Avg=123.97 90=80.89 99=1200.13 99.9=1358.85 99.99=14753.79]
2025-01-05 16:17:06 30 sec: 162673 operations; [INSERT: Count=162673 Max=470810.62 Min=12.56 Avg=165.88 90=1085.44 99=1213.44 99.9=1372.16 99.99=16670.72]
2025-01-05 16:17:16 40 sec: 181556 operations; [INSERT: Count=181556 Max=470810.62 Min=12.56 Avg=201.11 90=1094.65 99=1222.65 99.9=1392.64 99.99=19496.96]
2025-01-05 16:17:27 51 sec: 207580 operations; [INSERT: Count=207580 Max=985137.15 Min=12.56 Avg=223.30 90=1096.70 99=1226.75 99.9=1410.05 99.99=84279.29]
2025-01-05 16:17:37 61 sec: 222234 operations; [INSERT: Count=222234 Max=985137.15 Min=12.56 Avg=251.87 90=1101.82 99=1233.92 99.9=1438.72 99.99=119275.52]
2025-01-05 16:17:47 71 sec: 227562 operations; [INSERT: Count=227562 Max=985137.15 Min=12.56 Avg=272.46 90=1103.87 99=1240.06 99.9=1460.22 99.99=118751.23]
2025-01-05 16:17:58 82 sec: 257979 operations; [INSERT: Count=257979 Max=7012876.29 Min=12.56 Avg=294.83 90=1101.82 99=1236.99 99.9=1459.20 99.99=130416.64]
2025-01-05 16:18:08 92 sec: 266587 operations; [INSERT: Count=266587 Max=7012876.29 Min=12.56 Avg=321.87 90=1106.94 99=1244.16 99.9=1477.63 99.99=128581.63]
2025-01-05 16:18:18 102 sec: 295023 operations; [INSERT: Count=295023 Max=7012876.29 Min=12.56 Avg=322.69 90=1103.87 99=1240.06 99.9=1470.46 99.99=134086.65]
2025-01-05 16:18:28 113 sec: 303656 operations; [INSERT: Count=303656 Max=7012876.29 Min=12.56 Avg=348.54 90=1107.97 99=1245.18 99.9=1494.02 99.99=134873.09]
2025-01-05 16:18:38 123 sec: 306714 operations; [INSERT: Count=306714 Max=7012876.29 Min=12.56 Avg=356.38 90=1110.02 99=1248.26 99.9=1504.26 99.99=134873.09]
2025-01-05 16:18:48 133 sec: 336841 operations; [INSERT: Count=336841 Max=9814671.36 Min=12.56 Avg=371.34 90=1107.97 99=1246.21 99.9=1494.02 99.99=139329.54]
2025-01-05 16:18:59 144 sec: 345483 operations; [INSERT: Count=345483 Max=9814671.36 Min=12.56 Avg=392.75 90=1111.04 99=1251.33 99.9=1507.33 99.99=139722.75]
2025-01-05 16:19:09 154 sec: 376962 operations; [INSERT: Count=376962 Max=9814671.36 Min=12.56 Avg=384.57 90=1110.02 99=1249.28 99.9=1507.33 99.99=148111.36]
2025-01-05 16:19:19 164 sec: 385568 operations; [INSERT: Count=385568 Max=9814671.36 Min=12.56 Avg=401.26 90=1114.11 99=1255.42 99.9=1524.73 99.99=146276.35]
2025-01-05 16:19:30 175 sec: 405654 operations; [INSERT: Count=405654 Max=9814671.36 Min=12.54 Avg=405.66 90=1111.04 99=1252.35 99.9=1511.42 99.99=149159.93]
2025-01-05 16:19:40 185 sec: 420338 operations; [INSERT: Count=420338 Max=9814671.36 Min=12.54 Avg=414.72 90=1114.11 99=1254.40 99.9=1522.69 99.99=149946.37]
2025-01-05 16:19:50 195 sec: 436205 operations; [INSERT: Count=436205 Max=9814671.36 Min=12.54 Avg=421.70 90=1114.11 99=1254.40 99.9=1525.76 99.99=151912.45]
2025-01-05 16:20:01 206 sec: 459163 operations; [INSERT: Count=459163 Max=9814671.36 Min=12.54 Avg=423.11 90=1115.13 99=1255.42 99.9=1531.90 99.99=158203.90]
2025-01-05 16:20:11 216 sec: 465018 operations; [INSERT: Count=465018 Max=9814671.36 Min=12.54 Avg=432.03 90=1118.21 99=1257.47 99.9=1534.97 99.99=157155.33]
2025-01-05 16:20:21 226 sec: 493288 operations; [INSERT: Count=493288 Max=9814671.36 Min=12.14 Avg=432.82 90=1114.11 99=1254.40 99.9=1522.69 99.99=157155.33]
2025-01-05 16:20:32 237 sec: 501926 operations; [INSERT: Count=501926 Max=9814671.36 Min=12.14 Avg=446.55 90=1117.18 99=1257.47 99.9=1534.97 99.99=158203.90]
2025-01-05 16:20:42 247 sec: 504594 operations; [INSERT: Count=504594 Max=9814671.36 Min=12.14 Avg=450.16 90=1118.21 99=1258.49 99.9=1537.02 99.99=158203.90]
2025-01-05 16:20:52 257 sec: 535147 operations; [INSERT: Count=535147 Max=9831448.57 Min=12.14 Avg=454.77 90=1116.16 99=1256.45 99.9=1528.83 99.99=157941.76]
2025-01-05 16:21:03 267 sec: 543777 operations; [INSERT: Count=543777 Max=9831448.57 Min=12.14 Avg=467.10 90=1119.23 99=1258.49 99.9=1534.97 99.99=158728.19]
2025-01-05 16:21:13 277 sec: 544170 operations; [INSERT: Count=544170 Max=9831448.57 Min=12.14 Avg=467.58 90=1119.23 99=1258.49 99.9=1536.00 99.99=158728.19]
2025-01-05 16:21:23 287 sec: 544170 operations; [INSERT: Count=544170 Max=9831448.57 Min=12.14 Avg=467.58 90=1119.23 99=1258.49 99.9=1536.00 99.99=158728.19]
2025-01-05 16:21:34 298 sec: 575881 operations; [INSERT: Count=575881 Max=21088960.51 Min=12.14 Avg=493.40 90=1118.21 99=1257.47 99.9=1534.97 99.99=161349.63]
2025-01-05 16:21:44 308 sec: 583746 operations; [INSERT: Count=583746 Max=21088960.51 Min=12.14 Avg=501.95 90=1120.26 99=1258.49 99.9=1540.10 99.99=161349.63]
2025-01-05 16:21:54 318 sec: 583746 operations; [INSERT: Count=583746 Max=21088960.51 Min=12.14 Avg=501.95 90=1120.26 99=1258.49 99.9=1540.10 99.99=161349.63]
2025-01-05 16:22:05 329 sec: 614931 operations; [INSERT: Count=614931 Max=21088960.51 Min=12.14 Avg=510.78 90=1119.23 99=1257.47 99.9=1538.05 99.99=161349.63]
2025-01-05 16:22:15 339 sec: 623322 operations; [INSERT: Count=623322 Max=21088960.51 Min=12.14 Avg=519.15 90=1121.28 99=1259.52 99.9=1547.26 99.99=160432.13]
2025-01-05 16:22:25 349 sec: 623322 operations; [INSERT: Count=623322 Max=21088960.51 Min=12.14 Avg=519.15 90=1121.28 99=1259.52 99.9=1547.26 99.99=160432.13]
2025-01-05 16:22:35 360 sec: 651217 operations; [INSERT: Count=651217 Max=21088960.51 Min=12.14 Avg=528.40 90=1119.23 99=1257.47 99.9=1540.10 99.99=163577.86]
2025-01-05 16:22:45 370 sec: 659827 operations; [INSERT: Count=659827 Max=21088960.51 Min=12.14 Avg=536.27 90=1121.28 99=1260.54 99.9=1549.31 99.99=162529.28]
2025-01-05 16:22:55 380 sec: 662898 operations; [INSERT: Count=662898 Max=21088960.51 Min=12.14 Avg=539.06 90=1122.30 99=1261.57 99.9=1559.55 99.99=162529.28]
2025-01-05 16:23:06 391 sec: 689468 operations; [INSERT: Count=689468 Max=21088960.51 Min=12.14 Avg=542.53 90=1119.23 99=1259.52 99.9=1545.21 99.99=166723.58]
2025-01-05 16:23:16 401 sec: 698079 operations; [INSERT: Count=698079 Max=21088960.51 Min=12.14 Avg=549.79 90=1121.28 99=1262.59 99.9=1562.62 99.99=165281.79]
2025-01-05 16:23:26 411 sec: 702474 operations; [INSERT: Count=702474 Max=21088960.51 Min=12.14 Avg=553.44 90=1122.30 99=1263.62 99.9=1567.74 99.99=165281.79]
2025-01-05 16:23:50 435 sec: 702474 operations; [INSERT: Count=702474 Max=21088960.51 Min=12.14 Avg=553.44 90=1122.30 99=1263.62 99.9=1567.74 99.99=165281.79]
2025-01-05 16:24:00 445 sec: 732891 operations; [INSERT: Count=732891 Max=31776047.10 Min=12.14 Avg=582.37 90=1121.28 99=1261.57 99.9=1562.62 99.99=162529.28]
2025-01-05 16:24:10 455 sec: 741528 operations; [INSERT: Count=741528 Max=31776047.10 Min=12.14 Avg=588.73 90=1123.33 99=1263.62 99.9=1563.65 99.99=161349.63]
2025-01-05 16:24:21 465 sec: 742050 operations; [INSERT: Count=742050 Max=31776047.10 Min=12.14 Avg=589.12 90=1123.33 99=1263.62 99.9=1563.65 99.99=161349.63]
2025-01-05 16:24:31 475 sec: 757060 operations; [INSERT: Count=757060 Max=31776047.10 Min=12.14 Avg=603.31 90=1121.28 99=1262.59 99.9=1560.58 99.99=161349.63]
2025-01-05 16:24:41 485 sec: 776087 operations; [INSERT: Count=776087 Max=31776047.10 Min=12.14 Avg=600.83 90=1123.33 99=1262.59 99.9=1561.60 99.99=160432.13]
2025-01-05 16:24:51 496 sec: 781626 operations; [INSERT: Count=781626 Max=31776047.10 Min=12.14 Avg=604.69 90=1124.35 99=1263.62 99.9=1563.65 99.99=160432.13]
2025-01-05 16:25:01 506 sec: 781626 operations; [INSERT: Count=781626 Max=31776047.10 Min=12.14 Avg=604.69 90=1124.35 99=1263.62 99.9=1563.65 99.99=160432.13]
2025-01-05 16:25:11 516 sec: 812210 operations; [INSERT: Count=812210 Max=31776047.10 Min=12.14 Avg=610.74 90=1123.33 99=1262.59 99.9=1560.58 99.99=161349.63]
2025-01-05 16:25:22 526 sec: 820767 operations; [INSERT: Count=820767 Max=31776047.10 Min=12.14 Avg=616.89 90=1124.35 99=1264.64 99.9=1564.67 99.99=161349.63]
2025-01-05 16:25:32 536 sec: 821202 operations; [INSERT: Count=821202 Max=31776047.10 Min=12.14 Avg=617.17 90=1125.38 99=1264.64 99.9=1564.67 99.99=161349.63]
2025-01-05 16:25:42 546 sec: 844288 operations; [INSERT: Count=844288 Max=31776047.10 Min=12.14 Avg=622.72 90=1123.33 99=1262.59 99.9=1559.55 99.99=162136.06]
2025-01-05 16:25:53 557 sec: 855709 operations; [INSERT: Count=855709 Max=31776047.10 Min=12.14 Avg=626.53 90=1124.35 99=1264.64 99.9=1562.62 99.99=163577.86]
2025-01-05 16:26:03 567 sec: 860778 operations; [INSERT: Count=860778 Max=31776047.10 Min=12.14 Avg=629.59 90=1125.38 99=1265.66 99.9=1565.69 99.99=163577.86]
2025-01-05 16:26:13 577 sec: 860778 operations; [INSERT: Count=860778 Max=31776047.10 Min=12.14 Avg=629.59 90=1125.38 99=1265.66 99.9=1565.69 99.99=163577.86]
2025-01-05 16:26:23 588 sec: 887962 operations; [INSERT: Count=887962 Max=31776047.10 Min=12.14 Avg=637.25 90=1123.33 99=1263.62 99.9=1559.55 99.99=166723.58]
2025-01-05 16:26:33 598 sec: 896425 operations; [INSERT: Count=896425 Max=31776047.10 Min=12.14 Avg=642.11 90=1125.38 99=1267.71 99.9=1567.74 99.99=166723.58]
2025-01-05 16:26:43 608 sec: 900354 operations; [INSERT: Count=900354 Max=31776047.10 Min=12.14 Avg=644.32 90=1126.40 99=1268.73 99.9=1576.96 99.99=166723.58]
2025-01-05 16:26:54 618 sec: 900354 operations; [INSERT: Count=900354 Max=31776047.10 Min=12.14 Avg=644.32 90=1126.40 99=1268.73 99.9=1576.96 99.99=166723.58]
2025-01-05 16:27:04 628 sec: 927874 operations; [INSERT: Count=927874 Max=31776047.10 Min=12.14 Avg=652.52 90=1125.38 99=1266.69 99.9=1567.74 99.99=166723.58]
2025-01-05 16:27:14 638 sec: 936401 operations; [INSERT: Count=936401 Max=31776047.10 Min=12.14 Avg=656.98 90=1126.40 99=1268.73 99.9=1572.86 99.99=166723.58]
2025-01-05 16:27:24 649 sec: 939930 operations; [INSERT: Count=939930 Max=31776047.10 Min=12.14 Avg=658.82 90=1127.42 99=1269.76 99.9=1574.91 99.99=166723.58]
2025-01-05 16:27:34 659 sec: 939930 operations; [INSERT: Count=939930 Max=31776047.10 Min=12.14 Avg=658.82 90=1127.42 99=1269.76 99.9=1574.91 99.99=166723.58]
2025-01-05 16:27:44 669 sec: 967876 operations; [INSERT: Count=967876 Max=31776047.10 Min=12.14 Avg=666.41 90=1126.40 99=1268.73 99.9=1567.74 99.99=166723.58]
2025-01-05 16:27:55 679 sec: 976335 operations; [INSERT: Count=976335 Max=31776047.10 Min=12.14 Avg=671.25 90=1128.45 99=1270.78 99.9=1575.93 99.99=167510.02]
2025-01-05 16:28:05 689 sec: 979506 operations; [INSERT: Count=979506 Max=31776047.10 Min=12.14 Avg=672.81 90=1128.45 99=1271.81 99.9=1576.96 99.99=167510.02]
2025-01-05 16:28:15 699 sec: 979506 operations; [INSERT: Count=979506 Max=31776047.10 Min=12.14 Avg=672.81 90=1128.45 99=1271.81 99.9=1576.96 99.99=167510.02]
2025-01-05 16:28:26 710 sec: 979506 operations; [INSERT: Count=979506 Max=31776047.10 Min=12.14 Avg=672.81 90=1128.45 99=1271.81 99.9=1576.96 99.99=167510.02]
2025-01-05 16:28:30 714 sec: 1000000 operations; [INSERT: Count=1000000 Max=31776047.10 Min=12.03 Avg=689.14 90=1127.42 99=1270.78 99.9=1568.77 99.99=169082.88]
Load runtime(sec): 714.6
Load operations(ops): 1000000
Load throughput(ops/sec): 1399.38
2025-01-05 16:28:30 0 sec: 0 operations;
2025-01-05 16:28:40 10 sec: 588638 operations; [READ: Count=588638 Max=8577.02 Min=3.35 Avg=15.81 90=21.97 99=53.92 99.9=116.22 99.99=180.48]
2025-01-05 16:28:46 16 sec: 1000000 operations; [READ: Count=1000000 Max=11018.24 Min=3.31 Avg=15.25 90=21.15 99=51.77 99.9=113.41 99.99=168.57]
Run runtime(sec): 16.6548
Run operations(ops): 1000000
Run throughput(ops/sec): 60042.8
All steps completed successfully!
```

workloadd-1000000-20*1000

```
  LD       ycsb
2025-01-05 16:34:10 0 sec: 0 operations;
2025-01-05 16:34:20 10 sec: 105561 operations; [INSERT: Count=105561 Max=17367.04 Min=11.79 Avg=72.11 90=43.39 99=1189.89 99.9=1323.01 99.99=6950.91]
2025-01-05 16:34:30 20 sec: 164600 operations; [INSERT: Count=164600 Max=50724.86 Min=11.79 Avg=97.89 90=49.92 99=1209.34 99.9=1359.87 99.99=9568.25]
2025-01-05 16:34:41 30 sec: 197517 operations; [INSERT: Count=197517 Max=449576.96 Min=11.79 Avg=131.16 90=82.37 99=1232.89 99.9=1431.55 99.99=11100.16]
2025-01-05 16:34:51 40 sec: 254774 operations; [INSERT: Count=254774 Max=449576.96 Min=11.79 Avg=135.63 90=96.38 99=1238.02 99.9=1438.72 99.99=11968.51]
2025-01-05 16:35:01 50 sec: 305015 operations; [INSERT: Count=305015 Max=449576.96 Min=11.79 Avg=142.10 90=263.68 99=1239.04 99.9=1445.89 99.99=11968.51]
2025-01-05 16:35:11 61 sec: 344034 operations; [INSERT: Count=344034 Max=449576.96 Min=11.79 Avg=153.88 90=1095.68 99=1243.13 99.9=1458.17 99.99=12689.41]
2025-01-05 16:35:21 71 sec: 381832 operations; [INSERT: Count=381832 Max=449576.96 Min=11.79 Avg=162.36 90=1101.82 99=1246.21 99.9=1468.41 99.99=12746.75]
2025-01-05 16:35:31 81 sec: 435326 operations; [INSERT: Count=435326 Max=449576.96 Min=11.14 Avg=162.44 90=1100.80 99=1245.18 99.9=1471.49 99.99=12410.88]
2025-01-05 16:35:42 91 sec: 469614 operations; [INSERT: Count=469614 Max=449576.96 Min=11.14 Avg=171.38 90=1103.87 99=1247.23 99.9=1479.68 99.99=13017.09]
2025-01-05 16:35:52 101 sec: 502942 operations; [INSERT: Count=502942 Max=449576.96 Min=11.14 Avg=178.30 90=1106.94 99=1249.28 99.9=1490.94 99.99=13000.70]
2025-01-05 16:36:02 111 sec: 537494 operations; [INSERT: Count=537494 Max=449576.96 Min=11.14 Avg=183.84 90=1107.97 99=1251.33 99.9=1495.04 99.99=12689.41]
2025-01-05 16:36:12 122 sec: 570065 operations; [INSERT: Count=570065 Max=449576.96 Min=11.14 Avg=189.74 90=1110.02 99=1253.38 99.9=1505.28 99.99=12689.41]
2025-01-05 16:36:22 132 sec: 578399 operations; [INSERT: Count=578399 Max=449576.96 Min=11.14 Avg=203.80 90=1115.13 99=1259.52 99.9=1533.95 99.99=12689.41]
2025-01-05 16:36:32 142 sec: 613210 operations; [INSERT: Count=613210 Max=449576.96 Min=11.14 Avg=207.14 90=1116.16 99=1259.52 99.9=1537.02 99.99=13000.70]
2025-01-05 16:36:43 153 sec: 648709 operations; [INSERT: Count=648709 Max=449576.96 Min=11.14 Avg=210.91 90=1116.16 99=1261.57 99.9=1547.26 99.99=13017.09]
2025-01-05 16:36:53 163 sec: 683156 operations; [INSERT: Count=683156 Max=449576.96 Min=11.14 Avg=213.64 90=1117.18 99=1262.59 99.9=1550.34 99.99=13017.09]
2025-01-05 16:37:03 173 sec: 714972 operations; [INSERT: Count=714972 Max=449576.96 Min=11.14 Avg=216.98 90=1118.21 99=1264.64 99.9=1553.41 99.99=12967.93]
2025-01-05 16:37:14 183 sec: 749226 operations; [INSERT: Count=749226 Max=449576.96 Min=11.14 Avg=220.17 90=1118.21 99=1265.66 99.9=1563.65 99.99=13000.70]
2025-01-05 16:37:24 193 sec: 780964 operations; [INSERT: Count=780964 Max=449576.96 Min=11.14 Avg=222.99 90=1119.23 99=1266.69 99.9=1566.72 99.99=13066.24]
2025-01-05 16:37:34 203 sec: 789352 operations; [INSERT: Count=789352 Max=449576.96 Min=11.14 Avg=232.93 90=1121.28 99=1269.76 99.9=1586.17 99.99=13058.05]
2025-01-05 16:37:44 214 sec: 824697 operations; [INSERT: Count=824697 Max=449576.96 Min=11.14 Avg=234.78 90=1122.30 99=1269.76 99.9=1583.10 99.99=13197.31]
2025-01-05 16:37:54 224 sec: 860492 operations; [INSERT: Count=860492 Max=449576.96 Min=11.14 Avg=235.59 90=1121.28 99=1269.76 99.9=1582.08 99.99=13066.24]
2025-01-05 16:38:04 234 sec: 894914 operations; [INSERT: Count=894914 Max=449576.96 Min=11.14 Avg=236.72 90=1122.30 99=1269.76 99.9=1582.08 99.99=13058.05]
2025-01-05 16:38:15 245 sec: 927388 operations; [INSERT: Count=927388 Max=449576.96 Min=11.14 Avg=238.82 90=1122.30 99=1269.76 99.9=1583.10 99.99=13058.05]
2025-01-05 16:38:25 255 sec: 961250 operations; [INSERT: Count=961250 Max=449576.96 Min=11.14 Avg=239.92 90=1122.30 99=1269.76 99.9=1583.10 99.99=13000.70]
2025-01-05 16:38:35 265 sec: 970086 operations; [INSERT: Count=970086 Max=449576.96 Min=11.14 Avg=247.73 90=1124.35 99=1271.81 99.9=1596.41 99.99=13017.09]
2025-01-05 16:38:40 269 sec: 1000000 operations; [INSERT: Count=1000000 Max=449576.96 Min=11.14 Avg=244.24 90=1123.33 99=1270.78 99.9=1589.25 99.99=12967.93]
Load runtime(sec): 269.687
Load operations(ops): 1000000
Load throughput(ops/sec): 3708
2025-01-05 16:38:40 0 sec: 0 operations;
2025-01-05 16:38:50 10 sec: 130475 operations; [INSERT: Count=6511 Max=261750.78 Min=1081.34 Avg=1213.33 90=1240.06 99=1400.83 99.9=8282.11 99.99=13459.45] [READ: Count=123964 Max=439615.49 Min=1.23 Avg=19.96 90=36.09 99=92.86 99.9=160.38 99.99=271.62]
2025-01-05 16:39:00 20 sec: 689248 operations; [INSERT: Count=34470 Max=261750.78 Min=11.70 Avg=331.39 90=1145.86 99=1305.60 99.9=2539.52 99.99=14663.68] [READ: Count=654778 Max=439615.49 Min=1.19 Avg=12.40 90=23.33 99=61.95 99.9=138.37 99.99=684.03]
2025-01-05 16:39:10 30 sec: 903365 operations; [INSERT: Count=45382 Max=261750.78 Min=11.70 Avg=405.09 90=1160.19 99=1312.77 99.9=2748.41 99.99=14614.53] [READ: Count=857983 Max=439615.49 Min=1.19 Avg=12.57 90=23.68 99=62.49 99.9=164.48 99.99=848.38]
2025-01-05 16:39:12 32 sec: 1000000 operations; [INSERT: Count=50259 Max=261750.78 Min=11.70 Avg=369.41 90=1152.00 99=1302.53 99.9=2490.37 99.99=14614.53] [READ: Count=949741 Max=439615.49 Min=1.19 Avg=12.52 90=23.18 99=61.18 99.9=216.45 99.99=913.41]
Run runtime(sec): 32.4106
Run operations(ops): 1000000
Run throughput(ops/sec): 30854.1
All steps completed successfully!
```

workloade-recordcount=100000-operationcount=10000--20*1000

```
  LD       ycsb
2025-01-05 16:40:46 0 sec: 0 operations;
2025-01-05 16:40:54 7 sec: 99999 operations; [INSERT: Count=99999 Max=446431.23 Min=12.53 Avg=59.93 90=44.16 99=1135.62 99.9=1263.62 99.99=6901.76]
Load runtime(sec): 7.89257
Load operations(ops): 100000
Load throughput(ops/sec): 12670.2
2025-01-05 16:40:54 0 sec: 0 operations;
2025-01-05 16:41:04 10 sec: 3761 operations; [INSERT: Count=190 Max=1444.86 Min=15.39 Avg=408.79 90=1183.74 99=1317.89 99.9=1444.86 99.99=1444.86] [SCAN: Count=3571 Max=26050.56 Min=11.06 Avg=2499.82 90=4599.81 99=7327.74 99.9=14221.31 99.99=26050.56]
2025-01-05 16:41:14 20 sec: 8017 operations; [INSERT: Count=393 Max=1444.86 Min=15.39 Avg=242.06 90=1141.76 99=1291.26 99.9=1444.86 99.99=1444.86] [SCAN: Count=7624 Max=26050.56 Min=11.06 Avg=2364.89 90=4304.90 99=6103.04 99.9=11247.61 99.99=17350.65]
2025-01-05 16:41:18 24 sec: 10000 operations; [INSERT: Count=486 Max=1444.86 Min=15.39 Avg=214.03 90=1116.16 99=1238.02 99.9=1444.86 99.99=1444.86] [SCAN: Count=9514 Max=26050.56 Min=11.06 Avg=2343.41 90=4272.13 99=5824.51 99.9=11067.39 99.99=17350.65]
Run runtime(sec): 24.7688
Run operations(ops): 10000
Run throughput(ops/sec): 403.734
All steps completed successfully!
```

workloadf-1000000-20*1000

```
  LD       ycsb
2025-01-05 16:41:55 0 sec: 0 operations;
2025-01-05 16:42:05 10 sec: 102224 operations; [INSERT: Count=102224 Max=27901.95 Min=12.74 Avg=74.54 90=55.33 99=1187.84 99.9=1379.33 99.99=9502.72]
2025-01-05 16:42:15 20 sec: 161040 operations; [INSERT: Count=161040 Max=38141.95 Min=12.69 Avg=100.71 90=65.25 99=1211.39 99.9=1429.50 99.99=9502.72]
2025-01-05 16:42:25 30 sec: 193791 operations; [INSERT: Count=193791 Max=439091.20 Min=12.18 Avg=133.94 90=93.31 99=1229.82 99.9=1455.10 99.99=10731.52]
2025-01-05 16:42:35 40 sec: 231997 operations; [INSERT: Count=231997 Max=439091.20 Min=12.18 Avg=150.88 90=1086.46 99=1239.04 99.9=1483.78 99.99=11157.50]
2025-01-05 16:42:45 50 sec: 284611 operations; [INSERT: Count=284611 Max=439091.20 Min=12.18 Avg=153.71 90=1088.51 99=1238.02 99.9=1476.61 99.99=11108.35]
2025-01-05 16:42:56 61 sec: 321802 operations; [INSERT: Count=321802 Max=439091.20 Min=12.18 Avg=165.91 90=1094.65 99=1241.09 99.9=1477.63 99.99=11919.36]
2025-01-05 16:43:06 71 sec: 380381 operations; [INSERT: Count=380381 Max=439091.20 Min=12.18 Avg=162.95 90=1091.58 99=1238.02 99.9=1476.61 99.99=11157.50]
2025-01-05 16:43:16 81 sec: 415393 operations; [INSERT: Count=415393 Max=439091.20 Min=12.18 Avg=171.23 90=1096.70 99=1241.09 99.9=1488.89 99.99=11714.56]
2025-01-05 16:43:26 91 sec: 447342 operations; [INSERT: Count=447342 Max=439091.20 Min=12.18 Avg=180.91 90=1100.80 99=1245.18 99.9=1498.11 99.99=11919.36]
2025-01-05 16:43:36 101 sec: 483007 operations; [INSERT: Count=483007 Max=439091.20 Min=11.59 Avg=186.44 90=1102.85 99=1246.21 99.9=1498.11 99.99=11747.33]
2025-01-05 16:43:46 111 sec: 515498 operations; [INSERT: Count=515498 Max=439091.20 Min=11.59 Avg=192.48 90=1105.92 99=1248.26 99.9=1503.23 99.99=11919.36]
2025-01-05 16:43:57 122 sec: 550230 operations; [INSERT: Count=550230 Max=439091.20 Min=11.59 Avg=197.94 90=1107.97 99=1250.30 99.9=1515.52 99.99=12206.08]
2025-01-05 16:44:07 132 sec: 572189 operations; [INSERT: Count=572189 Max=439091.20 Min=11.59 Avg=206.80 90=1110.02 99=1254.40 99.9=1524.73 99.99=12255.23]
2025-01-05 16:44:17 142 sec: 590995 operations; [INSERT: Count=590995 Max=439091.20 Min=11.59 Avg=216.25 90=1113.09 99=1256.45 99.9=1533.95 99.99=12230.66]
2025-01-05 16:44:27 152 sec: 626022 operations; [INSERT: Count=626022 Max=439091.20 Min=11.59 Avg=219.64 90=1113.09 99=1257.47 99.9=1543.17 99.99=12648.45]
2025-01-05 16:44:37 162 sec: 659549 operations; [INSERT: Count=659549 Max=439091.20 Min=11.59 Avg=222.40 90=1114.11 99=1258.49 99.9=1541.12 99.99=12697.60]
2025-01-05 16:44:47 172 sec: 677703 operations; [INSERT: Count=677703 Max=439091.20 Min=11.59 Avg=230.48 90=1115.13 99=1260.54 99.9=1546.24 99.99=12697.60]
2025-01-05 16:44:58 183 sec: 702978 operations; [INSERT: Count=702978 Max=439091.20 Min=11.59 Avg=236.26 90=1116.16 99=1260.54 99.9=1543.17 99.99=12730.37]
2025-01-05 16:45:08 193 sec: 709070 operations; [INSERT: Count=709070 Max=439091.20 Min=11.59 Avg=244.10 90=1117.18 99=1262.59 99.9=1551.36 99.99=12697.60]
2025-01-05 16:45:18 203 sec: 738300 operations; [INSERT: Count=738300 Max=7092568.06 Min=11.59 Avg=250.91 90=1116.16 99=1261.57 99.9=1541.12 99.99=13385.73]
2025-01-05 16:45:29 214 sec: 746759 operations; [INSERT: Count=746759 Max=7092568.06 Min=11.59 Avg=261.94 90=1118.21 99=1264.64 99.9=1553.41 99.99=13557.76]
2025-01-05 16:45:39 224 sec: 748646 operations; [INSERT: Count=748646 Max=7092568.06 Min=11.59 Avg=264.18 90=1119.23 99=1265.66 99.9=1556.48 99.99=13557.76]
2025-01-05 16:45:49 234 sec: 776980 operations; [INSERT: Count=776980 Max=13220446.21 Min=11.59 Avg=276.59 90=1118.21 99=1263.62 99.9=1547.26 99.99=13983.74]
2025-01-05 16:45:59 244 sec: 785516 operations; [INSERT: Count=785516 Max=13220446.21 Min=11.59 Avg=286.25 90=1119.23 99=1265.66 99.9=1552.38 99.99=13983.74]
2025-01-05 16:46:09 254 sec: 788222 operations; [INSERT: Count=788222 Max=13220446.21 Min=11.59 Avg=289.19 90=1120.26 99=1266.69 99.9=1552.38 99.99=13983.74]
2025-01-05 16:46:19 264 sec: 788222 operations; [INSERT: Count=788222 Max=13220446.21 Min=11.59 Avg=289.19 90=1120.26 99=1266.69 99.9=1552.38 99.99=13983.74]
2025-01-05 16:46:30 275 sec: 820602 operations; [INSERT: Count=820602 Max=17448304.64 Min=11.59 Avg=310.24 90=1119.23 99=1266.69 99.9=1552.38 99.99=14999.55]
2025-01-05 16:46:40 285 sec: 827798 operations; [INSERT: Count=827798 Max=17448304.64 Min=11.59 Avg=317.50 90=1121.28 99=1268.73 99.9=1559.55 99.99=14999.55]
2025-01-05 16:46:50 295 sec: 827798 operations; [INSERT: Count=827798 Max=17448304.64 Min=11.59 Avg=317.50 90=1121.28 99=1268.73 99.9=1559.55 99.99=14999.55]
2025-01-05 16:47:00 305 sec: 859747 operations; [INSERT: Count=859747 Max=17448304.64 Min=11.59 Avg=330.55 90=1120.26 99=1267.71 99.9=1553.41 99.99=16162.82]
2025-01-05 16:47:10 315 sec: 867374 operations; [INSERT: Count=867374 Max=17448304.64 Min=11.59 Avg=337.70 90=1121.28 99=1269.76 99.9=1554.43 99.99=15966.21]
2025-01-05 16:47:20 325 sec: 867374 operations; [INSERT: Count=867374 Max=17448304.64 Min=11.59 Avg=337.70 90=1121.28 99=1269.76 99.9=1554.43 99.99=15966.21]
2025-01-05 16:47:31 336 sec: 883864 operations; [INSERT: Count=883864 Max=20652752.89 Min=11.59 Avg=355.46 90=1120.26 99=1268.73 99.9=1549.31 99.99=16302.08]
2025-01-05 16:47:41 346 sec: 901480 operations; [INSERT: Count=901480 Max=20652752.89 Min=11.59 Avg=359.14 90=1121.28 99=1268.73 99.9=1552.38 99.99=16859.13]
2025-01-05 16:47:51 356 sec: 906950 operations; [INSERT: Count=906950 Max=20652752.89 Min=11.59 Avg=363.89 90=1122.30 99=1270.78 99.9=1557.50 99.99=16752.64]
2025-01-05 16:48:02 367 sec: 906950 operations; [INSERT: Count=906950 Max=20652752.89 Min=11.59 Avg=363.89 90=1122.30 99=1270.78 99.9=1557.50 99.99=16752.64]
2025-01-05 16:48:12 377 sec: 938957 operations; [INSERT: Count=938957 Max=20652752.89 Min=11.59 Avg=376.37 90=1122.30 99=1270.78 99.9=1550.34 99.99=16859.13]
2025-01-05 16:48:22 387 sec: 946526 operations; [INSERT: Count=946526 Max=20652752.89 Min=11.59 Avg=382.52 90=1123.33 99=1271.81 99.9=1552.38 99.99=16752.64]
2025-01-05 16:48:32 397 sec: 946526 operations; [INSERT: Count=946526 Max=20652752.89 Min=11.59 Avg=382.52 90=1123.33 99=1271.81 99.9=1552.38 99.99=16752.64]
2025-01-05 16:48:42 407 sec: 950285 operations; [INSERT: Count=950285 Max=21374173.18 Min=11.59 Avg=403.75 90=1123.33 99=1271.81 99.9=1552.38 99.99=17039.36]
2025-01-05 16:48:52 417 sec: 979674 operations; [INSERT: Count=979674 Max=21374173.18 Min=11.59 Avg=401.09 90=1123.33 99=1271.81 99.9=1552.38 99.99=18202.62]
2025-01-05 16:49:03 428 sec: 986102 operations; [INSERT: Count=986102 Max=21374173.18 Min=11.59 Avg=406.56 90=1124.35 99=1272.83 99.9=1554.43 99.99=18792.45]
2025-01-05 16:49:13 438 sec: 986102 operations; [INSERT: Count=986102 Max=21374173.18 Min=11.59 Avg=406.56 90=1124.35 99=1272.83 99.9=1554.43 99.99=18792.45]
2025-01-05 16:49:16 441 sec: 1000000 operations; [INSERT: Count=1000000 Max=21374173.18 Min=11.59 Avg=415.65 90=1123.33 99=1272.83 99.9=1551.36 99.99=18825.22]
Load runtime(sec): 441.168
Load operations(ops): 1000000
Load throughput(ops/sec): 2266.71
2025-01-05 16:49:16 0 sec: 0 operations;
2025-01-05 16:49:26 10 sec: 50686 operations; [READ: Count=33796 Max=477.44 Min=1.68 Avg=30.50 90=58.53 99=130.30 99.9=236.67 99.99=368.64] [UPDATE: Count=16890 Max=12427.26 Min=15.27 Avg=500.79 90=1171.45 99=1301.50 99.9=1567.74 99.99=11362.30]
2025-01-05 16:49:36 20 sec: 69363 operations; [READ: Count=46277 Max=477.44 Min=1.68 Avg=33.11 90=64.58 99=138.37 99.9=232.45 99.99=313.60] [UPDATE: Count=23086 Max=427294.72 Min=15.27 Avg=701.93 90=1196.03 99=1324.03 99.9=1646.59 99.99=12427.26]
2025-01-05 16:49:46 30 sec: 69363 operations; [READ: Count=46277 Max=477.44 Min=1.68 Avg=33.11 90=64.58 99=138.37 99.9=232.45 99.99=313.60] [UPDATE: Count=23086 Max=427294.72 Min=15.27 Avg=701.93 90=1196.03 99=1324.03 99.9=1646.59 99.99=12427.26]
2025-01-05 16:49:56 40 sec: 128651 operations; [READ: Count=85787 Max=532.48 Min=1.68 Avg=26.33 90=49.98 99=122.56 99.9=207.87 99.99=305.92] [UPDATE: Count=42864 Max=20451426.30 Min=13.53 Avg=880.94 90=1141.76 99=1287.17 99.9=1523.71 99.99=131465.21]
2025-01-05 16:50:07 51 sec: 165468 operations; [READ: Count=110307 Max=416808.96 Min=1.68 Avg=32.23 90=53.70 99=138.75 99.9=288.51 99.99=528.89] [UPDATE: Count=55161 Max=20451426.30 Min=13.53 Avg=851.75 90=1173.50 99=1315.84 99.9=1639.42 99.99=107413.50]
2025-01-05 16:50:17 61 sec: 188007 operations; [READ: Count=125345 Max=416808.96 Min=1.68 Avg=33.76 90=57.38 99=154.11 99.9=328.70 99.99=827.90] [UPDATE: Count=62662 Max=20451426.30 Min=13.53 Avg=892.18 90=1205.25 99=1388.54 99.9=1708.03 99.99=107413.50]
2025-01-05 16:50:27 71 sec: 188007 operations; [READ: Count=125345 Max=416808.96 Min=1.68 Avg=33.76 90=57.38 99=154.11 99.9=328.70 99.99=827.90] [UPDATE: Count=62662 Max=20451426.30 Min=13.53 Avg=892.18 90=1205.25 99=1388.54 99.9=1708.03 99.99=107413.50]
2025-01-05 16:50:38 81 sec: 278529 operations; [READ: Count=185562 Max=425984.00 Min=1.68 Avg=39.25 90=53.34 99=166.27 99.9=2449.41 99.99=4435.97] [UPDATE: Count=92967 Max=20451426.30 Min=13.53 Avg=792.96 90=1180.67 99=1349.63 99.9=1615.87 99.99=12427.26]
2025-01-05 16:50:48 91 sec: 301430 operations; [READ: Count=200806 Max=425984.00 Min=1.68 Avg=41.88 90=55.87 99=177.79 99.9=2768.89 99.99=4571.14] [UPDATE: Count=100624 Max=20451426.30 Min=13.53 Avg=820.15 90=1188.86 99=1350.65 99.9=1617.92 99.99=12427.26]
2025-01-05 16:50:58 101 sec: 306301 operations; [READ: Count=204063 Max=425984.00 Min=1.68 Avg=42.61 90=56.41 99=180.86 99.9=2914.30 99.99=4628.48] [UPDATE: Count=102238 Max=20451426.30 Min=13.53 Avg=825.23 90=1189.89 99=1350.65 99.9=1622.02 99.99=12427.26]
2025-01-05 16:51:08 112 sec: 381267 operations; [READ: Count=254134 Max=425984.00 Min=1.48 Avg=44.94 90=52.96 99=216.70 99.9=3215.36 99.99=5038.08] [UPDATE: Count=127133 Max=20451426.30 Min=13.53 Avg=786.77 90=1169.41 99=1329.15 99.9=1583.10 99.99=11886.59]
2025-01-05 16:51:18 122 sec: 404759 operations; [READ: Count=269795 Max=425984.00 Min=1.48 Avg=47.67 90=54.40 99=245.50 99.9=3416.06 99.99=5459.97] [UPDATE: Count=134964 Max=20451426.30 Min=13.53 Avg=804.09 90=1175.55 99=1331.20 99.9=1594.37 99.99=11886.59]
2025-01-05 16:51:28 132 sec: 425225 operations; [READ: Count=283411 Max=425984.00 Min=1.48 Avg=50.03 90=55.94 99=305.66 99.9=3553.28 99.99=5783.55] [UPDATE: Count=141814 Max=20451426.30 Min=13.53 Avg=820.55 90=1179.65 99=1332.22 99.9=1601.54 99.99=11657.22]
2025-01-05 16:51:38 142 sec: 483667 operations; [READ: Count=322447 Max=425984.00 Min=1.48 Avg=52.79 90=53.98 99=455.94 99.9=3700.74 99.99=6180.86] [UPDATE: Count=161220 Max=20451426.30 Min=13.53 Avg=771.94 90=1168.38 99=1319.93 99.9=1579.01 99.99=11558.91]
2025-01-05 16:51:48 152 sec: 518061 operations; [READ: Count=345294 Max=425984.00 Min=1.48 Avg=55.81 90=54.75 99=599.04 99.9=3774.46 99.99=6209.53] [UPDATE: Count=172767 Max=20451426.30 Min=13.53 Avg=764.82 90=1170.43 99=1319.93 99.9=1588.22 99.99=11558.91]
2025-01-05 16:51:58 162 sec: 539077 operations; [READ: Count=359305 Max=425984.00 Min=1.48 Avg=58.77 90=56.38 99=797.70 99.9=3903.49 99.99=6578.18] [UPDATE: Count=179772 Max=20451426.30 Min=13.53 Avg=780.04 90=1176.58 99=1321.98 99.9=1593.34 99.99=11886.59]
2025-01-05 16:52:09 173 sec: 567475 operations; [READ: Count=378216 Max=425984.00 Min=1.48 Avg=61.04 90=56.32 99=1164.29 99.9=3991.55 99.99=6922.24] [UPDATE: Count=189259 Max=20451426.30 Min=13.05 Avg=785.89 90=1173.50 99=1319.93 99.9=1586.17 99.99=12419.07]
2025-01-05 16:52:20 183 sec: 630077 operations; [READ: Count=420027 Max=425984.00 Min=1.48 Avg=65.05 90=55.97 99=1682.43 99.9=4173.82 99.99=7344.13] [UPDATE: Count=210050 Max=20451426.30 Min=13.05 Avg=737.84 90=1168.38 99=1314.82 99.9=1575.93 99.99=12419.07]
2025-01-05 16:52:30 193 sec: 651113 operations; [READ: Count=434077 Max=425984.00 Min=1.48 Avg=67.34 90=57.02 99=1821.69 99.9=4276.22 99.99=7569.41] [UPDATE: Count=217036 Max=20451426.30 Min=13.05 Avg=751.10 90=1172.48 99=1315.84 99.9=1579.01 99.99=12419.07]
2025-01-05 16:52:40 204 sec: 683461 operations; [READ: Count=455645 Max=425984.00 Min=1.48 Avg=69.08 90=57.28 99=1918.97 99.9=4321.28 99.99=7598.08] [UPDATE: Count=227816 Max=20451426.30 Min=13.05 Avg=751.16 90=1171.45 99=1315.84 99.9=1577.98 99.99=12427.26]
2025-01-05 16:52:50 214 sec: 748094 operations; [READ: Count=498852 Max=425984.00 Min=1.40 Avg=72.81 90=57.15 99=2083.84 99.9=4407.30 99.99=7716.86] [UPDATE: Count=249242 Max=20451426.30 Min=13.05 Avg=706.88 90=1167.36 99=1309.69 99.9=1571.84 99.99=12419.07]
2025-01-05 16:53:00 224 sec: 768739 operations; [READ: Count=512676 Max=425984.00 Min=1.40 Avg=74.84 90=58.27 99=2148.35 99.9=4444.16 99.99=7761.92] [UPDATE: Count=256063 Max=20451426.30 Min=13.05 Avg=718.84 90=1171.45 99=1314.82 99.9=1577.98 99.99=12427.26]
2025-01-05 16:53:11 234 sec: 829288 operations; [READ: Count=552973 Max=425984.00 Min=1.40 Avg=79.07 90=58.37 99=2271.23 99.9=4534.27 99.99=7835.65] [UPDATE: Count=276315 Max=20451426.30 Min=13.05 Avg=684.77 90=1167.36 99=1309.69 99.9=1570.82 99.99=12427.26]
2025-01-05 16:53:21 244 sec: 870815 operations; [READ: Count=580658 Max=425984.00 Min=1.40 Avg=81.97 90=58.69 99=2351.10 99.9=4640.77 99.99=8060.93] [UPDATE: Count=290157 Max=20451426.30 Min=13.05 Avg=672.94 90=1167.36 99=1309.69 99.9=1570.82 99.99=13164.54]
2025-01-05 16:53:31 254 sec: 890992 operations; [READ: Count=594140 Max=425984.00 Min=1.40 Avg=83.89 90=59.49 99=2400.26 99.9=4698.11 99.99=8179.71] [UPDATE: Count=296852 Max=20451426.30 Min=13.05 Avg=683.70 90=1169.41 99=1312.77 99.9=1577.98 99.99=13164.54]
2025-01-05 16:53:41 265 sec: 930151 operations; [READ: Count=620292 Max=425984.00 Min=1.40 Avg=85.55 90=59.68 99=2449.41 99.9=4718.59 99.99=8212.48] [UPDATE: Count=309859 Max=20451426.30 Min=13.05 Avg=678.72 90=1168.38 99=1310.72 99.9=1575.93 99.99=13656.06]
2025-01-05 16:53:51 275 sec: 984167 operations; [READ: Count=656362 Max=425984.00 Min=1.40 Avg=88.62 90=59.84 99=2523.14 99.9=4829.18 99.99=8404.99] [UPDATE: Count=327805 Max=20451426.30 Min=13.05 Avg=656.22 90=1166.34 99=1310.72 99.9=1576.96 99.99=13656.06]
2025-01-05 16:54:01 285 sec: 1003410 operations; [READ: Count=669145 Max=425984.00 Min=1.40 Avg=90.65 90=60.70 99=2566.14 99.9=4829.18 99.99=8413.18] [UPDATE: Count=334265 Max=20451426.30 Min=13.05 Avg=665.84 90=1168.38 99=1314.82 99.9=1584.13 99.99=13656.06]
2025-01-05 16:54:12 296 sec: 1042131 operations; [READ: Count=694976 Max=425984.00 Min=1.40 Avg=92.95 90=61.28 99=2613.25 99.9=4878.34 99.99=8413.18] [UPDATE: Count=347155 Max=20451426.30 Min=13.05 Avg=660.05 90=1168.38 99=1314.82 99.9=1584.13 99.99=14041.09]
2025-01-05 16:54:22 306 sec: 1101345 operations; [READ: Count=734373 Max=425984.00 Min=1.40 Avg=95.42 90=61.34 99=2652.16 99.9=4960.26 99.99=8445.95] [UPDATE: Count=366972 Max=20451426.30 Min=13.05 Avg=636.47 90=1165.31 99=1311.74 99.9=1579.01 99.99=13893.63]
2025-01-05 16:54:32 316 sec: 1120558 operations; [READ: Count=747197 Max=425984.00 Min=1.40 Avg=97.24 90=62.14 99=2680.83 99.9=5033.98 99.99=8617.98] [UPDATE: Count=373361 Max=20451426.30 Min=13.05 Avg=645.30 90=1168.38 99=1313.79 99.9=1584.13 99.99=14991.36]
2025-01-05 16:54:42 326 sec: 1158975 operations; [READ: Count=772728 Max=425984.00 Min=1.40 Avg=99.21 90=62.66 99=2711.55 99.9=5042.18 99.99=8626.17] [UPDATE: Count=386247 Max=20451426.30 Min=13.05 Avg=640.70 90=1167.36 99=1312.77 99.9=1584.13 99.99=15302.66]
2025-01-05 16:54:52 336 sec: 1219610 operations; [READ: Count=813227 Max=425984.00 Min=1.39 Avg=101.38 90=62.72 99=2742.27 99.9=5083.14 99.99=8585.22] [UPDATE: Count=406383 Max=20451426.30 Min=13.05 Avg=619.10 90=1165.31 99=1309.69 99.9=1582.08 99.99=14524.42]
2025-01-05 16:55:02 346 sec: 1238972 operations; [READ: Count=826156 Max=425984.00 Min=1.39 Avg=102.80 90=63.55 99=2760.70 99.9=5148.67 99.99=8626.17] [UPDATE: Count=412816 Max=20451426.30 Min=13.05 Avg=627.50 90=1167.36 99=1312.77 99.9=1587.20 99.99=14991.36]
2025-01-05 16:55:13 357 sec: 1270961 operations; [READ: Count=847570 Max=425984.00 Min=1.39 Avg=104.38 90=64.25 99=2770.94 99.9=5165.06 99.99=8945.66] [UPDATE: Count=423391 Max=20451426.30 Min=13.05 Avg=628.31 90=1168.38 99=1314.82 99.9=1590.27 99.99=15302.66]
2025-01-05 16:55:23 367 sec: 1336590 operations; [READ: Count=891387 Max=425984.00 Min=1.39 Avg=106.37 90=64.38 99=2781.18 99.9=5136.38 99.99=9076.74] [UPDATE: Count=445203 Max=20451426.30 Min=12.53 Avg=605.49 90=1165.31 99=1310.72 99.9=1584.13 99.99=14524.42]
2025-01-05 16:55:33 377 sec: 1356137 operations; [READ: Count=904417 Max=425984.00 Min=1.39 Avg=107.53 90=65.25 99=2791.42 99.9=5132.29 99.99=9076.74] [UPDATE: Count=451720 Max=20451426.30 Min=12.53 Avg=613.37 90=1167.36 99=1311.74 99.9=1584.13 99.99=15302.66]
2025-01-05 16:55:44 387 sec: 1375599 operations; [READ: Count=917433 Max=425984.00 Min=1.39 Avg=108.75 90=66.11 99=2805.76 99.9=5160.96 99.99=9076.74] [UPDATE: Count=458166 Max=20451426.30 Min=12.53 Avg=622.31 90=1169.41 99=1312.77 99.9=1586.17 99.99=15663.10]
2025-01-05 16:55:54 397 sec: 1450516 operations; [READ: Count=967339 Max=425984.00 Min=1.39 Avg=111.21 90=66.43 99=2830.34 99.9=5210.11 99.99=9043.97] [UPDATE: Count=483177 Max=20451426.30 Min=12.53 Avg=594.42 90=1165.31 99=1308.67 99.9=1577.98 99.99=15507.45]
2025-01-05 16:56:04 407 sec: 1468268 operations; [READ: Count=979168 Max=425984.00 Min=1.39 Avg=112.99 90=67.26 99=2850.82 99.9=5242.88 99.99=9076.74] [UPDATE: Count=489100 Max=20451426.30 Min=12.53 Avg=601.30 90=1167.36 99=1310.72 99.9=1584.13 99.99=15548.42]
2025-01-05 16:56:14 418 sec: 1487251 operations; [READ: Count=991901 Max=425984.00 Min=1.39 Avg=114.74 90=68.16 99=2865.15 99.9=5275.65 99.99=9109.50] [UPDATE: Count=495350 Max=20451426.30 Min=12.53 Avg=608.63 90=1168.38 99=1311.74 99.9=1584.13 99.99=15663.10]
2025-01-05 16:56:18 422 sec: 1499359 operations; [READ: Count=1000000 Max=425984.00 Min=1.39 Avg=115.39 90=68.42 99=2873.34 99.9=5287.94 99.99=9109.50] [UPDATE: Count=499359 Max=20451426.30 Min=12.53 Avg=607.08 90=1168.38 99=1311.74 99.9=1584.13 99.99=15745.02]
Run runtime(sec): 422.052
Run operations(ops): 1000000
Run throughput(ops/sec): 2369.37
All steps completed successfully!
```



# YCSB-Leveldb:V3

## 10fields*500field-len

workloada-1000000-10*500

```
  LD       ycsb
2025-01-04 18:43:32 0 sec: 0 operations;
2025-01-04 18:43:42 10 sec: 590593 operations; [INSERT: Count=590593 Max=6512.64 Min=5.33 Avg=10.98 90=14.71 99=55.58 99.9=88.00 99.99=333.82]
2025-01-04 18:43:50 18 sec: 1000000 operations; [INSERT: Count=1000000 Max=6512.64 Min=5.33 Avg=12.16 90=17.70 99=57.34 99.9=92.67 99.99=316.16]
Load runtime(sec): 18.115
Load operations(ops): 1000000
Load throughput(ops/sec): 55202.8
2025-01-04 18:43:50 0 sec: 0 operations;
2025-01-04 18:44:00 9 sec: 493604 operations; [READ: Count=246810 Max=999.93 Min=2.70 Avg=6.92 90=9.02 99=19.09 99.9=47.94 99.99=306.18] [UPDATE: Count=246794 Max=25296.90 Min=9.01 Avg=16.96 90=21.65 99=60.41 99.9=102.59 99.99=590.34]
2025-01-04 18:44:07 17 sec: 999999 operations; [READ: Count=500268 Max=999.93 Min=2.70 Avg=7.42 90=10.09 99=21.34 99.9=52.13 99.99=331.01] [UPDATE: Count=499731 Max=25296.90 Min=9.00 Avg=18.53 90=27.18 99=69.69 99.9=111.17 99.99=534.53]
Run runtime(sec): 17.3849
Run operations(ops): 1000000
Run throughput(ops/sec): 57521.2
All steps completed successfully!
```



workloadb-1000000-10*500

```
  LD       ycsb
2025-01-04 18:45:54 0 sec: 0 operations;
2025-01-04 18:46:04 9 sec: 590276 operations; [INSERT: Count=590276 Max=13189.12 Min=5.33 Avg=11.21 90=15.29 99=55.81 99.9=78.97 99.99=322.30]
2025-01-04 18:46:12 17 sec: 1000000 operations; [INSERT: Count=1000000 Max=13189.12 Min=5.33 Avg=12.17 90=17.41 99=57.41 99.9=83.84 99.99=284.42]
Load runtime(sec): 17.9167
Load operations(ops): 1000000
Load throughput(ops/sec): 55813.8
2025-01-04 18:46:12 0 sec: 0 operations;
2025-01-04 18:46:22 10 sec: 804419 operations; [READ: Count=764183 Max=926.21 Min=2.69 Avg=6.23 90=7.86 99=16.10 99.9=30.53 99.99=357.38] [UPDATE: Count=40236 Max=13180.93 Min=9.38 Avg=39.70 90=49.38 99=84.99 99.9=370.18 99.99=10567.68]
2025-01-04 18:46:23 11 sec: 1000000 operations; [READ: Count=949947 Max=926.21 Min=2.69 Avg=6.28 90=7.99 99=16.19 99.9=30.56 99.99=360.96] [UPDATE: Count=50053 Max=13352.96 Min=9.38 Avg=40.81 90=49.34 99=85.50 99.9=404.48 99.99=11501.57]
Run runtime(sec): 11.7185
Run operations(ops): 1000000
Run throughput(ops/sec): 85335.4
All steps completed successfully!
```



workloadc-1000000-10*500

```
  LD       ycsb
2025-01-04 18:48:23 0 sec: 0 operations;
2025-01-04 18:48:33 9 sec: 597329 operations; [INSERT: Count=597329 Max=6266.88 Min=5.44 Avg=11.03 90=16.27 99=53.38 99.9=71.55 99.99=263.94]
2025-01-04 18:48:41 17 sec: 999999 operations; [INSERT: Count=999999 Max=14565.38 Min=5.44 Avg=11.96 90=17.73 99=54.21 99.9=80.64 99.99=255.62]
Load runtime(sec): 17.7072
Load operations(ops): 1000000
Load throughput(ops/sec): 56474
2025-01-04 18:48:41 0 sec: 0 operations;
2025-01-04 18:48:51 10 sec: 820264 operations; [READ: Count=820264 Max=1132.54 Min=3.41 Avg=7.91 90=12.02 99=43.87 99.9=70.66 99.99=378.88]
2025-01-04 18:48:52 11 sec: 1000000 operations; [READ: Count=1000000 Max=1132.54 Min=3.41 Avg=7.75 90=11.65 99=43.65 99.9=68.93 99.99=376.83]
Run runtime(sec): 11.3179
Run operations(ops): 1000000
Run throughput(ops/sec): 88355.8
All steps completed successfully!
```



workloadd-1000000-10*500

```
  LD       ycsb
2025-01-04 18:51:10 0 sec: 0 operations;
2025-01-04 18:51:20 10 sec: 634524 operations; [INSERT: Count=634524 Max=4374.53 Min=5.41 Avg=10.15 90=13.69 99=51.17 99.9=66.88 99.99=236.16]
2025-01-04 18:51:26 16 sec: 1000000 operations; [INSERT: Count=1000000 Max=23871.49 Min=5.41 Avg=11.26 90=16.43 99=53.60 99.9=70.33 99.99=238.46]
Load runtime(sec): 16.8912
Load operations(ops): 1000000
Load throughput(ops/sec): 59202.3
2025-01-04 18:51:26 0 sec: 0 operations;
2025-01-04 18:51:36 9 sec: 937394 operations; [INSERT: Count=46788 Max=11616.25 Min=5.84 Avg=31.66 90=41.47 99=75.07 99.9=322.56 99.99=10829.82] [READ: Count=890606 Max=17203.20 Min=2.26 Avg=5.35 90=7.63 99=15.26 99.9=28.22 99.99=355.58]
2025-01-04 18:51:37 10 sec: 1000000 operations; [INSERT: Count=50016 Max=11616.25 Min=5.84 Avg=31.64 90=41.73 99=75.39 99.9=322.56 99.99=10829.82] [READ: Count=949984 Max=17203.20 Min=2.26 Avg=5.36 90=7.69 99=15.29 99.9=28.30 99.99=361.98]
Run runtime(sec): 10.4692
Run operations(ops): 1000000
Run throughput(ops/sec): 95518
All steps completed successfully!
```



workloade-recordcount=100000-operationcount=10000-10*500

```
  LD       ycsb
2025-01-04 19:04:28 0 sec: 0 operations;
2025-01-04 19:04:30 1 sec: 100000 operations; [INSERT: Count=100000 Max=578.05 Min=5.40 Avg=7.60 90=8.70 99=25.23 99.9=54.11 99.99=236.29]
Load runtime(sec): 1.30505
Load operations(ops): 100000
Load throughput(ops/sec): 76625.5
2025-01-04 19:04:30 0 sec: 0 operations;
2025-01-04 19:04:39 9 sec: 2405 operations; [INSERT: Count=138 Max=134.91 Min=14.48 Avg=73.29 90=88.70 99=117.82 99.9=134.91 99.99=134.91] [SCAN: Count=2267 Max=9035.77 Min=107.52 Avg=4251.70 90=7323.65 99=8421.38 99.9=8855.55 99.99=9035.77]
2025-01-04 19:04:49 19 sec: 4639 operations; [INSERT: Count=246 Max=608.25 Min=8.88 Avg=77.09 90=93.06 99=117.82 99.9=608.25 99.99=608.25] [SCAN: Count=4393 Max=11075.58 Min=37.76 Avg=4449.28 90=7548.93 99=9134.08 99.9=9961.47 99.99=11075.58]
2025-01-04 19:04:59 29 sec: 6625 operations; [INSERT: Count=344 Max=608.25 Min=8.88 Avg=80.27 90=99.01 99=121.79 99.9=608.25 99.99=608.25] [SCAN: Count=6281 Max=11075.58 Min=37.76 Avg=4688.05 90=7892.99 99=9576.45 99.9=10354.69 99.99=10534.91]
2025-01-04 19:05:10 40 sec: 8781 operations; [INSERT: Count=439 Max=608.25 Min=7.84 Avg=80.83 90=99.07 99=129.34 99.9=608.25 99.99=608.25] [SCAN: Count=8342 Max=1127219.20 Min=37.76 Avg=4850.43 90=7970.81 99=9535.49 99.9=10321.92 99.99=11075.58]
2025-01-04 19:05:16 46 sec: 9998 operations; [INSERT: Count=503 Max=1876.99 Min=7.84 Avg=84.20 90=99.26 99=129.34 99.9=608.25 99.99=1876.99] [SCAN: Count=9495 Max=1127219.20 Min=37.76 Avg=4850.71 90=7991.30 99=9494.53 99.9=10313.73 99.99=11075.58]
Run runtime(sec): 46.3558
Run operations(ops): 10000
Run throughput(ops/sec): 215.723
All steps completed successfully!
```



workloadf-1000000-10*500

```
  LD       ycsb
2025-01-04 19:00:10 0 sec: 0 operations;
2025-01-04 19:00:20 10 sec: 700006 operations; [INSERT: Count=700006 Max=3137.53 Min=5.38 Avg=8.74 90=9.71 99=47.17 99.9=60.90 99.99=231.29]
2025-01-04 19:00:26 15 sec: 1000000 operations; [INSERT: Count=1000000 Max=17678.33 Min=5.38 Avg=9.94 90=11.18 99=51.58 99.9=64.86 99.99=236.80]
Load runtime(sec): 15.5099
Load operations(ops): 1000000
Load throughput(ops/sec): 64474.8
2025-01-04 19:00:26 0 sec: 0 operations;
2025-01-04 19:00:36 10 sec: 756627 operations; [READ: Count=504681 Max=106627.07 Min=2.65 Avg=6.58 90=8.21 99=15.46 99.9=31.54 99.99=381.44] [UPDATE: Count=251946 Max=12558.33 Min=8.58 Avg=12.98 90=12.39 99=49.92 99.9=64.25 99.99=607.23]
2025-01-04 19:00:43 17 sec: 1499546 operations; [READ: Count=1000000 Max=106627.07 Min=2.65 Avg=7.05 90=9.30 99=17.73 99.9=32.90 99.99=381.44] [UPDATE: Count=499546 Max=12558.33 Min=8.58 Avg=13.82 90=15.13 99=52.86 99.9=74.81 99.99=490.75]
Run runtime(sec): 17.8002
Run operations(ops): 1000000
Run throughput(ops/sec): 56179.1
All steps completed successfully!
```



## 10fields*100field-len

workloada-1000000-10*100

```
  LD       ycsb
2025-01-04 19:07:55 0 sec: 0 operations;
2025-01-04 19:08:03 8 sec: 1000000 operations; [INSERT: Count=1000000 Max=1120927.74 Min=3.13 Avg=7.24 90=7.59 99=43.30 99.9=54.72 99.99=299.26]
Load runtime(sec): 8.75521
Load operations(ops): 1000000
Load throughput(ops/sec): 114218
2025-01-04 19:08:03 0 sec: 0 operations;
2025-01-04 19:08:12 8 sec: 906762 operations; [READ: Count=453258 Max=766.46 Min=1.86 Avg=5.06 90=6.69 99=13.55 99.9=30.24 99.99=376.57] [UPDATE: Count=453504 Max=5009.41 Min=5.16 Avg=10.20 90=11.49 99=48.41 99.9=62.17 99.99=507.39]
2025-01-04 19:08:13 9 sec: 1000000 operations; [READ: Count=500046 Max=766.46 Min=1.86 Avg=5.10 90=6.77 99=13.55 99.9=30.45 99.99=381.69] [UPDATE: Count=499954 Max=5038.08 Min=5.16 Avg=10.26 90=11.57 99=48.51 99.9=62.46 99.99=509.95]
Run runtime(sec): 9.66972
Run operations(ops): 1000000
Run throughput(ops/sec): 103416
All steps completed successfully!
```



workloadb-1000000-10*100

```
  LD       ycsb
2025-01-04 19:09:06 0 sec: 0 operations;
2025-01-04 19:09:14 7 sec: 1000000 operations; [INSERT: Count=1000000 Max=798.21 Min=3.10 Avg=5.56 90=6.67 99=19.09 99.9=48.83 99.99=292.10]
Load runtime(sec): 7.00701
Load operations(ops): 1000000
Load throughput(ops/sec): 142714
2025-01-04 19:09:14 0 sec: 0 operations;
2025-01-04 19:09:24 10 sec: 1000000 operations; [READ: Count=949924 Max=1135607.81 Min=1.74 Avg=6.00 90=5.94 99=14.43 99.9=28.77 99.99=333.31] [UPDATE: Count=50076 Max=1792.00 Min=5.20 Avg=27.38 90=40.83 99=51.10 99.9=92.61 99.99=722.94]
Run runtime(sec): 10.1361
Run operations(ops): 1000000
Run throughput(ops/sec): 98656.9
All steps completed successfully!
```



workloadc-1000000-10*100

```
  LD       ycsb
2025-01-04 19:10:12 0 sec: 0 operations;
2025-01-04 19:10:19 6 sec: 1000000 operations; [INSERT: Count=1000000 Max=1092.61 Min=3.04 Avg=5.33 90=6.34 99=20.30 99.9=45.53 99.99=279.55]
Load runtime(sec): 6.90373
Load operations(ops): 1000000
Load throughput(ops/sec): 144849
2025-01-04 19:10:19 0 sec: 0 operations;
2025-01-04 19:10:26 6 sec: 1000000 operations; [READ: Count=1000000 Max=945.66 Min=2.26 Avg=3.83 90=4.29 99=5.74 99.9=25.58 99.99=290.30]
Run runtime(sec): 6.64328
Run operations(ops): 1000000
Run throughput(ops/sec): 150528
All steps completed successfully!
```



workloadd-1000000-10*100

```
  LD       ycsb
2025-01-04 19:11:28 0 sec: 0 operations;
2025-01-04 19:11:35 6 sec: 1000000 operations; [INSERT: Count=1000000 Max=857.09 Min=3.12 Avg=5.38 90=6.36 99=19.49 99.9=46.78 99.99=275.97]
Load runtime(sec): 6.76046
Load operations(ops): 1000000
Load throughput(ops/sec): 147919
2025-01-04 19:11:35 0 sec: 0 operations;
2025-01-04 19:11:43 8 sec: 1000000 operations; [INSERT: Count=49406 Max=1587.20 Min=3.26 Avg=18.87 90=34.88 99=42.91 99.9=84.86 99.99=650.24] [READ: Count=950594 Max=571473.92 Min=1.66 Avg=4.37 90=5.46 99=12.81 99.9=24.85 99.99=303.36]
Run runtime(sec): 8.18069
Run operations(ops): 1000000
Run throughput(ops/sec): 122239
All steps completed successfully!
```



workloade-recordcount=100000-operationcount=10000-10*100

```
  LD       ycsb
2025-01-04 19:12:35 0 sec: 0 operations;
2025-01-04 19:12:36 0 sec: 100000 operations; [INSERT: Count=100000 Max=796.67 Min=3.03 Avg=4.16 90=4.82 99=11.15 99.9=28.77 99.99=204.16]
Load runtime(sec): 0.56313
Load operations(ops): 100000
Load throughput(ops/sec): 177579
2025-01-04 19:12:36 0 sec: 0 operations;
2025-01-04 19:12:46 9 sec: 2536 operations; [INSERT: Count=137 Max=128.96 Min=10.58 Avg=67.85 90=87.87 99=111.81 99.9=128.96 99.99=128.96] [SCAN: Count=2399 Max=8617.98 Min=46.98 Avg=4041.24 90=7028.73 99=8015.87 99.9=8495.10 99.99=8617.98]
2025-01-04 19:12:56 19 sec: 5001 operations; [INSERT: Count=251 Max=158.46 Min=10.58 Avg=69.64 90=91.65 99=111.81 99.9=158.46 99.99=158.46] [SCAN: Count=4750 Max=8617.98 Min=46.98 Avg=4132.25 90=7135.23 99=8101.89 99.9=8519.68 99.99=8617.98]
2025-01-04 19:13:06 29 sec: 7437 operations; [INSERT: Count=384 Max=158.46 Min=10.58 Avg=70.90 90=94.72 99=117.69 99.9=158.46 99.99=158.46] [SCAN: Count=7053 Max=9601.02 Min=46.98 Avg=4190.69 90=7208.96 99=8212.48 99.9=8617.98 99.99=8880.13]
2025-01-04 19:13:17 41 sec: 9792 operations; [INSERT: Count=486 Max=158.46 Min=10.58 Avg=72.01 90=95.87 99=119.61 99.9=158.46 99.99=158.46] [SCAN: Count=9306 Max=1111490.56 Min=46.98 Avg=4436.21 90=7266.30 99=8318.98 99.9=9363.45 99.99=690487.30]
2025-01-04 19:13:18 42 sec: 9999 operations; [INSERT: Count=494 Max=158.46 Min=10.58 Avg=71.99 90=96.06 99=119.61 99.9=158.46 99.99=158.46] [SCAN: Count=9505 Max=1111490.56 Min=46.98 Avg=4433.36 90=7266.30 99=8318.98 99.9=9011.20 99.99=690487.30]
Run runtime(sec): 42.3571
Run operations(ops): 10000
Run throughput(ops/sec): 236.088
All steps completed successfully!
```



workloadf-1000000-10*100

```
  LD       ycsb
2025-01-04 19:17:08 0 sec: 0 operations;
2025-01-04 19:17:16 7 sec: 1000000 operations; [INSERT: Count=1000000 Max=1778.69 Min=3.14 Avg=6.10 90=7.17 99=43.20 99.9=50.66 99.99=294.14]
Load runtime(sec): 7.54327
Load operations(ops): 1000000
Load throughput(ops/sec): 132568
2025-01-04 19:17:16 0 sec: 0 operations;
2025-01-04 19:17:26 9 sec: 1185556 operations; [READ: Count=790800 Max=888.83 Min=1.86 Avg=5.00 90=6.54 99=9.75 99.9=30.64 99.99=426.75] [UPDATE: Count=394756 Max=4460.54 Min=5.24 Avg=7.49 90=8.02 99=19.71 99.9=54.81 99.99=496.13]
2025-01-04 19:17:28 11 sec: 1499353 operations; [READ: Count=1000000 Max=888.83 Min=1.86 Avg=5.07 90=6.70 99=9.94 99.9=30.66 99.99=430.08] [UPDATE: Count=499353 Max=4460.54 Min=5.24 Avg=7.47 90=8.01 99=19.87 99.9=53.89 99.99=488.19]
Run runtime(sec): 11.9756
Run operations(ops): 1000000
Run throughput(ops/sec): 83503.2
All steps completed successfully!
```



## 20fields*1000field-len

workloada-1000000-20*1000

```
  LD       ycsb
2025-01-04 19:19:33 0 sec: 0 operations;
2025-01-04 19:19:43 10 sec: 224275 operations; [INSERT: Count=224275 Max=6361.09 Min=11.08 Avg=23.87 90=55.07 99=83.39 99.9=139.90 99.99=340.22]
2025-01-04 19:19:53 19 sec: 433660 operations; [INSERT: Count=433660 Max=145752.06 Min=11.04 Avg=25.42 90=57.25 99=85.38 99.9=147.46 99.99=333.31]
2025-01-04 19:20:03 29 sec: 631879 operations; [INSERT: Count=631879 Max=145752.06 Min=11.04 Avg=26.70 90=60.16 99=90.50 99.9=158.46 99.99=371.45]
2025-01-04 19:20:13 40 sec: 835326 operations; [INSERT: Count=835326 Max=145752.06 Min=11.04 Avg=27.09 90=60.83 99=100.03 99.9=182.01 99.99=387.07]
2025-01-04 19:20:22 48 sec: 1000000 operations; [INSERT: Count=1000000 Max=145752.06 Min=11.04 Avg=28.01 90=64.13 99=108.93 99.9=192.77 99.99=392.96]
Load runtime(sec): 48.8622
Load operations(ops): 1000000
Load throughput(ops/sec): 20465.7
2025-01-04 19:20:22 0 sec: 0 operations;
2025-01-04 19:20:32 10 sec: 127878 operations; [READ: Count=63694 Max=1661.95 Min=5.01 Avg=14.96 90=20.53 99=48.58 99.9=419.58 99.99=668.67] [UPDATE: Count=64184 Max=27803.65 Min=17.74 Avg=53.75 90=96.38 99=366.85 99.9=1023.49 99.99=4665.34]
2025-01-04 19:20:42 20 sec: 301221 operations; [READ: Count=149927 Max=39550.97 Min=5.01 Avg=28.45 90=25.44 99=328.96 99.9=532.48 99.99=1097.73] [UPDATE: Count=151294 Max=127860.74 Min=17.22 Avg=65.71 90=122.88 99=450.30 99.9=923.13 99.99=4661.25]
2025-01-04 19:20:52 29 sec: 417957 operations; [READ: Count=208353 Max=55345.15 Min=5.01 Avg=38.08 90=48.77 99=352.25 99.9=587.77 99.99=1338.37] [UPDATE: Count=209604 Max=127860.74 Min=17.22 Avg=76.78 90=227.33 99=475.65 99.9=919.55 99.99=3141.63]
2025-01-04 19:21:02 40 sec: 513391 operations; [READ: Count=256145 Max=124190.72 Min=5.01 Avg=46.00 90=195.97 99=369.15 99.9=636.41 99.99=1415.17] [UPDATE: Count=257246 Max=127860.74 Min=17.22 Avg=85.83 90=256.25 99=492.54 99.9=925.18 99.99=3141.63]
2025-01-04 19:21:12 50 sec: 594915 operations; [READ: Count=296776 Max=124190.72 Min=5.01 Avg=51.99 90=206.21 99=392.70 99.9=689.66 99.99=1394.69] [UPDATE: Count=298139 Max=127860.74 Min=17.22 Avg=94.88 90=268.03 99=536.06 99.9=1025.54 99.99=3102.72]
2025-01-04 19:21:22 60 sec: 675407 operations; [READ: Count=336952 Max=124190.72 Min=5.01 Avg=57.55 90=213.50 99=425.47 99.9=765.44 99.99=1584.13] [UPDATE: Count=338455 Max=127860.74 Min=17.22 Avg=100.97 90=276.48 99=563.20 99.9=1046.02 99.99=3141.63]
2025-01-04 19:21:32 70 sec: 746196 operations; [READ: Count=372195 Max=124190.72 Min=5.01 Avg=63.00 90=222.21 99=464.38 99.9=859.65 99.99=2740.22] [UPDATE: Count=374001 Max=127860.74 Min=17.22 Avg=106.89 90=287.49 99=601.09 99.9=1118.21 99.99=3358.72]
2025-01-04 19:21:42 80 sec: 816521 operations; [READ: Count=407324 Max=124190.72 Min=5.01 Avg=67.47 90=232.45 99=493.57 99.9=875.01 99.99=2480.13] [UPDATE: Count=409197 Max=127860.74 Min=17.22 Avg=111.92 90=300.54 99=623.62 99.9=1115.13 99.99=3235.84]
2025-01-04 19:21:52 90 sec: 878437 operations; [READ: Count=438184 Max=124190.72 Min=5.01 Avg=71.84 90=242.69 99=520.19 99.9=937.98 99.99=3137.53] [UPDATE: Count=440253 Max=127860.74 Min=17.22 Avg=117.37 90=313.86 99=655.87 99.9=1160.19 99.99=3852.29]
2025-01-04 19:22:02 100 sec: 941734 operations; [READ: Count=470067 Max=124190.72 Min=5.01 Avg=75.71 90=255.36 99=547.84 99.9=951.29 99.99=3069.95] [UPDATE: Count=471667 Max=127860.74 Min=17.22 Avg=121.76 90=326.91 99=678.91 99.9=1153.02 99.99=3704.83]
2025-01-04 19:22:11 109 sec: 999997 operations; [READ: Count=499268 Max=124190.72 Min=5.01 Avg=79.08 90=266.24 99=567.29 99.9=962.56 99.99=3067.90] [UPDATE: Count=500729 Max=127860.74 Min=17.22 Avg=125.22 90=336.89 99=696.32 99.9=1149.95 99.99=3624.96]
Run runtime(sec): 109.283
Run operations(ops): 1000000
Run throughput(ops/sec): 9150.56
All steps completed successfully!
```



workloadb-1000000-20*1000

```
  LD       ycsb
2025-01-04 19:31:11 0 sec: 0 operations;
2025-01-04 19:31:21 9 sec: 191338 operations; [INSERT: Count=191338 Max=17760.26 Min=11.26 Avg=30.27 90=59.49 99=164.74 99.9=370.94 99.99=1328.13]
2025-01-04 19:31:31 19 sec: 323427 operations; [INSERT: Count=323427 Max=22691.84 Min=11.26 Avg=39.54 90=86.91 99=279.55 99.9=680.96 99.99=3506.18]
2025-01-04 19:31:41 29 sec: 460689 operations; [INSERT: Count=460689 Max=22691.84 Min=11.26 Avg=42.95 90=100.73 99=270.33 99.9=662.53 99.99=3117.05]
2025-01-04 19:31:51 39 sec: 593711 operations; [INSERT: Count=593711 Max=22691.84 Min=11.26 Avg=45.25 90=103.49 99=276.99 99.9=656.38 99.99=3051.52]
2025-01-04 19:32:01 49 sec: 767460 operations; [INSERT: Count=767460 Max=22691.84 Min=11.26 Avg=43.14 90=100.03 99=251.39 99.9=607.23 99.99=2832.38]
2025-01-04 19:32:11 59 sec: 899517 operations; [INSERT: Count=899517 Max=52756.48 Min=11.26 Avg=44.57 90=104.96 99=249.34 99.9=592.89 99.99=2576.38]
2025-01-04 19:32:19 67 sec: 999998 operations; [INSERT: Count=999998 Max=1075838.98 Min=11.26 Avg=45.28 90=100.54 99=238.21 99.9=575.49 99.99=2424.83]
Load runtime(sec): 67.3709
Load operations(ops): 1000000
Load throughput(ops/sec): 14843.2
2025-01-04 19:32:19 0 sec: 0 operations;
2025-01-04 19:32:28 8 sec: 0 operations;
2025-01-04 19:32:38 18 sec: 534414 operations; [READ: Count=507700 Max=556.03 Min=4.62 Avg=14.11 90=23.28 99=55.65 99.9=80.38 99.99=256.64] [UPDATE: Count=26714 Max=10608.64 Min=19.54 Avg=67.87 90=98.88 99=140.54 99.9=289.28 99.99=2185.22]
2025-01-04 19:32:46 27 sec: 1000000 operations; [READ: Count=949979 Max=594.43 Min=4.30 Avg=14.06 90=21.82 99=56.38 99.9=91.01 99.99=262.14] [UPDATE: Count=50021 Max=43417.60 Min=18.48 Avg=72.00 90=103.61 99=203.52 99.9=686.59 99.99=4255.74]
Run runtime(sec): 27.2017
Run operations(ops): 1000000
Run throughput(ops/sec): 36762.4
All steps completed successfully!
```



workloadc-1000000-20*1000

```
  LD       ycsb
2025-01-04 19:38:33 0 sec: 0 operations;
2025-01-04 19:38:45 11 sec: 151017 operations; [INSERT: Count=151017 Max=1058537.47 Min=11.04 Avg=51.18 90=100.99 99=226.30 99.9=499.45 99.99=2117.63]
2025-01-04 19:38:55 21 sec: 271494 operations; [INSERT: Count=271494 Max=1086324.74 Min=11.04 Avg=59.62 90=110.85 99=235.52 99.9=576.00 99.99=3235.84]
2025-01-04 19:39:05 31 sec: 410419 operations; [INSERT: Count=410419 Max=1086324.74 Min=11.04 Avg=56.59 90=109.76 99=218.62 99.9=562.17 99.99=3299.33]
2025-01-04 19:39:15 41 sec: 551640 operations; [INSERT: Count=551640 Max=1086324.74 Min=11.04 Avg=54.78 90=107.84 99=214.27 99.9=553.98 99.99=3182.59]
2025-01-04 19:39:25 51 sec: 687584 operations; [INSERT: Count=687584 Max=1151336.45 Min=11.04 Avg=56.02 90=108.61 99=211.97 99.9=552.45 99.99=3235.84]
2025-01-04 19:39:35 61 sec: 843014 operations; [INSERT: Count=843014 Max=1151336.45 Min=11.04 Avg=53.94 90=106.30 99=229.63 99.9=633.34 99.99=3241.98]
2025-01-04 19:39:44 70 sec: 999999 operations; [INSERT: Count=999999 Max=1151336.45 Min=11.04 Avg=51.80 90=103.55 99=214.40 99.9=596.99 99.99=3002.37]
Load runtime(sec): 70.8471
Load operations(ops): 1000000
Load throughput(ops/sec): 14114.9
2025-01-04 19:39:44 0 sec: 0 operations;
2025-01-04 19:39:54 10 sec: 34396 operations; [READ: Count=34396 Max=578.56 Min=5.11 Avg=12.69 90=30.29 99=51.07 99.9=79.36 99.99=282.37]
2025-01-04 19:40:05 21 sec: 897734 operations; [READ: Count=897734 Max=1093664.77 Min=4.75 Avg=12.37 90=16.56 99=49.28 99.9=62.69 99.99=281.60]
2025-01-04 19:40:07 22 sec: 1000000 operations; [READ: Count=1000000 Max=1093664.77 Min=4.75 Avg=12.15 90=16.46 99=49.25 99.9=62.30 99.99=286.72]
Run runtime(sec): 22.2029
Run operations(ops): 1000000
Run throughput(ops/sec): 45039.2
All steps completed successfully!
```



workloadd-1000000-20*1000

```
  LD       ycsb
2025-01-04 19:41:29 0 sec: 0 operations;
2025-01-04 19:41:39 10 sec: 209255 operations; [INSERT: Count=209255 Max=911.87 Min=10.54 Avg=26.76 90=53.53 99=110.40 99.9=164.09 99.99=300.29]
2025-01-04 19:41:49 20 sec: 368509 operations; [INSERT: Count=368509 Max=488636.41 Min=10.54 Avg=33.23 90=74.75 99=124.80 99.9=232.57 99.99=1790.97]
2025-01-04 19:41:59 29 sec: 523810 operations; [INSERT: Count=523810 Max=488636.41 Min=10.54 Avg=36.20 90=81.15 99=128.38 99.9=250.75 99.99=2301.95]
2025-01-04 19:42:09 39 sec: 678223 operations; [INSERT: Count=678223 Max=488636.41 Min=10.54 Avg=37.92 90=82.56 99=129.53 99.9=256.38 99.99=2096.13]
2025-01-04 19:42:19 49 sec: 850642 operations; [INSERT: Count=850642 Max=488636.41 Min=10.54 Avg=37.74 90=82.56 99=131.58 99.9=258.69 99.99=1921.02]
2025-01-04 19:42:27 58 sec: 999999 operations; [INSERT: Count=999999 Max=488636.41 Min=10.54 Avg=37.39 90=82.05 99=129.66 99.9=247.81 99.99=1790.97]
Load runtime(sec): 58.4519
Load operations(ops): 1000000
Load throughput(ops/sec): 17108.1
2025-01-04 19:42:27 0 sec: 0 operations;
2025-01-04 19:42:37 10 sec: 74489 operations; [INSERT: Count=3782 Max=430.33 Min=12.12 Avg=49.56 90=68.86 99=91.45 99.9=312.83 99.99=430.33] [READ: Count=70707 Max=342.27 Min=4.36 Avg=9.78 90=13.94 99=49.38 99.9=58.98 99.99=255.36]
2025-01-04 19:42:48 21 sec: 845620 operations; [INSERT: Count=42114 Max=1076887.55 Min=12.12 Avg=76.05 90=65.98 99=96.83 99.9=317.69 99.99=2799.61] [READ: Count=803506 Max=4513.79 Min=4.16 Avg=9.59 90=13.91 99=48.51 99.9=59.58 99.99=303.62]
2025-01-04 19:42:49 22 sec: 999999 operations; [INSERT: Count=49860 Max=1076887.55 Min=12.12 Avg=75.23 90=79.81 99=170.11 99.9=354.56 99.99=2764.80] [READ: Count=950139 Max=4513.79 Min=4.16 Avg=9.60 90=13.94 99=47.26 99.9=59.65 99.99=301.06]
Run runtime(sec): 22.177
Run operations(ops): 1000000
Run throughput(ops/sec): 45091.7
All steps completed successfully!
```



workloade-recordcount=100000-operationcount=10000--20*1000

```
  LD       ycsb
2025-01-04 19:43:48 0 sec: 0 operations;
2025-01-04 19:43:52 4 sec: 99999 operations; [INSERT: Count=99999 Max=2764.80 Min=10.58 Avg=26.37 90=51.36 99=121.53 99.9=190.08 99.99=461.31]
Load runtime(sec): 4.73872
Load operations(ops): 100000
Load throughput(ops/sec): 21102.7
2025-01-04 19:43:52 0 sec: 0 operations;
2025-01-04 19:44:02 10 sec: 1592 operations; [INSERT: Count=82 Max=264.70 Min=17.89 Avg=127.32 90=227.71 99=261.63 99.9=264.70 99.99=264.70] [SCAN: Count=1510 Max=12984.32 Min=226.82 Avg=6301.33 90=9887.74 99=11730.94 99.9=12320.77 99.99=12984.32]
2025-01-04 19:44:12 20 sec: 3271 operations; [INSERT: Count=163 Max=751.10 Min=16.12 Avg=121.26 90=166.27 99=261.63 99.9=751.10 99.99=751.10] [SCAN: Count=3108 Max=14082.05 Min=115.46 Avg=6233.02 90=9740.29 99=11665.41 99.9=12427.26 99.99=14082.05]
2025-01-04 19:44:22 30 sec: 4967 operations; [INSERT: Count=261 Max=751.10 Min=16.12 Avg=121.51 90=196.22 99=261.63 99.9=751.10 99.99=751.10] [SCAN: Count=4706 Max=14082.05 Min=115.46 Avg=6208.06 90=9764.86 99=11730.94 99.9=12509.18 99.99=14082.05]
2025-01-04 19:44:32 40 sec: 6626 operations; [INSERT: Count=332 Max=751.10 Min=16.12 Avg=117.78 90=162.30 99=261.63 99.9=751.10 99.99=751.10] [SCAN: Count=6294 Max=16809.98 Min=115.46 Avg=6207.91 90=9773.06 99=11730.94 99.9=12509.18 99.99=14082.05]
2025-01-04 19:44:42 50 sec: 8317 operations; [INSERT: Count=415 Max=3520.51 Min=16.12 Avg=130.32 90=211.20 99=266.50 99.9=3520.51 99.99=3520.51] [SCAN: Count=7902 Max=16809.98 Min=85.82 Avg=6191.63 90=9805.82 99=11714.56 99.9=12509.18 99.99=14082.05]
2025-01-04 19:44:52 59 sec: 10000 operations; [INSERT: Count=502 Max=3520.51 Min=16.12 Avg=126.91 90=196.22 99=264.70 99.9=751.10 99.99=3520.51] [SCAN: Count=9498 Max=16809.98 Min=85.82 Avg=6185.89 90=9789.44 99=11681.79 99.9=12509.18 99.99=14082.05]
Run runtime(sec): 59.9708
Run operations(ops): 10000
Run throughput(ops/sec): 166.748
All steps completed successfully!
```



workloadf-1000000-20*1000

```
  LD       ycsb
2025-01-04 19:46:54 0 sec: 0 operations;
2025-01-04 19:47:04 10 sec: 189877 operations; [INSERT: Count=189877 Max=6098.94 Min=10.85 Avg=27.02 90=63.81 99=92.03 99.9=143.36 99.99=327.17]
2025-01-04 19:47:14 20 sec: 363623 operations; [INSERT: Count=363623 Max=13459.45 Min=10.85 Avg=29.19 90=71.94 99=94.78 99.9=158.72 99.99=423.42]
2025-01-04 19:47:24 29 sec: 554522 operations; [INSERT: Count=554522 Max=14860.29 Min=10.85 Avg=28.35 90=66.17 99=92.42 99.9=155.26 99.99=394.24]
2025-01-04 19:47:34 39 sec: 731276 operations; [INSERT: Count=731276 Max=14860.29 Min=10.85 Avg=28.80 90=67.39 99=98.43 99.9=161.28 99.99=382.72]
2025-01-04 19:47:45 51 sec: 907984 operations; [INSERT: Count=907984 Max=1051721.73 Min=10.85 Avg=30.33 90=68.54 99=101.06 99.9=170.50 99.99=390.91]
2025-01-04 19:47:50 56 sec: 999999 operations; [INSERT: Count=999999 Max=1051721.73 Min=10.85 Avg=30.19 90=67.65 99=101.06 99.9=171.01 99.99=393.98]
Load runtime(sec): 56.0834
Load operations(ops): 1000000
Load throughput(ops/sec): 17830.6
2025-01-04 19:47:50 0 sec: 0 operations;
2025-01-04 19:47:59 8 sec: 167513 operations; [READ: Count=111693 Max=473.60 Min=5.08 Avg=13.75 90=21.18 99=53.70 99.9=69.95 99.99=203.65] [UPDATE: Count=55820 Max=18677.76 Min=16.45 Avg=45.74 90=84.99 99=216.83 99.9=627.71 99.99=1250.30]
2025-01-04 19:48:09 18 sec: 420546 operations; [READ: Count=280365 Max=348389.38 Min=5.02 Avg=24.23 90=24.27 99=282.88 99.9=511.23 99.99=1306.62] [UPDATE: Count=140181 Max=18677.76 Min=16.45 Avg=50.14 90=99.97 99=216.83 99.9=542.72 99.99=1236.99]
2025-01-04 19:48:19 28 sec: 604393 operations; [READ: Count=403027 Max=348389.38 Min=5.01 Avg=33.87 90=34.46 99=342.01 99.9=703.49 99.99=3239.93] [UPDATE: Count=201366 Max=18677.76 Min=16.45 Avg=49.45 90=94.97 99=210.69 99.9=457.73 99.99=1097.73]
2025-01-04 19:48:29 38 sec: 757689 operations; [READ: Count=505380 Max=348389.38 Min=5.01 Avg=40.90 90=66.05 99=356.86 99.9=664.58 99.99=2928.64] [UPDATE: Count=252309 Max=18677.76 Min=16.45 Avg=50.49 90=92.99 99=209.02 99.9=432.38 99.99=1002.50]
2025-01-04 19:48:39 48 sec: 875514 operations; [READ: Count=583931 Max=348389.38 Min=4.69 Avg=48.02 90=195.33 99=382.46 99.9=791.55 99.99=5242.88] [UPDATE: Count=291583 Max=18677.76 Min=16.45 Avg=52.14 90=94.97 99=213.63 99.9=430.33 99.99=990.21]
2025-01-04 19:48:49 58 sec: 997411 operations; [READ: Count=665275 Max=348389.38 Min=4.69 Avg=53.20 90=206.34 99=406.27 99.9=779.26 99.99=4431.87] [UPDATE: Count=332136 Max=18677.76 Min=16.45 Avg=53.22 90=96.58 99=212.22 99.9=415.23 99.99=924.16]
2025-01-04 19:48:59 68 sec: 1111580 operations; [READ: Count=741456 Max=348389.38 Min=4.69 Avg=58.10 90=215.17 99=435.45 99.9=791.55 99.99=4032.51] [UPDATE: Count=370124 Max=18677.76 Min=16.45 Avg=53.56 90=95.30 99=210.69 99.9=398.85 99.99=906.75]
2025-01-04 19:49:09 78 sec: 1210026 operations; [READ: Count=807016 Max=348389.38 Min=4.69 Avg=63.19 90=221.69 99=465.66 99.9=913.92 99.99=5242.88] [UPDATE: Count=403010 Max=18677.76 Min=16.45 Avg=54.01 90=94.85 99=209.41 99.9=397.06 99.99=884.22]
2025-01-04 19:49:19 88 sec: 1313067 operations; [READ: Count=875717 Max=348389.38 Min=4.69 Avg=67.11 90=229.25 99=482.81 99.9=902.65 99.99=4853.76] [UPDATE: Count=437350 Max=18677.76 Min=16.45 Avg=54.50 90=94.46 99=209.15 99.9=391.94 99.99=848.89]
2025-01-04 19:49:29 98 sec: 1410443 operations; [READ: Count=940627 Max=348389.38 Min=4.69 Avg=70.84 90=237.69 99=502.01 99.9=909.31 99.99=4636.67] [UPDATE: Count=469816 Max=18677.76 Min=16.45 Avg=54.98 90=94.14 99=208.64 99.9=390.40 99.99=843.26]
2025-01-04 19:49:38 108 sec: 1499599 operations; [READ: Count=999997 Max=348389.38 Min=4.69 Avg=74.29 90=248.06 99=522.75 99.9=912.89 99.99=4431.87] [UPDATE: Count=499602 Max=18677.76 Min=16.45 Avg=55.39 90=94.02 99=208.51 99.9=387.07 99.99=833.53]
Run runtime(sec): 108.584
Run operations(ops): 1000000
Run throughput(ops/sec): 9209.49
All steps completed successfully!
```



# workload_getkeys

workload_getkeys-100000-10*500-getkeysbyfieldproportion=0.001

```
  LD       ycsb
2025-01-04 19:53:53 0 sec: 0 operations;
2025-01-04 19:53:54 1 sec: 100000 operations; [INSERT: Count=100000 Max=1814.53 Min=5.60 Avg=8.98 90=10.19 99=39.30 99.9=71.42 99.99=292.10]
Load runtime(sec): 1.46882
Load operations(ops): 100000
Load throughput(ops/sec): 68081.7
2025-01-04 19:53:54 0 sec: 0 operations;
2025-01-04 19:54:04 10 sec: 46086 operations; [READ: Count=46042 Max=79.04 Min=3.10 Avg=5.00 90=5.82 99=9.89 99.9=30.62 99.99=58.40] [GETKEYSBYFIELD: Count=44 Max=257556.48 Min=194641.92 Avg=212005.98 90=224002.05 99=257556.48 99.9=257556.48 99.99=257556.48]
2025-01-04 19:54:14 20 sec: 93080 operations; [READ: Count=92990 Max=79.04 Min=3.10 Avg=5.03 90=5.91 99=10.69 99.9=30.38 99.99=57.66] [GETKEYSBYFIELD: Count=90 Max=257556.48 Min=194641.92 Avg=213242.49 90=227409.92 99=244973.57 99.9=257556.48 99.99=257556.48]
2025-01-04 19:54:16 21 sec: 100000 operations; [READ: Count=99902 Max=79.04 Min=3.10 Avg=5.03 90=5.89 99=10.51 99.9=30.32 99.99=54.59] [GETKEYSBYFIELD: Count=98 Max=257556.48 Min=194641.92 Avg=212870.29 90=227278.85 99=244973.57 99.9=257556.48 99.99=257556.48]
Run runtime(sec): 21.661
Run operations(ops): 100000
Run throughput(ops/sec): 4616.6
All steps completed successfully!
```



workload_getkeys-100000-10*100-getkeysbyfieldproportion=0.001

```
  LD       ycsb
2025-01-04 19:57:03 0 sec: 0 operations;
2025-01-04 19:57:03 0 sec: 100000 operations; [INSERT: Count=100000 Max=775.68 Min=3.14 Avg=4.54 90=5.34 99=15.21 99.9=31.61 99.99=242.94]
Load runtime(sec): 0.605355
Load operations(ops): 100000
Load throughput(ops/sec): 165192
2025-01-04 19:57:03 0 sec: 0 operations;
2025-01-04 19:57:14 11 sec: 81386 operations; [READ: Count=81299 Max=216.83 Min=2.39 Avg=3.85 90=4.48 99=6.33 99.9=28.22 99.99=47.30] [GETKEYSBYFIELD: Count=87 Max=1167065.09 Min=97779.71 Avg=119777.21 90=111083.52 99=116654.08 99.9=1167065.09 99.99=1167065.09]
2025-01-04 19:57:16 12 sec: 100000 operations; [READ: Count=99896 Max=216.83 Min=2.26 Avg=3.80 90=4.42 99=6.24 99.9=27.70 99.99=46.98] [GETKEYSBYFIELD: Count=104 Max=1167065.09 Min=97779.71 Avg=116532.15 90=111083.52 99=116654.08 99.9=1167065.09 99.99=1167065.09]
Run runtime(sec): 12.737
Run operations(ops): 100000
Run throughput(ops/sec): 7851.13
All steps completed successfully!
```



workload_getkeys-100000-20*1000-getkeysbyfieldproportion=0.001

```
  LD       ycsb
2025-01-04 19:58:09 0 sec: 0 operations;
2025-01-04 19:58:14 4 sec: 100000 operations; [INSERT: Count=100000 Max=5672.96 Min=10.30 Avg=29.59 90=67.07 99=86.27 99.9=152.70 99.99=370.94]
Load runtime(sec): 4.93885
Load operations(ops): 100000
Load throughput(ops/sec): 20247.6
2025-01-04 19:58:14 0 sec: 0 operations;
2025-01-04 19:58:23 9 sec: 18226 operations; [READ: Count=18205 Max=60.90 Min=4.49 Avg=7.49 90=8.41 99=16.21 99.9=32.77 99.99=58.40] [GETKEYSBYFIELD: Count=21 Max=481558.53 Min=233046.02 Avg=447445.48 90=477102.08 99=481558.53 99.9=481558.53 99.99=481558.53]
2025-01-04 19:58:33 19 sec: 35562 operations; [READ: Count=35520 Max=60.90 Min=4.49 Avg=7.51 90=8.42 99=16.30 99.9=32.90 99.99=51.01] [GETKEYSBYFIELD: Count=42 Max=482082.82 Min=233046.02 Avg=450798.74 90=477888.51 99=482082.82 99.9=482082.82 99.99=482082.82]
2025-01-04 19:58:44 29 sec: 54955 operations; [READ: Count=54892 Max=60.90 Min=4.49 Avg=7.59 90=8.54 99=16.18 99.9=33.12 99.99=51.01] [GETKEYSBYFIELD: Count=63 Max=521404.41 Min=233046.02 Avg=457704.46 90=482082.82 99=507248.64 99.9=521404.41 99.99=521404.41]
2025-01-04 19:58:54 39 sec: 76867 operations; [READ: Count=76783 Max=324.35 Min=4.49 Avg=7.60 90=8.54 99=16.12 99.9=33.12 99.99=52.51] [GETKEYSBYFIELD: Count=84 Max=525598.72 Min=233046.02 Avg=463173.34 90=485228.54 99=521404.41 99.9=525598.72 99.99=525598.72]
2025-01-04 19:59:02 48 sec: 100000 operations; [READ: Count=99898 Max=324.35 Min=4.49 Avg=7.63 90=8.55 99=17.30 99.9=33.25 99.99=52.61] [GETKEYSBYFIELD: Count=102 Max=525598.72 Min=233046.02 Avg=464891.18 90=485228.54 99=521404.41 99.9=525598.72 99.99=525598.72]
Run runtime(sec): 48.4727
Run operations(ops): 100000
Run throughput(ops/sec): 2063.02
All steps completed successfully!
```



workload_getkeys-100000-20*1000-getkeysbyfieldproportion=0.01

```
  LD       ycsb
2025-01-04 20:00:44 0 sec: 0 operations;
2025-01-04 20:00:49 4 sec: 100000 operations; [INSERT: Count=100000 Max=7639.04 Min=10.52 Avg=22.76 90=52.67 99=80.19 99.9=130.05 99.99=312.83]
Load runtime(sec): 4.34701
Load operations(ops): 100000
Load throughput(ops/sec): 23004.3
2025-01-04 20:00:49 0 sec: 0 operations;
2025-01-04 20:01:00 11 sec: 2702 operations; [READ: Count=2681 Max=57.89 Min=4.36 Avg=8.17 90=9.26 99=27.23 99.9=40.13 99.99=57.89] [GETKEYSBYFIELD: Count=21 Max=1519386.62 Min=401342.46 Avg=511511.60 90=475791.36 99=1519386.62 99.9=1519386.62 99.99=1519386.62]
2025-01-04 20:01:10 21 sec: 5261 operations; [READ: Count=5218 Max=57.89 Min=4.36 Avg=8.23 90=9.42 99=28.05 99.9=39.62 99.99=48.93] [GETKEYSBYFIELD: Count=43 Max=1519386.62 Min=401342.46 Avg=479909.46 90=466092.03 99=1519386.62 99.9=1519386.62 99.99=1519386.62]
2025-01-04 20:01:20 31 sec: 7645 operations; [READ: Count=7579 Max=57.89 Min=4.36 Avg=8.23 90=9.45 99=28.03 99.9=39.20 99.99=48.93] [GETKEYSBYFIELD: Count=66 Max=1519386.62 Min=401342.46 Avg=467283.60 90=464519.17 99=731381.76 99.9=1519386.62 99.99=1519386.62]
2025-01-04 20:01:31 42 sec: 10489 operations; [READ: Count=10401 Max=57.89 Min=4.36 Avg=8.18 90=9.41 99=27.61 99.9=39.62 99.99=48.93] [GETKEYSBYFIELD: Count=88 Max=1519386.62 Min=401342.46 Avg=476205.43 90=466092.03 99=1503657.98 99.9=1519386.62 99.99=1519386.62]
2025-01-04 20:01:41 52 sec: 12537 operations; [READ: Count=12426 Max=87.17 Min=4.36 Avg=8.21 90=9.42 99=28.03 99.9=40.13 99.99=57.89] [GETKEYSBYFIELD: Count=111 Max=1519386.62 Min=401342.46 Avg=469867.14 90=466092.03 99=1503657.98 99.9=1519386.62 99.99=1519386.62]
2025-01-04 20:01:51 62 sec: 14592 operations; [READ: Count=14459 Max=87.17 Min=4.36 Avg=8.22 90=9.45 99=27.76 99.9=41.22 99.99=57.89] [GETKEYSBYFIELD: Count=133 Max=1519386.62 Min=368312.32 Avg=465253.37 90=464519.17 99=1503657.98 99.9=1519386.62 99.99=1519386.62]
2025-01-04 20:02:01 72 sec: 17161 operations; [READ: Count=17005 Max=87.17 Min=4.36 Avg=8.21 90=9.42 99=27.76 99.9=40.13 99.99=48.93] [GETKEYSBYFIELD: Count=156 Max=1519386.62 Min=368312.32 Avg=462632.07 90=464519.17 99=731381.76 99.9=1519386.62 99.99=1519386.62]
2025-01-04 20:02:11 82 sec: 19587 operations; [READ: Count=19409 Max=92.67 Min=4.36 Avg=8.21 90=9.42 99=27.86 99.9=40.22 99.99=57.89] [GETKEYSBYFIELD: Count=178 Max=1519386.62 Min=368312.32 Avg=461114.24 90=464519.17 99=731381.76 99.9=1519386.62 99.99=1519386.62]
2025-01-04 20:02:21 92 sec: 22250 operations; [READ: Count=22050 Max=92.67 Min=4.36 Avg=8.22 90=9.43 99=27.68 99.9=40.22 99.99=57.89] [GETKEYSBYFIELD: Count=200 Max=1519386.62 Min=368312.32 Avg=459565.96 90=464519.17 99=731381.76 99.9=1519386.62 99.99=1519386.62]
2025-01-04 20:02:32 103 sec: 24009 operations; [READ: Count=23788 Max=92.67 Min=4.36 Avg=8.29 90=9.56 99=28.02 99.9=40.22 99.99=57.89] [GETKEYSBYFIELD: Count=221 Max=1551892.48 Min=368312.32 Avg=466631.74 90=468975.61 99=1503657.98 99.9=1551892.48 99.99=1551892.48]
2025-01-04 20:02:42 113 sec: 27347 operations; [READ: Count=27104 Max=92.67 Min=4.36 Avg=8.24 90=9.53 99=27.86 99.9=39.97 99.99=48.93] [GETKEYSBYFIELD: Count=243 Max=1551892.48 Min=368312.32 Avg=465637.86 90=470024.19 99=1503657.98 99.9=1551892.48 99.99=1551892.48]
2025-01-04 20:02:52 123 sec: 29917 operations; [READ: Count=29653 Max=92.67 Min=4.36 Avg=8.25 90=9.54 99=27.73 99.9=39.62 99.99=48.93] [GETKEYSBYFIELD: Count=264 Max=1551892.48 Min=368312.32 Avg=464912.88 90=470024.19 99=731381.76 99.9=1551892.48 99.99=1551892.48]
2025-01-04 20:03:02 133 sec: 32187 operations; [READ: Count=31900 Max=92.67 Min=4.36 Avg=8.26 90=9.54 99=27.95 99.9=39.39 99.99=48.93] [GETKEYSBYFIELD: Count=287 Max=1551892.48 Min=368312.32 Avg=464023.20 90=470286.34 99=731381.76 99.9=1551892.48 99.99=1551892.48]
2025-01-04 20:03:14 144 sec: 34440 operations; [READ: Count=34131 Max=92.67 Min=4.36 Avg=8.27 90=9.56 99=28.00 99.9=39.36 99.99=51.33] [GETKEYSBYFIELD: Count=309 Max=1626341.38 Min=368312.32 Avg=467004.45 90=470810.62 99=1503657.98 99.9=1626341.38 99.99=1626341.38]
2025-01-04 20:03:24 154 sec: 37072 operations; [READ: Count=36741 Max=92.67 Min=4.36 Avg=8.27 90=9.56 99=28.00 99.9=39.39 99.99=51.33] [GETKEYSBYFIELD: Count=331 Max=1626341.38 Min=368312.32 Avg=466155.79 90=471334.91 99=1503657.98 99.9=1626341.38 99.99=1626341.38]
2025-01-04 20:03:34 164 sec: 39067 operations; [READ: Count=38715 Max=92.67 Min=4.36 Avg=8.28 90=9.57 99=28.02 99.9=39.39 99.99=51.33] [GETKEYSBYFIELD: Count=352 Max=1626341.38 Min=368312.32 Avg=466006.76 90=471859.20 99=731381.76 99.9=1626341.38 99.99=1626341.38]
2025-01-04 20:03:44 174 sec: 40397 operations; [READ: Count=40022 Max=92.67 Min=4.36 Avg=8.29 90=9.57 99=28.19 99.9=39.39 99.99=51.33] [GETKEYSBYFIELD: Count=375 Max=1626341.38 Min=368312.32 Avg=464223.47 90=471334.91 99=731381.76 99.9=1626341.38 99.99=1626341.38]
2025-01-04 20:03:54 184 sec: 43244 operations; [READ: Count=42846 Max=92.67 Min=4.36 Avg=8.27 90=9.54 99=28.00 99.9=39.20 99.99=51.33] [GETKEYSBYFIELD: Count=398 Max=1626341.38 Min=368312.32 Avg=462590.30 90=470286.34 99=731381.76 99.9=1626341.38 99.99=1626341.38]
2025-01-04 20:04:05 196 sec: 45396 operations; [READ: Count=44977 Max=92.67 Min=4.36 Avg=8.29 90=9.58 99=28.02 99.9=39.13 99.99=51.33] [GETKEYSBYFIELD: Count=419 Max=1626341.38 Min=368312.32 Avg=466311.63 90=473694.21 99=1503657.98 99.9=1626341.38 99.99=1626341.38]
2025-01-04 20:04:15 206 sec: 48252 operations; [READ: Count=47809 Max=92.67 Min=4.36 Avg=8.26 90=9.55 99=27.86 99.9=38.40 99.99=50.98] [GETKEYSBYFIELD: Count=443 Max=1626341.38 Min=368312.32 Avg=464403.78 90=472121.34 99=1503657.98 99.9=1626341.38 99.99=1626341.38]
2025-01-04 20:04:25 216 sec: 50152 operations; [READ: Count=49687 Max=92.67 Min=4.36 Avg=8.26 90=9.54 99=27.89 99.9=38.72 99.99=51.33] [GETKEYSBYFIELD: Count=465 Max=1626341.38 Min=368312.32 Avg=463348.26 90=471859.20 99=786432.00 99.9=1626341.38 99.99=1626341.38]
2025-01-04 20:04:35 226 sec: 52455 operations; [READ: Count=51968 Max=92.67 Min=4.36 Avg=8.28 90=9.56 99=27.95 99.9=39.20 99.99=64.06] [GETKEYSBYFIELD: Count=487 Max=1626341.38 Min=368312.32 Avg=463702.32 90=472121.34 99=786432.00 99.9=1626341.38 99.99=1626341.38]
2025-01-04 20:04:47 237 sec: 55353 operations; [READ: Count=54845 Max=92.67 Min=4.36 Avg=8.27 90=9.56 99=27.93 99.9=39.13 99.99=64.06] [GETKEYSBYFIELD: Count=508 Max=1626341.38 Min=368312.32 Avg=465966.38 90=473694.21 99=1503657.98 99.9=1585446.91 99.99=1626341.38]
2025-01-04 20:04:57 247 sec: 56935 operations; [READ: Count=56404 Max=92.67 Min=4.36 Avg=8.28 90=9.57 99=28.03 99.9=39.39 99.99=57.89] [GETKEYSBYFIELD: Count=531 Max=1626341.38 Min=368312.32 Avg=465235.99 90=472907.78 99=1503657.98 99.9=1585446.91 99.99=1626341.38]
2025-01-04 20:05:07 258 sec: 59616 operations; [READ: Count=59063 Max=92.67 Min=4.36 Avg=8.28 90=9.57 99=28.00 99.9=39.36 99.99=57.89] [GETKEYSBYFIELD: Count=553 Max=1626341.38 Min=368312.32 Avg=465523.42 90=472907.78 99=840433.66 99.9=1585446.91 99.99=1626341.38]
2025-01-04 20:05:17 268 sec: 61662 operations; [READ: Count=61087 Max=92.67 Min=4.36 Avg=8.28 90=9.57 99=27.98 99.9=39.20 99.99=57.89] [GETKEYSBYFIELD: Count=575 Max=1626341.38 Min=368312.32 Avg=464981.23 90=472907.78 99=840433.66 99.9=1585446.91 99.99=1626341.38]
2025-01-04 20:05:27 278 sec: 64525 operations; [READ: Count=63928 Max=92.67 Min=4.36 Avg=8.27 90=9.56 99=27.93 99.9=39.13 99.99=57.89] [GETKEYSBYFIELD: Count=597 Max=1626341.38 Min=368312.32 Avg=464326.62 90=471859.20 99=840433.66 99.9=1585446.91 99.99=1626341.38]
2025-01-04 20:05:37 288 sec: 67644 operations; [READ: Count=67025 Max=92.67 Min=4.36 Avg=8.26 90=9.55 99=27.87 99.9=38.78 99.99=57.89] [GETKEYSBYFIELD: Count=619 Max=1626341.38 Min=368312.32 Avg=463931.57 90=471859.20 99=840433.66 99.9=1585446.91 99.99=1626341.38]
2025-01-04 20:05:47 298 sec: 69573 operations; [READ: Count=68932 Max=92.67 Min=4.36 Avg=8.27 90=9.55 99=27.93 99.9=38.72 99.99=57.89] [GETKEYSBYFIELD: Count=641 Max=1626341.38 Min=368312.32 Avg=463528.46 90=471859.20 99=840433.66 99.9=1585446.91 99.99=1626341.38]
2025-01-04 20:05:57 308 sec: 71085 operations; [READ: Count=70425 Max=9871.36 Min=4.36 Avg=8.44 90=9.60 99=28.08 99.9=39.39 99.99=66.50] [GETKEYSBYFIELD: Count=660 Max=1626341.38 Min=368312.32 Avg=465232.72 90=472907.78 99=840433.66 99.9=1585446.91 99.99=1626341.38]
2025-01-04 20:07:46 417 sec: 72971 operations; [READ: Count=72290 Max=9871.36 Min=4.36 Avg=8.44 90=9.61 99=28.16 99.9=39.62 99.99=66.50] [GETKEYSBYFIELD: Count=681 Max=99857989.63 Min=368312.32 Avg=611465.89 90=475791.36 99=1233125.38 99.9=1626341.38 99.99=99857989.63]
2025-01-04 20:07:56 427 sec: 74581 operations; [READ: Count=73878 Max=9871.36 Min=4.36 Avg=8.45 90=9.62 99=28.25 99.9=39.62 99.99=66.50] [GETKEYSBYFIELD: Count=703 Max=99857989.63 Min=368312.32 Avg=606942.97 90=475791.36 99=1233125.38 99.9=1626341.38 99.99=99857989.63]
2025-01-04 20:08:06 437 sec: 77395 operations; [READ: Count=76671 Max=9871.36 Min=4.36 Avg=8.44 90=9.62 99=28.18 99.9=39.20 99.99=64.06] [GETKEYSBYFIELD: Count=724 Max=99857989.63 Min=368312.32 Avg=602976.10 90=476577.79 99=1233125.38 99.9=1626341.38 99.99=99857989.63]
2025-01-04 20:08:16 447 sec: 79723 operations; [READ: Count=78978 Max=9871.36 Min=4.36 Avg=8.45 90=9.62 99=28.25 99.9=39.39 99.99=64.06] [GETKEYSBYFIELD: Count=745 Max=99857989.63 Min=368312.32 Avg=599103.37 90=477364.22 99=1233125.38 99.9=1626341.38 99.99=99857989.63]
2025-01-04 20:08:26 457 sec: 81379 operations; [READ: Count=80612 Max=9871.36 Min=4.36 Avg=8.45 90=9.63 99=28.32 99.9=39.20 99.99=64.06] [GETKEYSBYFIELD: Count=767 Max=99857989.63 Min=368312.32 Avg=595160.01 90=477364.22 99=840433.66 99.9=1626341.38 99.99=99857989.63]
2025-01-04 20:08:36 467 sec: 83499 operations; [READ: Count=82710 Max=9871.36 Min=4.36 Avg=8.45 90=9.64 99=28.34 99.9=39.36 99.99=64.06] [GETKEYSBYFIELD: Count=789 Max=99857989.63 Min=368312.32 Avg=591353.84 90=477364.22 99=840433.66 99.9=1626341.38 99.99=99857989.63]
2025-01-04 20:08:46 477 sec: 85503 operations; [READ: Count=84693 Max=9871.36 Min=4.36 Avg=8.46 90=9.64 99=28.37 99.9=39.36 99.99=64.06] [GETKEYSBYFIELD: Count=810 Max=99857989.63 Min=368312.32 Avg=588195.79 90=477364.22 99=840433.66 99.9=1626341.38 99.99=99857989.63]
2025-01-04 20:08:56 487 sec: 87519 operations; [READ: Count=86687 Max=9871.36 Min=4.36 Avg=8.46 90=9.65 99=28.37 99.9=39.39 99.99=63.62] [GETKEYSBYFIELD: Count=832 Max=99857989.63 Min=368312.32 Avg=584765.12 90=477364.22 99=840433.66 99.9=1626341.38 99.99=99857989.63]
2025-01-04 20:09:06 497 sec: 89323 operations; [READ: Count=88470 Max=9871.36 Min=4.36 Avg=8.46 90=9.65 99=28.38 99.9=39.62 99.99=63.62] [GETKEYSBYFIELD: Count=853 Max=99857989.63 Min=368312.32 Avg=581671.57 90=477102.08 99=786432.00 99.9=1626341.38 99.99=99857989.63]
2025-01-04 20:09:16 507 sec: 91520 operations; [READ: Count=90645 Max=9871.36 Min=4.36 Avg=8.46 90=9.65 99=28.45 99.9=39.74 99.99=63.62] [GETKEYSBYFIELD: Count=875 Max=99857989.63 Min=368312.32 Avg=578677.49 90=477102.08 99=786432.00 99.9=1626341.38 99.99=99857989.63]
2025-01-04 20:09:26 517 sec: 93620 operations; [READ: Count=92723 Max=9871.36 Min=4.36 Avg=8.47 90=9.66 99=28.50 99.9=39.62 99.99=63.62] [GETKEYSBYFIELD: Count=897 Max=99857989.63 Min=368312.32 Avg=575807.77 90=477102.08 99=786432.00 99.9=1626341.38 99.99=99857989.63]
2025-01-04 20:09:36 527 sec: 95578 operations; [READ: Count=94659 Max=9871.36 Min=4.36 Avg=8.47 90=9.67 99=28.51 99.9=39.39 99.99=63.62] [GETKEYSBYFIELD: Count=919 Max=99857989.63 Min=368312.32 Avg=572898.60 90=476839.93 99=786432.00 99.9=1626341.38 99.99=99857989.63]
2025-01-04 20:09:46 537 sec: 97298 operations; [READ: Count=96358 Max=9871.36 Min=4.36 Avg=8.48 90=9.68 99=28.54 99.9=39.39 99.99=61.92] [GETKEYSBYFIELD: Count=940 Max=99857989.63 Min=368312.32 Avg=570331.36 90=476839.93 99=786432.00 99.9=1626341.38 99.99=99857989.63]
2025-01-04 20:09:56 547 sec: 99746 operations; [READ: Count=98784 Max=9871.36 Min=4.36 Avg=8.48 90=9.69 99=28.59 99.9=39.74 99.99=63.62] [GETKEYSBYFIELD: Count=962 Max=99857989.63 Min=368312.32 Avg=568057.87 90=477364.22 99=756023.30 99.9=1626341.38 99.99=99857989.63]
2025-01-04 20:09:58 549 sec: 100000 operations; [READ: Count=99034 Max=9871.36 Min=4.36 Avg=8.48 90=9.69 99=28.62 99.9=39.74 99.99=63.62] [GETKEYSBYFIELD: Count=966 Max=99857989.63 Min=368312.32 Avg=567599.02 90=477364.22 99=756023.30 99.9=1626341.38 99.99=99857989.63]
Run runtime(sec): 549.445
Run operations(ops): 100000
Run throughput(ops/sec): 182.002
All steps completed successfully!
```





