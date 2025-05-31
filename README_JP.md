## [English](/README.md)
## [简体中文](/README_CN.md)

# **thprac**
> thpracは東方STGの為の練習ツールです。

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/V7V7O03J4)

## [ダウンロード](https://github.com/touhouworldcup/thprac/releases/latest) - [ベータ版ダウンロード](https://nightly.link/touhouworldcup/thprac/workflows/main/master/thprac.zip)
> ベータ版は次の安定版へのみアップデートされ、ベータ版からベータ版へアップデートすることに出来ないことに注意してください。

## **目次**
* [ダウンロード](#downloading)
* [使い方](#usage)
* [互換性](#compatability)
* [機能](#features)
* [Backspace メニュー](#backspace-menu)
* [拡張オプション](#advanced-options)
* [ゲーム個別の機能](#game-specific-features)
* [クレジット](#credits)
* [FAQ](#faq)

## <a name="downloading"></a> **ダウンロード**
ツールをダウンロードしたい場合は[最新版](https://github.com/touhouworldcup/thprac/releases/latest)をダウンロードするか、[最新ベータ版](https://nightly.link/touhouworldcup/thprac/workflows/main/master/thprac.zip)をダウンロードしてください。最新ベータ版は次の安定版へアップデートされ、ベータ版からベータ版へアップデートされないことに注意してください。

## <a name="usage"></a> **使い方**
このツールは様々な方法で使用することができます。主な使い方としては **実行ファイルをゲームと同じフォルダに配置する** 、 **実行ファイルをゲーム実行後に起動する** 、 **thprac のランチャーを使用する** 、があります。

### **thprac をゲームフォルダに配置する**
**この方法は Steam で購入したゲームでは動作しません。** ゲームと同じフォルダに `thprac.exe` を配置することで thprac を動作させることができます。この方法はvpatchも自動的に検出します。ただしゲーム自体が最新バージョンであり、ゲームの実行ファイル名が以下の形式になっている必要があります。この方法はvpatchでも動作します。

- thXX.exe (多くの場合)
- 東方紅魔郷.exe (東方紅魔郷の場合)
- alcostg.exe (黄昏酒場の場合)

### **thprac をゲーム実行後に起動する**
**この方法であれば、大抵問題ありません。** まず、任意の方法(thcrap, vpatch, Steamなど)でゲームを実行してください。その後 `thprac.exe` を起動してください。 thprac は実行中のゲームを検出し、そのゲームに接続しようと試みます。 thprac の機能を使うにはメニュー画面に戻る必要がある場合があります。

### **thprac のランチャーを使用する**
この動画でランチャーの使用方法を説明しています。ただし英語で説明されているので注意してください。
[![A quick and dirty overview of thprac 2.0](https://i.ytimg.com/vi/sRV4PDQceYo/maxresdefault.jpg)](https://www.youtube.com/watch?v=sRV4PDQceYo)

### コマンドライン
以下のコマンドラインオプションがサポートされています。
- `<path to game exe>` `thprac.exe <path to game exe>` コマンドはゲームを実行したうえで確認メッセージ無しで thprac は自動的にゲームに接続します。これにより東方のゲーム実行ファイルに `thprac.exe` をドラッグ&ドロップすることで thprac が有効な状態で起動することができます。
- `--attach <pid>` プロセスIDが `<pid>` のゲームに確認メッセージ無しで thprac を即座に適用します。
- `--attach` (フラグ無し) `thprac.exe --attach` コマンドは最初に検出した東方ゲームに確認メッセージ無しで thprac を適用します。
- `--without-vpatch` vpatchの自動適用を防ぎます。

コマンド例
```
thprac.exe --attach 1234
thprac.exe --attach
thprac.exe C:\Users\Name\Desktop\Games\Touhou\th17\th17.exe
thprac.exe --without-vpatch C:\Users\Name\Desktop\Games\Touhou\th17\th17.exe
```

## <a name="compatability"></a> **互換性**
thprac は **Windows Vista** 以降の全てのバージョンの Windows に公式にサポートしています。Windows XP であっても [One-Core-API](https://github.com/Skulltrail192/One-Core-API-Binaries)を使うことで動作する可能性はありますが、これは**公式に動作確認されているわけではありません。**

thprac は **Wine** や Steam Deck にも対応しています。Backspace メニューは Steam Deck で動作するよう設計されていますが、未検証なことに注意してください。

## <a name="features"></a> **機能**

thprac は全ての東方ゲームと妖精大戦争及び黄昏酒場に対応した拡張プラクティスモードが搭載されています。

![Unconnected Marketeers](https://user-images.githubusercontent.com/23106652/174433923-0a6069e7-d10d-4107-8f0d-f4a8a9d56976.png)

![Imperishable Night](https://user-images.githubusercontent.com/23106652/174433975-8f23b0b0-e48e-4be1-8cb7-d8e3e7ab6b8e.png)

thprac は元のプラクティス画面を上のような新しいUIに置き換えます。特定のスペルカードの発狂段階の選択や各種パラメータの変更ができます。元々プラクティスモードが存在しないゲーム(黄昏酒場、妖精大戦争など)はゲーム開始時のメニューにプラクティス画面が追加されます。

![Uwabami Breakers](https://user-images.githubusercontent.com/23106652/174434103-5fee7a13-0254-4602-a468-42330b985bb2.png)
![Great Fairy Wars](https://user-images.githubusercontent.com/23106652/174434121-063142f2-ef3d-4721-ab96-a252343cdb0e.png)

このメニューはキーボードやコントローラで操作することができます。 **上** キーと **下** キーで項目を選択し、 **左** キーと **右** キーでオプションを変更、 **ショット** キーで決定します。マウスを使用して操作することも可能です。

thprac は選択したオプションを自動的にリプレイファイルに保存します。リプレイを再生する際は thprac が自動的にオプションを適用します。 **thprac 有効時に『カスタム』モードを使用して保存されたリプレイは thprac 未適用の状態では動作しません。**

## <a name="backspace-menu"></a> **Backspaceメニュー**
対応しているゲーム(花映塚と獣王園を除く)の場合、 **Backspace** キーを押すことでBackspaceメニューに入ることができます。このメニューは様々なオプションが表示され、ファンクション(F)キーを使ってそれらを有効にすることができます。

これらのオプションは常にクリック可能で、デスクトップPCではマウスで操作可能な他、 Steam Deck では左親指で操作することができます。 Steam Deck でこの機能を使用するには、 Steam Input で Backspace キーを割り当ててください。

![Unconnected Marketeers](https://user-images.githubusercontent.com/23106652/174434813-73748a66-0f6d-4c6e-9f3a-895a49b93434.png)
![Wily Beast and Weakest Creature](https://user-images.githubusercontent.com/23106652/174434834-6bd93104-1ed2-48ae-a440-9d9cb871ea03.png)

## <a name="advanced-options"></a> **拡張オプション**
対応しているゲームでは F12 キーを押すことで拡張オプションを使用することができます。これらのオプションはゲームに関する修正やパッチ、その他便利な機能が含まれている場合があります。

![Imperishable Night](https://user-images.githubusercontent.com/23106652/174434977-683da583-324b-4bd5-8408-13373dfd5a93.png)
![Unconnected Marketeers](https://user-images.githubusercontent.com/23106652/174435006-e906d30d-0ef5-4930-ae57-1f0919beb5af.png)

## <a name="game-specific-features"></a> **ゲーム個別の機能**
### バレットフィリア達の闇市場 - 強行突破
![100th Black Market](https://github.com/touhouworldcup/thprac/assets/23106652/02c55e5e-5c89-462f-beea-9ab07cbb1051)

thprac が『カスタム』モードで動作している場合、ゲームがウェーブを選択しようとするタイミングで上記ポップアップが表示されます。

### 花映塚 - ツール
![Phantasmagoria of Flower View](https://user-images.githubusercontent.com/23106652/174434249-2bf1d70a-101c-4538-a4e6-8eeaf273dd88.png) 

対戦モードでゲームを開始する場合、『モード』画面で『カスタム』を選択することができます。『カスタム』を選択するとゲーム内にこのウィンドウが表示されます。このウィンドウは自由に移動・調整が可能です。 F11 キーで非表示にすることも可能です。

### 獣王園 - ツール
![TH19 Tools](https://github.com/touhouworldcup/thprac/assets/23106652/a2cdb385-b61d-4111-af6b-b195e85bf18a)

花映塚のようなことが獣王園でも行うことができます。

### 紅魔郷 - 一時停止画面
![TH06](https://user-images.githubusercontent.com/23106652/174436027-734d642a-300c-45ab-9591-b6219aca087b.png)

『終了』を選択することでリプレイを保存するかどうかの確認メッセージが表示されます。

**注意**: この方法で保存されたリプレイは保存されたフレーム以降も再生されます。自機はその場に停止し、ショットを撃たなくなります。

### ナイトメアダイアリー、鬼形獣、虹龍洞の ESC + R の高速リトライに関する修正

## <a name="credits"></a> **クレジット (2025年6月1日以来)**
- 開発: [32th System](https://www.youtube.com/channel/UChyVpooBi31k3xPbWYsoq3w), [muter3000](https://github.com/muter3000), [zero318](https://github.com/zero318), [Lmocinemod](https://github.com/Lmocinemod), [Cao Minh](https://github.com/hoangcaominh), [raviddog](https://github.com/raviddog) 
- 中国語翻訳: [CrestedPeak9](https://twitter.com/CrestedPeak9), maksim71_doll, DeepL
- 日本語翻訳: [ゆーみや](https://www.twitch.tv/toho_yumiya), CyrusVorazan, DeepL
- 全てのバグ報告フォームを [旧バグ報告フォーム](https://github.com/ack7139/thprac/issues) から [こちら](https://github.com/touhouworldcup/thprac/issues) に移行中です: [toimine](https://www.youtube.com/channel/UCtxu8Rg0Vh8mX6iENenetuA)
- 英語版 README.md: [Galagyy](https://github.com/Galagyy)
- 中国語版 README.md 翻訳: [TNT569](https://github.com/TNT569), [H-J-Granger](https://github.com/H-J-Granger)
- 日本語版 README.md 翻訳: [wefma](https://github.com/wefma)

## <a name="faq"></a> **よくある質問 (FAQ)**

### 一般

#### なぜ thprac は移管されたのですか？
元の開発者であるAck氏は thprac/Marketeer の開発を無期限で停止すると発表しています。以下は本人のコメントです。
> 私は今後 thprac/Marketeer の開発を無期限に停止します。ライセンス条項を守っていただけるのであれば引き続きこのプロジェクトに取り組んで頂いて構いません。 
> 私の技術力不足で、コードに不可解な記述や酷いロジックで溢れていて、全体が滅茶苦茶になってしまいました。もしご迷惑をおかけしているのであれば申し訳ございません。

現在Ack氏とは連絡が取れず、開発は他の人たちによって引き継がれています。

#### ウイルス対策ソフトウェアが thprac がマルウェアだと言っています。本当に安全ですか？
thprac は悪意のあるコードは含まれていませんが、その動作がウイルス対策ソフトウェアに検知される可能性があります。もし警告が出た場合は例外やホワイトリストに入れて thprac を正常に動作できるようにしてください。あるいは[こちらのバージョン](https://github.com/touhouworldcup/thprac/issues/112)を試してみて、解決しない場合は報告してください。

---

### 互換性

#### thprac は英語化パッチが動きますか？
thprac は **thcrap** と互換性があり、 [Universal THCRAP Launcher](https://github.com/thpatch/Universal-THCRAP-Launcher/)のように、thcrapのランチャーとして動作します。ただし gensokyo.org が作成したような静的な英語化パッチはサポートしていません。

---

#### thprac が東方紅魔郷を見つけられません。どうすればいいですか？

実行ファイルの名前を `東方紅魔郷.exe` か `th06.exe` にしてください。それでも thprac が検出しない場合は、ランチャーからゲームを手動で起動すれば、任意のファイル名であっても thprac をゲームに適用できます。

---

### 機能

#### 紅魔郷から風神録までのゲームでゲーム中にリプレイを途中保存するには？
技術的な制約によりゲーム中にリプレイを途中保存することはサポートされていません。 thprac の 2.0.8.3 には以下のように書かれています。
> これらのゲームのプログラム構造上、この機能を追加するのは非常に難しく、現時点では直接的な解決策がありません。

ただし紅魔郷ではこの機能がサポートされています。

注意：途中保存したリプレイは途中保存したフレーム以降もタイトル画面に戻ることはありません。

---

#### 『永遠に続くBGM』とは何ですか？
このオプションはゲームを一時停止したときにBGMがリセットされるのを防ぎます。

---

#### 『圧迫取材』とは何ですか？ (文花帖 / ダブルスポイラー)
この機能はカメラを常にボスにロックし、カメラのリチャージ時間をなくします。

---

#### 言語はどうやって切り替えますか？
- ランチャーはWindowsの設定に基づいて自動的に言語を選択します。  
- ゲーム内の言語を変更するには、以下のショートカットを使用してください。
  - **ALT + 1**: 日本語 
  - **ALT + 2**: 中国語 
  - **ALT + 3**: 英語 

これらのショートカットはランチャーでは機能しません。詳しい操作方法は **使い方** の章の動画をご覧ください。

---

### バグ報告

#### どこからバグを報告すればいいですか？
[GitHubのissueタブ](https://github.com/touhouworldcup/thprac/issues)からバグ報告や改善提案をしてください。

---

### 技術的な問題

#### 拡張オプションで未対応のvpatchが使われています。
互換性のあるバージョンのvpatchを使用しているか確認してください。[ここ](https://maribelhearn.com/tools#vpatch)からダウンロードすることができます。可能であれば **rev7** のDLLを使用してください。

---

#### vpatch使用時にFPS調整が正しく動作しません。

『DX8 to DX9 Converter』などの一部ツールはvpatchと競合することがあります。リプレイ速度調整(スロー / 早送り)は **神霊廟** のみサポートされています。

---

### ゲーム個別

#### 『輝針城 - 魔理沙レーザー関連』とは何ですか？
**輝針城** において悪名高い魔理沙のレーザーによるリプレイずれバグを修正するオプションです。
