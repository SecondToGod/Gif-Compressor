g++ imgdeflate.cc -o a -std=c++11  -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc  -L/usr/local/Cellar/opencv/3.4.2
ffmpeg -i in.png -vf palettegen=max_colors=256:stats_mode=single -y out_3.png
ffprobe -hide_banner -v quiet -print_format json -show_format -show_streams in.png > in.log
ffmpeg -i in.png -i out_3.png -lavfi "[0][1:v] paletteuse" -pix_fmt pal8 -y out_4.png
g++ imgdeflate.cc -o a -std=c++11 -I/usr/local/Cellar/ffmpeg/4.0.1/include  -L/usr/local/Cellar/opencv/3.4.2  -L/usr/local/Cellar/ffmpeg/4.0.1 -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -liconv
