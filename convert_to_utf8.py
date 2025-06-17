import os

# 你的文件夹路径
folder_path = r'C:\Users\yourfather\Desktop\stm32f407\github\20250617\study\study'

# ANSI 在中文 Windows 中通常是 GBK 编码
source_encoding = 'gbk'
target_encoding = 'utf-8'

# 需要处理的文件后缀
target_extensions = ['.c', '.h']

def convert_encoding(file_path):
    try:
        with open(file_path, 'r', encoding=source_encoding) as f:
            content = f.read()
        with open(file_path, 'w', encoding=target_encoding) as f:
            f.write(content)
        print(f"[成功] {file_path}")
    except Exception as e:
        print(f"[失败] {file_path}，错误：{e}")

for root, dirs, files in os.walk(folder_path):
    for file in files:
        if any(file.lower().endswith(ext) for ext in target_extensions):
            full_path = os.path.join(root, file)
            convert_encoding(full_path)

print("\n✅ 所有文件已批量转换为 UTF-8 编码")
