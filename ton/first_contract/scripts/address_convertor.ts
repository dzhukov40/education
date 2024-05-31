import { Address } from "ton-core";


const address1 = Address.parse('0:a3935861f79daf59a13d6d182e1640210c02f98e3df18fda74b8f5ab141abf18');

// toStrings arguments: urlSafe, bounceable, testOnly
// defaults values: true, true, false

console.log(address1.toString());
console.log(address1.toRawString());

