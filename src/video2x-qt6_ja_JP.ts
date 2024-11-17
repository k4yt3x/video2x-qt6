<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ja_JP">
<context>
    <name>AboutDialog</name>
    <message>
        <location filename="aboutdialog.ui" line="20"/>
        <source>About Video2X Qt6</source>
        <translation>Video2X Qt6 について</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="mainwindow.ui" line="29"/>
        <source>Video Processing Job Queue</source>
        <translation>動画処理のジョブキュー</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="43"/>
        <source>Click the &quot;Add File(s)&quot; button or drag files into the job queue to add videos for processing.</source>
        <translation>処理する動画を追加するには「ファイルを追加」ボタンをクリックするか、ファイルをジョブキューにドラッグしてください。</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="71"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Select files to add to the job queue.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;ジョブキューに追加するファイルを選択します。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="74"/>
        <source>Add File(s)</source>
        <translation>ファイルを追加</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="81"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Delete the selected file(s) from the job queue.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;選択したファイルをジョブキューから消去します。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="84"/>
        <source>Delete Selected</source>
        <translation>選択した項目を消去</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="91"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Remove all jobs from the job queue.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;ジョブキューからすべてのジョブを消去します。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="94"/>
        <source>Clear</source>
        <translation>すべて消去</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="118"/>
        <source>Filter Settings</source>
        <translation>フィルター設定</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="130"/>
        <source>Filter Selection</source>
        <translation>フィルターを選択</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="154"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-size:9pt; font-weight:400;&quot;&gt;Select the filter you want to use to process the video.&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-size:9pt; font-weight:400;&quot;&gt;- RealESRGAN: Higher quality but slower, with fixed scaling factor. Works better than Anime4K v4 with small (&amp;lt;720p) videos.&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-size:9pt; font-weight:400;&quot;&gt;- libplacebo (with Anime4K v4): Very fast, but is of lower quality. Works poorly with small (&amp;lt;720p) videos.&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-size:9pt; font-weight:400;&quot;&gt;動画の処理に使用するフィルターを選択してください。&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-size:9pt; font-weight:400;&quot;&gt;- RealESRGAN: 高画質ですが処理速度が遅く、拡大倍率は固定です。解像度が小さい動画 (&amp;lt;720p) では Anime4K v4 より上手く機能します。&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-size:9pt; font-weight:400;&quot;&gt;- libplacebo (Anime4K v4 を使用): 非常に高速ですが、画質は低くなります。解像度の低い動画 (&amp;lt;720p) では上手く機能しません。&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="158"/>
        <source>RealESRGAN</source>
        <translation></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="163"/>
        <source>libplacebo</source>
        <translation></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="192"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The GPU to use to process the files.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;ファイル処理に使用する GPU。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="216"/>
        <source>Model</source>
        <translation>モデル</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="223"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The RealESRGAN model to use. Different models support specific scaling ratios.&lt;/p&gt;&lt;p&gt;- realesr-animevideov3 (2x, 3x, 4x): preferred for anime&lt;br/&gt;- realesrgan-plus-anime (4x)&lt;br/&gt;- realesrgan-plus (4x): for non-anime videos&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <oldsource>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The RealESRGAN model to use.&lt;/p&gt;&lt;p&gt;- realesr-animevideov3: preferred for anime&lt;/p&gt;&lt;p&gt;- realesrgan-plus-anime&lt;/p&gt;&lt;p&gt;- realesrgan-plus: for non-anime videos&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</oldsource>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;RealESRGAN モデルを使用します。&lt;/p&gt;&lt;p&gt;- realesr-animevideov3 (2x, 3x, 4x): アニメの動画に最適&lt;br/&gt;- realesrgan-plus-anime (4x)&lt;br/&gt;- realesrgan-plus (4x): アニメ以外の動画に最適&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="252"/>
        <source>Scaling Factor</source>
        <translation>スケーリング係数</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="265"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The scaling factor to use. I.e., how many times larger you want the output to be.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;使用するスケーリング係数です。出力を何倍にしたいかを設定します。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="295"/>
        <source>libplacebo Settings</source>
        <translation>libplacebo 設定</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="303"/>
        <source>Output Width</source>
        <translation>出力する幅</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="316"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The output video&apos;s width.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;出力する動画の幅です。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="336"/>
        <source>Output Height</source>
        <translation>出力する高さ</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="349"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The output video&apos;s height.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;出力する動画の高さです。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="376"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The GLSL shader used to process the video.&lt;/p&gt;&lt;p&gt;- Anime4K v4 Mode A: Optimized for 1080p anime&lt;br/&gt;- Anime4K v4 Mode B: Optimized for 720p anime&lt;br/&gt;- Anime4K v4 Mode C: Optimized for 480p anime&lt;br/&gt;- Anime4K v4 Mode A+A: Higher quality version of mode A&lt;br/&gt;- Anime4K v4 Mode B+B: Higher quality version of mode B&lt;br/&gt;- Anime4K v4 Mode C+A: Higher quality version of mode C&lt;br/&gt;- Anime4K v4.1 GAN: Experimental SRGAN shaders; delivers the highest quality but is slower than standard modes&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <oldsource>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The GLSL shader used to process the video.&lt;/p&gt;&lt;p&gt;- anime4k-a: Optimized for 1080p anime&lt;br/&gt;- anime4k-b: Optimized for 720p anime&lt;br/&gt;- anime4k-c: Optimized for 480p anime&lt;br/&gt;- anime4k-a+a: Higher quality version of a&lt;br/&gt;- anime4k-b+b: Higher quality version of b&lt;br/&gt;- anime4k-c+a: Higher quality version of c&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</oldsource>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;動画の処理に使用する GLSL シェーダー。&lt;/p&gt;&lt;p&gt;- Anime4K v4 モード A：1080p のアニメに最適化&lt;br/&gt;- Anime4K v4 モード B：720p のアニメに最適化&lt;br/&gt;- Anime4K v4 モード C：480p のアニメに最適化&lt;br/&gt;- Anime4K v4 モード A+A：モード A の高画質版&lt;br/&gt;- Anime4K v4 モード B+B：モード B の高画質版&lt;br/&gt;- Anime4K v4 モード C+A：モード C の高画質版&lt;br/&gt;- Anime4K v4.1 GAN：実験的な SRGAN シェーダー。最高品質を提供するが、標準モードよりも速度が遅い&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="423"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Path to a custom MPV-compatible GLSL shader file. Leave empty to use the GLSL shader selected above for processing.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;MPV 互換の GLSL シェーダーファイルのパス。処理には上で選択した GLSL シェーダーを使用する場合は空のままにします。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="433"/>
        <source>Select GLSL Shader File</source>
        <translation>GLSL シェーダーファイルを選択</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="444"/>
        <source>FFmpeg Settings</source>
        <translation>FFmpeg の設定</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="452"/>
        <source>Container Format (suffix)</source>
        <translation>コンテナの形式 (接頭辞)</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="465"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Sets the container format for the output file, determining how audio, video, and metadata are stored.&lt;/p&gt;&lt;p&gt;E.g., .mp4, .mkv, .mov, .avi&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;出力ファイルのコンテナ形式の設定とオーディオ、動画、メタデータの保存方法を設定します。&lt;/p&gt;&lt;p&gt;例: .mp4, .mkv, .mov, .avi&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="479"/>
        <source>Codec (-vcodec)</source>
        <translation>コーデック (-vcodec)</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="492"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Selects the video codec for encoding, affecting compression and compatibility.&lt;/p&gt;&lt;p&gt;E.g., libx264, libx265, h264_nvenc&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;エンコード用のビデオコーデックを選択して動画の圧縮と互換性の設定をします。&lt;/p&gt;&lt;p&gt;例: libx264, libx265, h264_nvenc&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="506"/>
        <source>Preset (preset)</source>
        <translation>プリセット (-preset)</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="519"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The encoding preset to use. A slower preset provides better compression efficiency (quality-to-file-size ratio), while a faster preset results in quicker encoding at the cost of compression efficiency.&lt;/p&gt;&lt;p&gt;- ultrafast&lt;br/&gt;- superfast&lt;br/&gt;- veryfast&lt;br/&gt;- faster&lt;br/&gt;- fast&lt;br/&gt;- medium&lt;br/&gt;- slow – default preset&lt;br/&gt;- slower&lt;br/&gt;- veryslow&lt;br/&gt;- placebo&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <oldsource>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The encoding preset to use. A slower preset provides better compression efficiency (quality-to-file-size ratio), while a faster preset results in quicker encoding at the cost of compression efficiency.&lt;/p&gt;&lt;p&gt;- ultrafast&lt;/p&gt;&lt;p&gt;- superfast&lt;/p&gt;&lt;p&gt;- veryfast&lt;/p&gt;&lt;p&gt;- faster&lt;/p&gt;&lt;p&gt;- fast&lt;/p&gt;&lt;p&gt;- medium&lt;/p&gt;&lt;p&gt;- slow – default preset&lt;/p&gt;&lt;p&gt;- slower&lt;/p&gt;&lt;p&gt;- veryslow&lt;/p&gt;&lt;p&gt;- placebo&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</oldsource>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;使用するエンコードのプリセットです。処理が遅いプリセットでは圧縮効率 (品質とファイルサイズの比率) が向上し、処理が速いプリセットでは圧縮効率が低下しますがエンコードが速くなります。&lt;/p&gt;&lt;p&gt;- ultrafast&lt;br/&gt;- superfast&lt;br/&gt;- veryfast&lt;br/&gt;- faster&lt;br/&gt;- fast&lt;br/&gt;- medium&lt;br/&gt;- slow – デフォルトプリセット&lt;br/&gt;- slower&lt;br/&gt;- veryslow&lt;br/&gt;- placebo&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="533"/>
        <source>Pixel Format (-pix_fmt)</source>
        <translation>ピクセル形式 (-pix_fmt)</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="546"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Specifies the pixel format for the output video. Determines how pixel data is stored, affecting color depth, transparency, and compatibility.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;出力する動画のピクセル形式を指定します。ピクセルデータの保存方法を決定することで色の深度、透明度、互換性に影響します。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="560"/>
        <source>Bit Rate (-b:v)</source>
        <translation>ビットレート (-b:v)</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="567"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Sets the target bit rate for the output video, enabling constant bit rate (CBR) encoding. A value of 0 uses variable bit rate (VBR) for better quality at lower file sizes.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;出力する動画のビットレートを設定することで固定ビットレート (CBR) のエンコードを有効化します。値が 0 の場合は可変ビットレート (VBR) が使用され、ファイルサイズが小さく画質が向上します。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="570"/>
        <source>bps</source>
        <translation></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="590"/>
        <source>CRF (-crf)</source>
        <translation></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="597"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Sets the Constant Rate Factor (CRF) to control video quality. Uses variable bit rate (VBR) encoding. &lt;/p&gt;&lt;p&gt;- 0-17: Visually lossless quality, large file sizes.&lt;/p&gt;&lt;p&gt;- 18-23: High quality, good balance between quality and file size (recommended range).&lt;/p&gt;&lt;p&gt;- 24-28: Medium quality, suitable for streaming, some artifacts may appear.&lt;/p&gt;&lt;p&gt;- 29+: Low quality, noticeable compression artifacts, smaller file sizes.&lt;/p&gt;&lt;p&gt;Lower values improve quality but increase file size, while higher values reduce quality and file size.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;動画の画質を制御するために一定のレート係数 (CRF) を設定します。可変ビットレート (VBR) のエンコードを使用します。&lt;/p&gt;&lt;p&gt;- 0-17: 視覚的にロスレスな画質、ファイルサイズが大きくなります。&lt;/p&gt;&lt;p&gt;- 18-23: 画質が高く、ファイルサイズのバランスが良いです (推奨範囲)。&lt;/p&gt;&lt;p&gt;- 24-28: 中程度の画質でストリーミング適していますが、アーティファクトな仕上がりになる場合があります。&lt;/p&gt;&lt;p&gt;- 29+: 低画質、圧縮されたアーティファクトが目立ちますがファイルサイズが小さくなります。&lt;/p&gt;&lt;p&gt;値を低くすると画質は向上しますがファイルサイズが大きくなり、値を大きくすると画質とファイルサイズは低下します。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="620"/>
        <source>Hardware Acceleration (-hwaccel)</source>
        <translation>ハードウェアアクセラレーション (-hwaccel)</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="633"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Sets the hardware acceleration method to use. Currently not very stable, especially on Linux.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;使用するハードウェアアクセラレーション方式を設定します。現在はあまり安定していません (特に Linux の環境)。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="647"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Check to copy audio and subtitle streams from the input video to the output video.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;チェックをすると入力の動画から出力の動画にオーディオと字幕のストリームがコピーされます。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="650"/>
        <source>Copy audio and subtitle streams</source>
        <translation>オーディオストリームと字幕をコピー</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="676"/>
        <source>Debug</source>
        <translation>デバッグ</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="684"/>
        <source>Log Level</source>
        <translation>ログレベル</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="691"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Sets the verbosity of the log, where &apos;trace&apos; is the most verbose, and &apos;none&apos; is the least verbose, disabling all logging output.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;ログの詳細を設定します。「trace」は詳細なログを出力「none」は最低限なログになり、すべてのログの出力が無効化されます。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="780"/>
        <source>Progress</source>
        <translation>進行</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="813"/>
        <source>Current: %v/%m (%p%)</source>
        <translation>現在：%v/%m (%p%)</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="845"/>
        <source>Overall: %v/%m (%p%)</source>
        <translation>全体：%v/%m (%p%)</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="865"/>
        <source>Frames/s:</source>
        <translation>フレーム/秒：</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="886"/>
        <source>Time Elapsed:</source>
        <translation>経過時間：</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="907"/>
        <source>Time Remaining:</source>
        <translation>残り時間：</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="369"/>
        <source>GLSL Shader</source>
        <translation>GLSL シェーダー</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="180"/>
        <source>GPU Selection</source>
        <translation>GPU 選択</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="208"/>
        <source>RealESRGAN Settings</source>
        <translation>RealESRGAN 設定</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="426"/>
        <source>Custom GLSL shader file path</source>
        <translation>カスタム GLSL シェーダーファイルのパス</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="743"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Displays the logs in a console window (Windows only). On other platforms, the logs will be printed in the terminal.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;ログをコンソールウィンドウに表示します (Windows のみ)。他のプラットフォームではターミナルにログが出力されます。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="746"/>
        <source>Show Logs</source>
        <translation>ログを表示</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="947"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-size:9pt; font-weight:400;&quot;&gt;When stopped: start processing.&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-size:9pt; font-weight:400;&quot;&gt;When running: pause processing.&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-size:9pt; font-weight:400;&quot;&gt;停止時: 処理を開始します。&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-size:9pt; font-weight:400;&quot;&gt;実行中: 処理を一時停止します。&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="953"/>
        <location filename="mainwindow.cpp" line="380"/>
        <source>START</source>
        <translation>開始</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="975"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-size:9pt; font-weight:400;&quot;&gt;Abort processing.&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-size:9pt; font-weight:400;&quot;&gt;処理を中止します。&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="981"/>
        <source>STOP</source>
        <translation>停止</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1002"/>
        <source>File</source>
        <translation>ファイル</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1008"/>
        <source>Help</source>
        <translation>ヘルプ</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1015"/>
        <source>Language</source>
        <translation>言語</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1029"/>
        <source>Exit</source>
        <translation>終了</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1032"/>
        <source>Ctrl+Q</source>
        <translation></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1037"/>
        <source>About</source>
        <translation>バージョン情報</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1042"/>
        <source>English (United States)</source>
        <translation></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1047"/>
        <source>中文（中国）</source>
        <translation></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1052"/>
        <source>日本語（日本）</source>
        <translation></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1057"/>
        <source>Português (Portugal)</source>
        <translation></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1062"/>
        <source>Report Bugs...</source>
        <translation>バグを報告...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="264"/>
        <source>Select Files</source>
        <translation>ファイルを選択</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="266"/>
        <source>All Files (*.*)</source>
        <translation>すべてのファイル (*.*)</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="333"/>
        <source>RESUME</source>
        <translation>再開</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="336"/>
        <source>PAUSE</source>
        <translation>一時停止</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="152"/>
        <location filename="mainwindow.cpp" line="162"/>
        <source>Error</source>
        <translation>エラー</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="44"/>
        <location filename="mainwindow.cpp" line="388"/>
        <location filename="mainwindow.cpp" line="399"/>
        <source>Status: </source>
        <translation>ステータス： </translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="44"/>
        <location filename="mainwindow.cpp" line="388"/>
        <source>idle</source>
        <translation>待機中</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="86"/>
        <source>No GPUs with Vulkan support found!</source>
        <translation>Vulkan をサポートする GPU が見つかりませんでした！</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="173"/>
        <source>Warning</source>
        <translation>警告</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="347"/>
        <source>The job queue is empty!</source>
        <translation>ジョブキューが空です!</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="368"/>
        <source>Processing aborted</source>
        <translation>処理を中止</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="368"/>
        <source>Video processing aborted!</source>
        <translation>動画の処理が中止されました!</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="371"/>
        <source>Processing complete</source>
        <translation>処理が完了しました</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="371"/>
        <source>All videos processed.</source>
        <translation>すべての動画の処理が完了しました。</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="399"/>
        <source>Processing file </source>
        <translation>処理中のファイル </translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="418"/>
        <source>Failed to allocate memory for FilterConfig.</source>
        <translation>FilterConfig のメモリ割り当てに失敗しました。</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="493"/>
        <source>Invalid filter selected!</source>
        <translation>無効なフィルターが選択されました!</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="501"/>
        <source>Invalid FFmpeg video codec.</source>
        <oldsource>Invalid FFmpeg codec.</oldsource>
        <translation>FFmpeg ビデオコーデックが無効です。</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="512"/>
        <source>Invalid FFmpeg video pixel format.</source>
        <oldsource>Invalid FFmpeg pixel format.</oldsource>
        <translation>FFmpeg ビデオピクセル形式が無効です。</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="522"/>
        <source>Failed to allocate memory for EncoderConfig.</source>
        <translation>EncoderConfig のメモリ割り当てに失敗しました。</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="533"/>
        <source>Invalid hardware acceleration method.</source>
        <translation>ハードウェアアクセレーション方式が無効です。</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="556"/>
        <source>Failed to allocate memory for VideoProcessingContext.</source>
        <translation>VideoProcessingContext のメモリ割り当てに失敗しました。</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="643"/>
        <source> days</source>
        <translation> 日</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="684"/>
        <source>Video processing failed for: %1.
Check logs for more information. Enable logging in Debug &gt; Show Logs.</source>
        <translation>動画の処理に失敗しました: %1。
詳細はログを確認してください。「デバッグ &gt; ログを表示」でログを有効にしてください。</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="769"/>
        <source>Select GLSL Shader</source>
        <translation>GLSL シェーダーを選択</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="771"/>
        <source>GLSL Files (*.glsl);;All Files (*.*)</source>
        <translation>GLSL ファイル (*.glsl);;すべてのファイル (*.*)</translation>
    </message>
</context>
</TS>
