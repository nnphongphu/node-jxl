const platform_and_arch = require('./platform.js')();

const decompress = require('decompress');
const decompressTargz = require('decompress-targz');
 
decompress(`./vendor/libjxl/${platform_and_arch}.tar.gz`, `./libjxl`, {
    plugins: [
        decompressTargz()
    ]
});

decompress(`./vendor/libjpeg/${platform_and_arch}.tar.gz`, `./libjpeg`, {
  plugins: [
      decompressTargz()
  ]
});
