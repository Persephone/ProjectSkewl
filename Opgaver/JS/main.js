// Opgave 1
var pris = 123;
var bet  = 200;
var retur = bet - pris;

const penge = [ 20, 10, 5, 2, 1 ];

const resultat = {};

penge.forEach(v => { 
    resultat[v] = Math.floor(retur / v);
    retur %= v;
});

console.table(resultat);