# phone.c
同じWifiにつながっていれば電話ができます。

まずsoxをインストールしてください。
次にlibsoxもインストールしてください。
`gcc -o phone phone.c -lsox`としてコンパイルしてください。

サーバー側はターミナルなどで`./phone | play -t raw -c 1 -e s -b 16 -r 44100 -`と打ってください。
`I'M SERVER`という文字とポート番号が出ます。
クライアント側は`rec -t raw -c 1 -e s -b 16 -r 44100 - | ./phone <クライアント側のIPアドレス> <ポート番号>| play -t raw -c 1 -e s -b 16 -r 44100 -`と打ってください。
電話が始まります。
