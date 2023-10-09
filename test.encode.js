//index.js
const { createCanvas, loadImage } = require('canvas')
const testAddon = require('./build/Release/libjxl.node');

console.log('addon', testAddon);

const fs = require('fs');

const wrapper = (buffer, width, height) => {
  return new Promise((resolve, reject) => {
    testAddon.bitmapToJxl({ buffer, width, height }, (error, data) => {
      if (error) {
        reject(error);
      } else resolve(data);
    });
  });
};

const writeFile = async (url, buffer) => {
  return new Promise((resolve, reject) => {
    fs.writeFile(url, buffer, (err) => {
      if (err) {
        reject(err);
      } else {
        resolve();
      }
    });
  });
}

const canvas = createCanvas(0, 0);
const ctx = canvas.getContext('2d');

const func = async () => {
  const files = fs.readdirSync('./test/encode/input');

  const result = [];
  for (const fileName of files) {
    const start_read = Date.now();
    const img = await loadImage(`./test/encode/input/${fileName}`);
    canvas.width = img.width;
    canvas.height = img.height;
    ctx.drawImage(img, 0, 0);
    const bitmap = ctx.getImageData(0, 0, img.width, img.height);
    const time = [];
    const end_read = Date.now();
    console.log("time read", end_read - start_read);
    const filtered_bitmap = new Uint8ClampedArray(bitmap.data.length-bitmap.data.length/4);
    for (let i = 0, j = 0; i < bitmap.data.length; i += 1, j += 1) {
      filtered_bitmap[j] = bitmap.data[i];
      if ((i+1+1)%4 === 0) i += 1;
    }
    for (let i = 1; i <= 30; i++) {
      try {
        const start =  Date.now();
        const image = await wrapper(Buffer.from(filtered_bitmap), img.width, img.height);
        const end = Date.now();
        
        //if (i == 1) await writeFile(`./test/output/${fileName.split(".")[0]}.jxl`, image);
        time.push(end-start);
      } catch (error) {
        console.log('error', error);
      }
    }

    const _result = {};
    _result["file"] = fileName;
    _result["max_time"] = Math.max(...time);
    _result["min_time"] = Math.min(...time);
    _result["average_time"] = time.reduce((a, b) => a + b, 0) / time.length;
    _result["percentage_95"] = time.sort((a, b) => a - b)[Math.floor(time.length * 0.95)-1];
    _result["percentage_99"] = time.sort((a, b) => a - b)[Math.floor(time.length * 0.99)-1];
    result.push(_result);
    console.log(JSON.stringify(_result));
  }
  console.log(JSON.stringify(result));
};

func();