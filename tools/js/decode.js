var jwt = require('jsonwebtoken');

var token = 'eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9.eyJhdWQiOiJodHRwczovL2ZjbS5nb29nbGVhcGlzLmNvbSIsImV4cCI6MTUzMzMwOTI4MSwic3ViIjoiIn0.QOY2H--IY4ZSkqYq2zs3DFlAbc4J0acwm5x3kiAzIy1Jfde1SNMYVSFeSZpf5x7O4jw78nGBpL-JCLLrMthHYg';
var pk = 'BVf3ez_0L3n23aYx0zrSuwQO_kQ5oAvWL5eaj6QXL0E';
var pem = '-----BEGIN PUBLIC KEY-----\r\n' +
  'MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEk29sq1e1gTs+QdUvNdGgNHfnmTPE\r\n' +
  'nG0tVV+94CmDFy+TqBp/kSAaq08exTTLbe1n9XA3oyXOn2nl2/pnDz1GVA==\r\n' +
  '-----END PUBLIC KEY-----';
// var pem = '-----BEGIN PUBLIC KEY-----\r\nBJNvbKtXtYE7PkHVLzXRoDR355kzxJxtLVVfveApgxcvk6gaf5EgGqtPHsU0y23tZ_VwN6Mlzp9p5dv6Zw89RlQ==\r\n-----END PUBLIC KEY-----\r\n';
// get the decoded payload ignoring signature, no secretOrPrivateKey needed
var decoded = jwt.decode(token);

// get the decoded payload and header
var decoded = jwt.decode(token, {complete: true});
console.log('header: ');
console.log(decoded.header);
console.log('payload: ');
console.log(decoded.payload);

console.log('verify');
console.log(pem);

jwt.verify(token, pem, function(err, decoded) {
  console.log(decoded);
});
