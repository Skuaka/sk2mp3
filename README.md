# sk2mp3

想把文件上传到网易音乐云盘，但是发现只修改文件后缀名是没用的，云盘会检测出文件格式不正确。

为此编写了一个编码程序，把文件内容编码进mp3的每一帧里面去。

转换速度和你电脑上的文件复制速度几乎一样，生成的mp3文件大小会比原文件略微增大一点点。

### 安装

在目录下自己编译

```bash
gcc -o sk2mp3_encode sk2mp3_encode.c
gcc -o sk2mp3_decode sk2mp3_decode.c
```

### 功能

#### sk2mp3_encode

把任意文件编码为 mp3 格式的文件。

usage: `sk2mp3_encode [file] [title] [author]`

example output:

```bash
myMBP:sk2mp3 skuaka$ ./sk2mp3_encode test.pdf tit aut
File test.pdf opened.
File test_sk_e.mp3 created.
Start covering (3 steps in total)
Step 1 ...
Step 2 ...
(This process may take a long time, please wait)
Step 3 ...
Completed!
```

#### sk2mp3_decode

把编码后的文件解码为原文件。

usage: `sk2mp3_decode [xxx.mp3]`



PS：如果多年以后有空，添加压缩算法。