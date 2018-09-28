<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Document</title>

    <link rel="stylesheet" href="styles/main.css">
</head>

<body>
    <div id="container">
        <label for="numberbox">Vare Nummer:</label>
        <input id="numberbox" type="number" name="numberbox" placeholder="12345" autofocus>

        <label for="namebox">Vare Navn:</label>
        <input id="namebox" type="text" name="namebox" placeholder="Navn">

        <label for="pricebox">Vare Pris:</label>
        <input id="pricebox" type="number" name="pricebox" placeholder="Pris">
        <span id="error"></span>
    </div>
    
    <section id="box">
        <select id="mySelect" size="5">
        </select>
    </section>

    <section id="buttons">
        <input type="submit" value="Indsæt" onclick="add()">
        <input type="submit" value="Fjern" id="remove">
    </section>


    <script>
        var x = document.getElementById("mySelect");

        function add() {
            let option = document.createElement("option");
            let number = parseInt(document.getElementById("numberbox").value);
            let name   = document.getElementById("namebox").value;
            let price  = parseInt(document.getElementById("pricebox").value);
            let error  = document.getElementById("error");

            if (isNaN(price) || price < 0 || price > 1000) {
                error.innerHTML = "Prisen skal være større end 0 og mindre end 1000.";
                return;
            }

            error.innerHTML = "";

            option.text = `${number} + ${name} + ${price}`;
            x.add(option);
        }

        const FjernBruger = document.getElementById("remove");
        FjernBruger.addEventListener("click", function () {
            let bruger = x.options[x.selectedIndex].value;
            x.remove(bruger);
        })
    </script>
</body>

</html>