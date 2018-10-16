// Your code here!
var pris = 123;
var bet  = 200;
var retur = bet - pris;

var a20 = retur % 20;
var a10 = retur % 10;
var a5  = retur % 5;
var a2  = retur % 2;
var a1  = retur % 1;

console.log(a20 + "\n" + a10 + "\n" + a5 + "\n" + a2 + "\n" + a1);