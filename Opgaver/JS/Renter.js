// K = K0 * rente^år
// K = k0 * Math.pow(rente, 2)
function UdregnRente(startBeløb, rente, år) {

    return (startBeløb * Math.pow(1 + rente, år)).toFixed(2);
}

const RenteTable = document.getElementById("RenteTable");

var startBeløb = 100;
var rente = 0.05;
var antalTerminer = 10;
var totalRente = 0;

for (let år = 0; år < antalTerminer; år++) {
    if (år > 0)
        totalRente += rente;
    RenteTable.innerHTML += `
            <tr>
                <td>${år}</td>
                <td>${UdregnRente(startBeløb, rente, år)}</td>
                <td>${(totalRente.toFixed(2) * 100)}</td>
            </tr>
            `;
}