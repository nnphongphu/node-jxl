const fs = require('fs');

let libs = fs.readdirSync(`./libjxl/darwin_x86_64/lib`);

for (const lib of libs)
  fs.copyFileSync(`./libjxl/darwin_x86_64/lib/${lib}`, `./build/Release/${lib}`);

libs = fs.readdirSync(`./libjpeg/darwin_x86_64/lib`);
for (const lib of libs)
  fs.copyFileSync(`./libjpeg/darwin_x86_64/lib/${lib}`, `./build/Release/${lib}`);
