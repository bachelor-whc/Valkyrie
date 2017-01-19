![Current](https://github.com/bachelorwhc/Valkyrie/blob/master/doc/result/20170120022003.gif?raw=true)

# Valkyrie
Valkyrie利用Vulkan進行3D Rendering，目標是要作為一個遊戲引擎提供遊戲開發者使用，但<b>目前仍離此目標相當相當遙遠如天與地</b>。作者希望透過達成此目標而學習遊戲開發所需要的各種技術，尤其是Vulkan提供的可能性。
> Render your model by Vulkan just in 150 lines!
### Valkyrie使用的第三方函式庫或代碼
Valkyrie所使用的第三方函式庫或代碼其版權聲明都放置於`license`資料夾內，本專案目前所使用的如下：
* Dear ImGui
* GLM
* googletest
* nlohmann/json
* SDL2
* Intel® TBB
* zlib

由於專案性質，可能會同時於各個branch使用不同的第三方函式庫與代碼，屆時每個branch的`license`內容皆不同。但任何二進位可執行、連結的執行檔或Library與`README.md`僅紀錄該版本及目前`master`所採用的。

## 目前功能
* Valkyrie提供Vulkan的封裝與介面
* Valkyrie可透過libpng讀取PNG影像檔作為Texture
* Valkyrie可Render具材質貼圖的3D Model
* 載入SPV(使用者自行編譯)
* 簡易設置Descriptor Pool, Descriptor Set, Descriptor Set Layout(2016/12/22)
* 專用格式Lavy(仿glTF)與Blender用Exporter

## 未來開發項目
* 更好的Vulkan封裝與介面
* 特定3D Model格式的Loader與周邊處理工具
  * 考慮使用glTF或自訂格式，以容量小、輕便簡單為目標
* 跨平台的GUI系統
* Data-driven的繪圖控制使用系統
* Multi-threaded Rendering
* 物理引擎
* 場景編輯工具
* CMake與跨平台編譯環境支援
* 良好的文件支援
* Shader Code的解析與Vulkan參數自動調整

## 目前開發項目
* 使用imgui作為GUI系統
* 常見特效(e.g., toon shading, bloom, shadow)
* 特定3D Model格式的Loader與周邊處理工具

## 如何參與開發與使用Valkyrie
### 參與開發
Valkyrie採用MIT License作為開源計畫。<br/>
如果您有任何心得或建議，都歡迎你寄信至`bachelor.whc@gmail.com`。<br/>
基於挑戰與學習 作者希望保持獨自開發，但你若有更好的Valkyrie變體或改進，都歡迎讓作者知道、參考以及討論。

### 使用Valkyrie
Valkyrie目前仍為迷你且青澀的小玩具(Toy)，任何人都可以輕易地理解、使用、擴充原始碼。<br/>
因此，若學生有任何的電腦繪圖學(Computer Graphics)作業需要~~抄襲交差~~，Valkyrie應該是不錯的選擇。<br/>
Valkeyrie使用需求：<br/>
* 基本電腦繪圖學觀念
* GLM操作
* 自定義Shader的Vulkan參數設定
* 自行編譯GLSL為SPV
* 自行載入Model與旋轉、縮放、平移等矩陣處理
* 自行設定Buffer
