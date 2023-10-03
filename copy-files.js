const fs = require('fs');
const platform_and_arch = require('./platform.js')();

let libs = fs.readdirSync(`./libjxl/${platform_and_arch}/lib`);

for (const lib of libs)
  fs.copyFileSync(`./libjxl/${platform_and_arch}/lib/${lib}`, `./build/Release/${lib}`);

libs = fs.readdirSync(`./libjpeg/${platform_and_arch}/lib`);
for (const lib of libs)
  fs.copyFileSync(`./libjpeg/${platform_and_arch}/lib/${lib}`, `./build/Release/${lib}`);