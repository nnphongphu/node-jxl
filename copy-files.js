const fs = require('fs');

const libs = [
  "brotlicommon.dll",
  "brotlidec.dll",
  "jxl_dec.dll",
  "jxl_threads.dll",
]

for (const lib of libs) {
  fs.copyFileSync(`./libjxl/x86Clang/lib/${lib}`, `./build/Release/${lib}`);
}