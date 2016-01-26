注意：//本文档有可能行尾为/r而非/r/n，显示不分行
----v0.9----
说明：
除obj导入外已基本完成；
文件中包含obj导入的相关文件，包括程序文件和obj文件，尚未使用。
因系统故障，暂时无法进行后续工作。

使用：
1.墙面纹理切换：e
2.sky模式和跟随模式切换：w
3.灯源编辑：asdzxc（共2个灯源，包含光强调整和开关灯）
4.end键：mode切换
5.主球移动：方向键（mode0）
6.主球方向旋转：左右方向键(mode1)
7.主球加速减速（步距调整）：上下方向键（mode1）
8.sky模式：鼠标右键-观察视角旋转；鼠标左键：观察视角移动
默认：跟随模式，mode0。
从后上方跟随主球，sky模式初始默认在起点上方平视前方。
----v1.0----
备份：
linux编译命令：
gcc -g -o MazeFx main.cpp Display.h Display.cpp Keyboard.h Keyboard.cpp glm.h glm.cpp Obj.h Obj.cpp -lGL -lGLU -lglut -lm

说明：本来在win7下制作，由于win7系统故障无法使用，在ubuntu下用gcc重新编译，对无法通过的部分进行了调试修改。

update:
1.修改gcc编译中无法通过的部分（修改内容较多）。
2.修正碰撞检测及其他细微bug。（在复制过程中出现偏差）
3.添加obj导入。【所有obj均为立体文本obj，由ubuntu下blender制作】

程序说明：
（1）在起终点有start和end字样；
（2）主球碰到其他3个球或其他球先到达终点（概率极低）会显示You Lose，主球到达终点会显示You Win.(在主球正上方，正向方向)
（3）程序会在起点方向外的3个方向墙边任意处各生成一个球（有一定几率在终点边上），定时随机移动单位1。
【由于定时时间小，移动频繁，有几率在程序游戏开始时就移动到了主球附近，基本上因为随机移动，看起来像在一个小范围内乱动，但也有可能移动的比较远】

其他：
1.光源编辑无效；
2.物体上色错误(color3f)；
【在v0.9(win7下)时都是已测试正常的功能】
----v1.1----
说明：
1.bug修正及优化

update:
main.cpp:
1.更新winlose判断。（已经win或lose后不再进行判断），
位置：timer函数内，在winlose=1/2之前加上if(winlose==0)
2.提升sky模式初始视角高度。
位置：InitMaze函数内：Eye[1]=eye[1]+flo[1]+30；
keyboard.cpp:
1.添加退出键[q]
位置：keyboardfunc内，添加case 'q':exit(0);break;
2.修正sky模式下仍可以转变方向的bug.（此时仍可以移动，允许转换方向会出现移动与方向键不符的bug）
位置：specialfunc内，在case GLUT_KEY_LEFT和GLUT_KEY_RIGHT的if(mode==0){……}else{……}
改为else if(sky==0){……}。
  sky模式下球的方向重置为UP方向。
  位置：keyboardfunc内,在sky控制后，判断sky==1,调整方向。
3.修正球消失后仍可以碰撞
位置：keyboard.cpp,main.cpp
备注：win或lose后球都会消失，可以移动出去
4.修正win/lose方向(随球的方向变化)
位置：Display.cpp

----V1.2----
update:
1.修正颜色问题：在绘制面后添加glDisable(GL_TEXTURE_2D);
2.修正灯光问题：去除glcolor颜色，设置物体材质
3.删除light0
