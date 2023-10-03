const env = process.env;

console.log(function () {
  const arch = env.npm_config_arch || process.arch;
  const platform = env.npm_config_platform || process.platform;
  const platformId = `${platform}_${arch}`;
  return platformId;
}())