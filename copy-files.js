const fs = require('fs');

let libs = fs.readdirSync(`./libjxl/darwin_arm64/lib`);

for (const lib of libs)
  fs.copyFileSync(`./libjxl/darwin_arm64/lib/${lib}`, `./build/Release/${lib}`);

libs = fs.readdirSync(`./libjpeg/darwin_arm64/lib`);
for (const lib of libs)
  fs.copyFileSync(`./libjpeg/darwin_arm64/lib/${lib}`, `./build/Release/${lib}`);
