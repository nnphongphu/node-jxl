//index.js
const testAddon = require('./build/Release/libjxl.node');

console.log('addon', testAddon);

const fs = require('fs');

const wrapper = (buffer) => {
  return new Promise((resolve, reject) => {
    testAddon.jxlToJpeg({ buffer }, (error, data) => {
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

const func = async () => {
  const files = fs.readdirSync('./test/input');

  const result = [];
  for (const fileName of files) {
    const fileUrl = `./test/input/${fileName}`;
    const time = [];
    for (let i = 1; i <= 30; i++) {
      const buffer = fs.readFileSync(fileUrl);
      try {
        const start =  process.hrtime();
        const image = await wrapper(buffer);
        const end = process.hrtime();
        if (i == 1) await writeFile(`./test/output/${fileName.replace(".jxl", "")}.jpg`, image);
        time.push((end[0] * 1000000000 + end[1] - start[0] * 1000000000 - start[1]) / 1000000);
      } catch (error) {
        console.log('error', error);
      }
    }

    const _result = {};
    _result["file"] = fileName;
    _result["max_time"] = Math.max(...time);
    _result["min_time"] = Math.min(...time);
    _result["average_time"] = time.reduce((a, b) => a + b, 0) / time.length;
    _result["percentage_95"] = time.sort((a, b) => a - b)[Math.floor(time.length * 0.95)];
    _result["percentage_99"] = time.sort((a, b) => a - b)[Math.floor(time.length * 0.99)];
    result.push(_result);
    console.log(JSON.stringify(_result));
  }
  console.log(JSON.stringify(result));
};

func();