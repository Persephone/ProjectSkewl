var striplength = 0.0001;
var a = 1;
var b = 2;

const f = x => x * x;

/*function f(x) { same shit
    return x * x;
}*/


function areaUnderFunction (func, start, end, precision) {
    let result = 0;

    for (let increment = start; increment < end; increment += precision)
        result += func(increment) * striplength;

    return result;
}

console.log(areaUnderFunction(f, a, b, striplength));
