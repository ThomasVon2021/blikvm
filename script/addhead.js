const fs = require('fs');
const path = require('path');

const targetDirectory = './web-client/src'; // 替换为你的目标目录

// 定义不同文件类型的头部注释
const jsHeaderComment = `
/*****************************************************************************
#                                                                            #
#    blikvm                                                                  #
#                                                                            #
#    Copyright (C) 2021-present     blicube <info@blicube.com>               #
#                                                                            #
#    This program is free software: you can redistribute it and/or modify    #
#    it under the terms of the GNU General Public License as published by    #
#    the Free Software Foundation, either version 3 of the License, or       #
#    (at your option) any later version.                                     #
#                                                                            #
#    This program is distributed in the hope that it will be useful,         #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of          #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           #
#    GNU General Public License for more details.                            #
#                                                                            #
#    You should have received a copy of the GNU General Public License       #
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.  #
#                                                                            #
*****************************************************************************/
`;

const vueHeaderComment = `
<!--
****************************************************************************
#                                                                            #
#    blikvm                                                                  #
#                                                                            #
#    Copyright (C) 2021-present     blicube <info@blicube.com>               #
#                                                                            #
#    This program is free software: you can redistribute it and/or modify    #
#    it under the terms of the GNU General Public License as published by    #
#    the Free Software Foundation, either version 3 of the License, or       #
#    (at your option) any later version.                                     #
#                                                                            #
#    This program is distributed in the hope that it will be useful,         #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of          #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           #
#    GNU General Public License for more details.                            #
#                                                                            #
#    You should have received a copy of the GNU General Public License       #
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.  #
#                                                                            #
****************************************************************************
-->
`;

// 函数：在文件开头添加头部注释
function addHeaderToFile(filePath, headerComment) {
  fs.readFile(filePath, 'utf8', (err, data) => {
    if (err) {
      console.error(`无法读取文件 ${filePath}: ${err}`);
      return;
    }

    // 检查是否已经包含头部注释（避免重复添加）
    if (data.startsWith(headerComment)) {
      console.log(`文件 ${filePath} 已经包含头部注释，跳过处理。`);
      return;
    }

    // 将头部注释和原始内容拼接
    const updatedData = headerComment + data;

    // 写回文件
    fs.writeFile(filePath, updatedData, 'utf8', (err) => {
      if (err) {
        console.error(`无法写入文件 ${filePath}: ${err}`);
        return;
      }
      console.log(`已将头部注释添加到 ${filePath}`);
    });
  });
}

// 遍历目录并处理每个 .js 和 .vue 文件（包括子目录）
function processDirectory(directory) {
  fs.readdir(directory, (err, files) => {
    if (err) {
      console.error(`无法读取目录 ${directory}: ${err}`);
      return;
    }

    files.forEach((file) => {
      const filePath = path.join(directory, file);
      fs.stat(filePath, (err, stats) => {
        if (err) {
          console.error(`无法获取文件信息 ${filePath}: ${err}`);
          return;
        }

        if (stats.isDirectory()) {
          // 如果是目录，递归调用 processDirectory
          processDirectory(filePath);
        } else if (stats.isFile()) {
          const extname = path.extname(file);
          let headerComment;

          // 根据文件扩展名选择合适的头部注释
          if (extname === '.js') {
            headerComment = jsHeaderComment;
          } else if (extname === '.vue') {
            headerComment = vueHeaderComment;
          } else {
            // 如果不是 .js 或 .vue 文件，跳过
            return;
          }

          // 处理 .js 和 .vue 文件
          addHeaderToFile(filePath, headerComment);
        }
      });
    });
  });
}

// 执行脚本
processDirectory(targetDirectory);
