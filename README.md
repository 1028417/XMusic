# **XMusic介绍**
![](https://raw.githubusercontent.com/1028417/readmeres/master/Player/Player.jpg)

## **创作背景**
07年用VB开发一个迷你播放器，支持mp3、wma、wav。  

09年用VC开发媒体库模型和配套界面。  
  
18年采用ffmpeg4+SDL2解码播放，支持更多音频格式。优化媒体库模型和用户界面，并开发一系列实用功能。

19年用QT做跨平台适配，并开发安卓端点播App。

## **系统功能**
### 文件系统
* 系统可挂载多个不同硬盘分区的目录；
* 支持Mp3、Wav、Flac、Ape、Wma、AAC、AC3等常见文件格式，支持MP3/flac的tag信息读入；
* 资源浏览器可按平铺、图标、明细显示，与媒体库联动；
* 导出目录和生成快照；
* 智能检测相似文件；
* 智能识别cue对应的整轨文件；
* 完美支持日、韩、泰、西欧小语种等字符集。

### 媒体库
* 播放列表管理：管理播放列表及曲目的增删改，支持拖放，与资源浏览器、歌手/专辑联动。
![](https://raw.githubusercontent.com/1028417/readmeres/master/Player/Playlist.jpg)

* 歌手/专辑管理：管理歌手、专辑及曲目的增删改，支持拖放，与资源浏览器、播放列表联动。
![](https://raw.githubusercontent.com/1028417/readmeres/master/Player/SingerAlbum.jpg)

* 曲目查找：按目录查找引用、按文件查找引用、按标题查找曲目、按歌手查找曲目、查看新曲目。

* 媒体检测：检查曲目对应的文件是否存在，是否可播放，失效曲目一键自动查找重新匹配。

* 重复曲目检查：检查列表库中重复的曲目，检查歌手库中重复的曲目。

* 媒体导出：按播放列表、歌手、专辑导出曲目。

* 合入外部文件：从外部目录合入媒体文件替换媒体库中的现有文件。

### 正在播放面板
管理正在播放的曲目，支持拖放；
与迷你播放界面联动，迷你界面可换肤；  
与资源浏览器、媒体库联动；  
整轨内部曲目可跳转；

### 随机点播
歌手随机点播、专辑随机点播、播放列表随机点播、列表库中的曲目随机点播、歌手库中的曲目随机点播。

### 其他功能
* 备份与恢复：针对媒体库db文件备份、比对和恢复。

* 播放记录：按时间查看播放过的曲目。

* 高分辨率/高DPI适配：根据桌面分辨率和DPI智能设置字体大小。

* 支持Win10平板触屏操作

