# thermalview
temperature viewer using Lepton3.5 and PureThermal 2

[Lepton3.5](https://www.flir.jp/products/lepton/) と [PureThermal 2](https://groupgets.com/manufacturers/getlab/products/purethermal-2-flir-lepton-smart-i-o-module) を使用したサーマルカメラのプログラムです。
グラフィック表示には[EGGX/ProCAL](https://www.ir.isas.jaxa.jp/~cyamauch/eggx_procall/index.ja.html)ライブラリを使用しています。
Raspberry Pi 4で動作確認を行っています。[Raspberry Pi 4はアップデートが必要です。](https://www.raspberrypi.org/forums/viewtopic.php?t=273027)

## ビルド

### V4Lのライブラリのインストール
```
$ sudo apt-get install libv4l-dev v4l-utils
```

### EGGX/ProCALライブラリのインストール
```
$ sudo apt install libx11-dev
$ git clone https://github.com/cyamauch/eggx.git
$ cd eggx/src
$ ln -sf Makefile.linux Makefile
$ make
$ sudo make install
```
### thermalviewのビルド
```
$ git clone https://github.com/namikata2020/thermalview.git
$ cd thermalview
$ make
```

### 実行
```
$ ./thermalview
```
![スクリーンショット](https://github.com/namikata2020/thermalview/blob/master/scrimg.png)

## 参考
[PureThermal UVC Capture Examples](https://github.com/groupgets/purethermal1-uvc-capture)
