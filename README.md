1. プログラムのファイル構成
◎無線型, 有線併用型
・ネットワーク制御(model/kitagawa-routing-protocol.cc, model/kitagawa-routing-protocol.h)
・パケットフォーマット (model/kitagawa-packet.cc, model/kitagawa-packet.h)
・シナリオファイル(example/wireless-sumo.cc, example/wired-sumo.cc)


2. 実行手順
◎前準備(Windows, Macでも同様)
  ・バーチャルマシンをダウンロード, インストール(今回はVirtualBox)
  ・VirtualBox 上でUbuntu16.04ダウンロード, インストール
・ほとんど説明されている通りにダウンロードすればよいが, メモリの容量を30GBくらいはUbuntuの方に持っていく
　・Ubuntu上でSimulation of Urban Mobility(SUMO)をダウンロード, インストール
    ・SUMOのダウンロード方法
　　　①ubuntu上でsumoをダウンロード・インストール
・sudo apt update
・sudo apt-get install cmake python g++ libxerces-c-dev libfox-1.6-dev libgdal-dev libproj-dev libgl2ps-dev swig
・sudo apt install git
・git clone —recursive https://github.com/eclipse/sumo
・export SUMO_HOME=$PWD/sumo
・mkdir sumo/build/cmake-build && cd sumo/build/cmake-build
・cmake ../..
・make -j8(時間長い)
・gedit .bashrc
・# forexampleの下に
export PATH=$PATH/home/kitagawa/sumo/bin
export SUMO_HOME=/home/kitagawa/sumo
・source .bashrc

　　・モビリティの作成方法(マンハッタン型) & SUMO->NS3変換方法
　　　　②sumoでモビリティファイルを作る
・cd sumo/tools
・netgenerate --grid \
--grid.length 50 \(各ブロックの長さ 50×50)
--grid.number 5 \(ブロック数 5×5)
--default.lanenumber 2 \ 
--default.speed 11.111(11.111m/s=40km/h)
--tls.guess \(信号の有無)
--output-file manhattan.net.xml
・randomTrips.pyまでディレクトリ移動
python randomTrips.py \
-n manhattan.net.xml \
-b 0 \(開始時間)
-e 20 \(終了時間)
-p 0.5 \(出発間隔)
-o manhattan.trip.xml
・duarouter \
-n manhattan.net.xml \
-t manhattan.trip.xml \
-o manhattan.rou.xml
・manhattan.sumocfg作成(XML形式)
<?xml version=“1.0” encoding=“iso-8859-1”?>
<configuration xmins:xsi=“http://www.w3.org/2001/XMLSchema-
nstance” xs〜(まだ先ある)>
<input>
<net-file value=“manhattan.net.xml”/>
<route-files value=“manhattan.rou.xml”/>
</input>
<time>
<begin value=“0”/>
<end value=“20”/>
</time>
</configuration>

　　　　　③sumocfgからtrace.xmlへ
・ns2-mobility-trace.ccをscratchへコピー
・cd
・cd sumo/tools/manhattan
・sumo -c manhattan.sumocfg --fcd-output trace.xml
④trace.xmlからmobility.tclへ
・cd ..
・python traceExporter.py -i manhattan/trace.xml --ns2mobility-output=mobility.tcl
・mobility.tclをmanhattanファイルの中に移動
・gedit mobility.tcl(ノードの数を確認)
・mobility.tclをhome/kitagawaに移動

　・Ubuntu上でNS3をダウンロード, インストール
　　・できるだけ新しいバージョンを選ぶ(今回は3.29ver)
    ・Ubuntuのバージョンによってサイトで調べた通りにできない可能性があるのでサイトの通り(UbuntuのバージョンとNS3のバージョン)のバージョンをダウンロードする

◎実行の仕方
1, VirtualBox 上でUbuntuを起動
2, 端末でホームディレクトリからダウンロードしたNS3までディレクトリを移動させる
3, シナリオファイルをexampleからNS3下のscratchへ
4, scratchにシナリオファイルが入っていることを確認し, 実行する

実行例)・cd ns-3.29
・./waf --run “scratch/ns2-mobility-trace --traceFile=/home/kitagawa/mobility.tcl --nodeNum=44 --duration=100.0 --logFile=ns2-mob.log”
※	ディレクトリを合わせる＆nodeNum(ノード数)が作成したノード数としっかり合っていないとエラーがでる

◎youtubeに詳細があるので参考に
https://www.youtube.com/watch?v=PAAY1zvHeoE&list=LL&index=6
https://www.youtube.com/watch?v=9Tt5GNMuOpc&list=LL&index=7
