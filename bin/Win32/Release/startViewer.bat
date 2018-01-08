:: ipath 设置引用图片的路径, Token ${ipath} 会被替换成它
SET "ipath=H:\Photo\2009-11-21"
:: IMG 设置测试图片，可以添加任意多图片；
:: 也可不用${ipath}，而直接给完整的图片路径，如：
:: SET "IMG=d:\a\DSC04692.jpg|%IMG%"
SET "IMG=${ipath}DSC04700.jpg"
SET "IMG=${ipath}DSC04692.jpg|%IMG%"
SET "IMG=${ipath}DSC04693.jpg|%IMG%"
SET "IMG=${ipath}DSC04695.jpg|%IMG%"
SET "IMG=${ipath}DSC04696.jpg|%IMG%"
SET "IMG=${ipath}DSC04697.jpg|%IMG%"
SET "IMG=${ipath}DSC04698.jpg|%IMG%"

:: 运行图片预览工具
::ImageViewer.exe -ipath="%ipath%" -images="%IMG%"
ImageViewer.exe -help -taskbar=0 -ipath=H:\Photo\2009-11-21 -ximages="D:\oIM\src\ImageViewer\uires\image\test.gif" -ifilter=*.jpg -scale=100
::[4832] [C_CommandLine::ParseCommandLine L000255] -taskbar=0 -images="D:\eIM.V3\src\eIMMain\..\..\bin\Win32\Debug\Emoticon\face_78liwu.png"

