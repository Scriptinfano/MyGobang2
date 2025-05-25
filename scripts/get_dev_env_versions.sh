#!/bin/bash
(
echo "收集开发环境版本信息中，请稍候..."
echo "==============================="
echo "        系统版本信息"
echo "==============================="
sw_vers
uname -a

echo
echo "==============================="
echo "        Qt 版本信息"
echo "==============================="

# Qt Creator 版本（如果已安装）
if [ -d "$HOME/Qt/Qt Creator.app" ]; then
    version=$(defaults read "/Users/mavrick/Qt/Qt Creator.app/Contents/Info.plist" CFBundleShortVersionString)
    echo "Qt Creator 版本: $version"
else
    echo "未检测到 Qt Creator"
fi

echo
echo "==============================="
echo "        CMake 版本信息"
echo "==============================="
if command -v cmake &> /dev/null
then
    cmake --version | head -n 1
else
    echo "未检测到 cmake"
fi

echo
echo "==============================="
echo "        C++ 编译器信息"
echo "==============================="
if command -v clang++ &> /dev/null
then
    clang++ --version | head -n 1
else
    echo "未检测到 clang++"
fi

echo
echo "==============================="
echo "        Xcode 工具链"
echo "==============================="
if command -v xcodebuild &> /dev/null
then
    xcodebuild -version
else
    echo "未检测到 Xcode"
fi

echo
echo "==============================="
echo "        Git 版本"
echo "==============================="
if command -v git &> /dev/null
then
    git --version
else
    echo "未检测到 git"
fi

echo
echo "==============================="
echo "        Python 版本"
echo "==============================="
if command -v python3 &> /dev/null
then
    python3 --version
else
    echo "未检测到 python3"
fi

echo
echo "==============================="
echo "        Homebrew 版本"
echo "==============================="
if command -v brew &> /dev/null
then
    brew --version | head -n 1
else
    echo "未检测到 Homebrew"
fi

echo
echo "==============================="
echo "        Make 工具版本"
echo "==============================="
if command -v make &> /dev/null
then
    make --version | head -n 1
else
    echo "未检测到 make"
fi

echo
echo "==============================="
echo "        检查完毕"
echo "==============================="
)|tee dev_env_info.txt
