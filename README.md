# kiranwidgets-qt5
kiran桌面环境的相关**Qt控件**的封装

## 编译
1.  安装编译依赖  
   `sudo yum install gcc-c++ qt5-qtbase qt5-qtbase-devel qt5-qtx11extras qt5-qtx11extras-devel libX11 libX11-devel`
2. **源码根目录**下创建**build**目录`mkdir build`
3. 进行**build**目录,执行`qmake-qt5 ..`生成**Makefile**
4. 执行`make`进行编译

## 安装
1. 在**build**目录下执行`sudo make install`

## 卸载
1. 在**build**目录下执行`sudo make uninstall`

## 使用
1. 安装**examples**，查看**/usr/share/kiranwidgets-qt5/examples/**


## 具体控件的使用

### kiran-titlebar-window

<img src="preview-images/kiran-titlebar-window.png" alt="kiran-titlebar-window" style="zoom:33%;" />

自定义标题栏窗口

#### 头文件位置

/usr/include/kiranwidgets-qt5/kiran-titlebar-window.h

#### 例子

##### KiranTitlebarWindow_UI

使用UI文件来生成界面的示例代码

###### 注意事项:

因为传入**setupUI**的参数为**KiranTitlebarWindow**创建的**QWidget**(专门用于显示窗口内容的区域 )，而不是**this**

- **不要**通过**QtDesigner信号和槽编辑器**添加的信号和槽

  QtDesigner中信号和槽编辑器中连接的信号和槽在生成代码setupUI方法中，会connect ui控件和KiranTitlebarWindow创建的QWidget的槽，因为传入setupUI的参数为KiranTitlebarWindow创建的子控件QWidget类型

- 尽量**不要**在QtDesigner**中通过右键控件“**Designer”来添加槽函数

  这个实际上是生成具有规范函数名的槽函数，在setupUI中的QMetaObject::connectSlotsByName(...);来进行连接，因为传入setupUI的参数为KiranTitlebarWindow创建的子控件QWidget类型，所以连接不上

##### KiranTitlebarWindow_CPP

使用代码来生成界面的示例代码

##### KiranTitlebarWindow_Simple

单纯使用面向过程,不通过派生来使用KiranTitlebarWindow
