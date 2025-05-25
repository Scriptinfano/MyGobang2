#!/bin/bash

# 获取输出文件路径
OUTPUT_FILE="hardware_info.txt"

# 清空旧内容
echo "收集本机硬件配置中，请稍候..." > "$OUTPUT_FILE"

# 获取基本信息
echo "=== 系统信息 ===" >> "$OUTPUT_FILE"
system_profiler SPSoftwareDataType | grep "System Version" >> "$OUTPUT_FILE"
system_profiler SPSoftwareDataType | grep "Kernel Version" >> "$OUTPUT_FILE"

# 获取芯片信息（Apple Silicon）
echo -e "\n=== 芯片信息（CPU） ===" >> "$OUTPUT_FILE"
sysctl -n machdep.cpu.brand_string >> "$OUTPUT_FILE"
system_profiler SPHardwareDataType | grep "Chip" >> "$OUTPUT_FILE"

# 获取内存信息
echo -e "\n=== 内存信息 ===" >> "$OUTPUT_FILE"
system_profiler SPHardwareDataType | grep "Memory" >> "$OUTPUT_FILE"

# 获取磁盘信息
echo -e "\n=== 磁盘信息 ===" >> "$OUTPUT_FILE"
diskutil info / | grep "Device / Media Name" >> "$OUTPUT_FILE"
diskutil info / | grep "Disk Size" >> "$OUTPUT_FILE"

# 获取显示器信息
echo -e "\n=== 显示器信息 ===" >> "$OUTPUT_FILE"
system_profiler SPDisplaysDataType | grep "Resolution" >> "$OUTPUT_FILE"

# 获取电池信息（可选）
echo -e "\n=== 电池信息 ===" >> "$OUTPUT_FILE"
system_profiler SPPowerDataType | grep "Cycle Count" >> "$OUTPUT_FILE"
system_profiler SPPowerDataType | grep "Full Charge Capacity" >> "$OUTPUT_FILE"

# 输出到终端
cat "$OUTPUT_FILE"

