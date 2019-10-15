# **XMusic介绍**
![](https://raw.githubusercontent.com/Musicrosoft/XMusic/master/XMusicHost.jpg)

## **创作背景**
07年用VB开发一个迷你播放器，支持mp3、wma、wav。  

09年用VC开发媒体库模型和配套界面。  
  
18年采用ffmpeg4解码，SDL2播放，支持更多音频格式。优化媒体库模型和用户界面，并开发一系列实用功能。

19年做跨平台适配，并用Qt开发跨平台点播App。

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
![](https://raw.githubusercontent.com/Musicrosoft/XMusic/master/XMusicHost_Playlist.jpg)

* 歌手/专辑管理：分组管理歌手、专辑及曲目的增删改，支持拖放，与资源浏览器、播放列表联动。
![](https://raw.githubusercontent.com/Musicrosoft/XMusic/master/XMusicHost_SingerAlbum.jpg)

* 曲目查找：按目录查找引用、按文件查找引用、按标题查找曲目、按歌手查找曲目、查看新曲目。

* 媒体检测：检查曲目对应的文件是否存在，是否可播放，失效曲目一键自动查找重新匹配。

* 重复曲目检查：检查歌手库、列表库中重复的曲目，可按标题、文件名、路径检查。

* 媒体导出：按歌手、专辑、播放列表导出曲目，常年不需要导出的项目可以设置‘导出屏蔽’。

* 合入外部文件：从外部目录合入媒体文件替换媒体库中的现有文件。

### 正在播放面板
管理正在播放的曲目，支持拖放；
与迷你播放界面联动，迷你界面可换肤；  
与资源浏览器、媒体库联动；  
整轨内部曲目可跳转；

### 随机点播
可随机点播歌手、专辑、专辑曲目、列表、列表曲目，不需要参与点播的项目可设置点播屏蔽。
也可以按语种点播以上5种类型，需要提前设定歌手、专辑、播放列表的语种：国语、粤语、韩语、日语、泰语、英文、西欧小语种。

### 其他功能
* 备份与恢复：针对媒体库db文件备份、比对和恢复。

* 播放记录：按时间查看播放过的曲目。

* 高分辨率/高DPI适配：根据桌面分辨率和DPI智能设置字体大小。

* 支持Win10平板触屏操作

### 跨平台点播APP
使用Qt开发的跨平台UI，其他模块复用。

主要实现随机点播和媒体库浏览功能，支持横屏、竖屏和背景更换。

已编译出Windows、macOs版本和android安装包，安卓采用OpenSL ES播放音频流。
完成ios版本编译和iPhone/iPad各机型适配，app待上架

![](https://raw.githubusercontent.com/Musicrosoft/XMusic/master/XMusic_V.jpg)
![](https://raw.githubusercontent.com/Musicrosoft/XMusic/master/XMusic_H.jpg)
