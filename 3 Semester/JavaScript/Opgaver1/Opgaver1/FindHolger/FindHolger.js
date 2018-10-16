const field  = document.getElementById("field");
const square = 5;

field.innerText = "";

for (let i = 0; i < square; i++) {
    const tr = document.createElement("tr");

    for (let j = 0; j < square; j++) {
        const td = document.createElement("td");
        td.classList += "square";
        td.setAttribute("data-row", i);
        td.setAttribute("data-column", j);
        td.bgColor = getRandomColor();
        td.innerText += "X";

        tr.appendChild(td);
    }

    field.appendChild(tr);
}

const fields = document.querySelectorAll(".square");
const selectedField = fields[Math.floor(Math.random() * fields.length)];
selectedField.innerHTML = '<img src="https://uwm.edu/chemistry/wp-content/uploads/sites/194/2014/12/holger.jpg" alt="" border=3 height=80% width=80%></img>';

function getRandomColor() {
    var letters = '0123456789ABCDEF';
    var color = '#';
    for (var i = 0; i < 6; i++) {
      color += letters[Math.floor(Math.random() * 16)];
    }
    return color;
  }